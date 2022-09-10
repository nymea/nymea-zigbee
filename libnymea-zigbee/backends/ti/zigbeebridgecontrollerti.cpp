/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2022, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea-zigbee.
* This project including source code and documentation is protected by copyright law, and
* remains the property of nymea GmbH. All rights, including reproduction, publication,
* editing and translation, are reserved. The use of this project is subject to the terms of a
* license agreement to be concluded with nymea GmbH in accordance with the terms
* of use of nymea GmbH, available under https://nymea.io/license
*
* GNU Lesser General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the terms of the GNU
* Lesser General Public License as published by the Free Software Foundation; version 3.
* this project is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License along with this project.
* If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under contact@nymea.io
* or see our FAQ/Licensing Information on https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "zigbeeutils.h"
#include "loggingcategory.h"
#include "zigbeechannelmask.h"
#include "zdo/zigbeedeviceprofile.h"
#include "zigbeebridgecontrollerti.h"

#include <QDataStream>
#include <QMetaEnum>

#define NEW_PAYLOAD QByteArray payload; QDataStream stream(&payload, QIODevice::WriteOnly); stream.setByteOrder(QDataStream::LittleEndian);
#define PAYLOAD_STREAM(x) QDataStream stream(x); stream.setByteOrder(QDataStream::LittleEndian);

ZigbeeBridgeControllerTi::ZigbeeBridgeControllerTi(QObject *parent) :
    ZigbeeBridgeController(parent)
{
    m_interface = new ZigbeeInterfaceTi(this);
    connect(m_interface, &ZigbeeInterfaceTi::availableChanged, this, &ZigbeeBridgeControllerTi::onInterfaceAvailableChanged);
    connect(m_interface, &ZigbeeInterfaceTi::packetReceived, this, &ZigbeeBridgeControllerTi::onInterfacePacketReceived);

    m_permitJoinTimer.setSingleShot(true);
    connect(&m_permitJoinTimer, &QTimer::timeout, this, [=]{emit permitJoinStateChanged(0);});
}

ZigbeeBridgeControllerTi::~ZigbeeBridgeControllerTi()
{
    qCDebug(dcZigbeeController()) << "Destroying controller";
}

TiNetworkConfiguration ZigbeeBridgeControllerTi::networkConfiguration() const
{
    return m_networkConfiguration;
}

ZigbeeInterfaceTiReply *ZigbeeBridgeControllerTi::setLed(bool on)
{
    NEW_PAYLOAD;
    stream << static_cast<quint8>(0x03); // LED ID
    stream << static_cast<quint8>(on);
    return sendCommand(Ti::SubSystemUtil, Ti::UtilCommandLedControl, payload);
}

ZigbeeInterfaceTiReply* ZigbeeBridgeControllerTi::reset()
{
    NEW_PAYLOAD
    stream << static_cast<quint8>(Ti::ResetTypeSoft);
    ZigbeeInterfaceTiReply *resetReply = sendCommand(Ti::SubSystemSys, Ti::SYSCommandResetReq, payload);
    waitFor(resetReply, Ti::SubSystemSys, Ti::SYSCommandResetInd);
    return resetReply;
}

ZigbeeInterfaceTiReply *ZigbeeBridgeControllerTi::init()
{
    ZigbeeInterfaceTiReply *initReply = new ZigbeeInterfaceTiReply(this, 15000);

    ZigbeeInterfaceTiReply *resetReply = reset();
    connect(resetReply, &ZigbeeInterfaceTiReply::finished, initReply, [=]() {

        qCDebug(dcZigbeeController()) << "Skipping CC2530/CC2531 bootloader.";
        m_interface->sendMagicByte();

        QTimer::singleShot(1000, initReply, [=]{
            qCDebug(dcZigbeeController()) << "Trying to ping controller.";
            ZigbeeInterfaceTiReply *pingReply = sendCommand(Ti::SubSystemSys, Ti::SYSCommandPing, QByteArray(), 1000);
            connect(pingReply, &ZigbeeInterfaceTiReply::finished, initReply, [=]() {
                if (pingReply->statusCode() != Ti::StatusCodeSuccess) {
                    qCWarning(dcZigbeeController()) << "Error pinging controller.";

                    qCDebug(dcZigbeeInterface()) << "Skipping CC2652/CC1352 bootloader.";
                    m_interface->setDTR(false);
                    m_interface->setRTS(false);
                    QTimer::singleShot(150, initReply, [=]{
                        m_interface->setRTS(true);
                        QTimer::singleShot(150, initReply, [=]{
                            m_interface->setRTS(false);
                            QTimer::singleShot(150, initReply, [=]{
                                initPhase2(initReply, 0);
                            });
                        });
                    });
                    return;
                }

                qCDebug(dcZigbeeController()) << "Controller ping succeeded.";
                initPhase2(initReply, 0);
            });
        });
    });

    return initReply;
}

void ZigbeeBridgeControllerTi::initPhase2(ZigbeeInterfaceTiReply *initReply, int attempt)
{
    qCDebug(dcZigbeeController()) << "Trying to ping controller... (" << (attempt + 1) << "/ 10 )";
    ZigbeeInterfaceTiReply *pingReply = sendCommand(Ti::SubSystemSys, Ti::SYSCommandPing, QByteArray(), 1000);

    connect(pingReply, &ZigbeeInterfaceTiReply::finished, initReply, [=]() {
        if (pingReply->statusCode() != Ti::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Error pinging controller.";
            if (attempt < 9) {
                initPhase2(initReply, attempt+1);
            } else {
                qCWarning(dcZigbeeController()) << "Giving up...";
                initReply->finish(Ti::StatusCodeFailure);
            }
            return;
        }

        PAYLOAD_STREAM(pingReply->responsePayload());
        quint16 caps;
        stream >> caps;
        Ti::ControllerCapabilities capabilities = static_cast<Ti::ControllerCapabilities>(caps);
        qCDebug(dcZigbeeController()) << "Controller ping succeeded! Capabilities:" << capabilities;

        Ti::ControllerCapabilities requiredCapabilities = Ti::ControllerCapabilityNone;
        requiredCapabilities |= Ti::ControllerCapabilitySys;
        requiredCapabilities |= Ti::ControllerCapabilityUtil;
        requiredCapabilities |= Ti::ControllerCapabilityZDO;
        requiredCapabilities |= Ti::ControllerCapabilityAF;

        if ((capabilities & requiredCapabilities) != requiredCapabilities) {
            qCCritical(dcZigbeeController()) << "Controller doesn't support all required capabilities:" << capabilities;
            initReply->finish(Ti::StatusCodeUnsupported);
            return;
        }

        qCDebug(dcZigbeeController()) << "Fetching firmware information...";
        ZigbeeInterfaceTiReply *versionReply = sendCommand(Ti::SubSystemSys, Ti::SYSCommandVersion);
        connect(versionReply, &ZigbeeInterfaceTiReply::finished, initReply, [=]() {
            if (versionReply->statusCode() != Ti::StatusCodeSuccess) {
                qCWarning(dcZigbeeInterface()) << "Error reading controller version";
                initReply->finish(versionReply->statusCode());
                return;
            }
            PAYLOAD_STREAM(versionReply->responsePayload());
            quint8 transportRevision, product, majorRelease, minorRelease, maintRelease;
            quint32 revision;
            stream >> transportRevision >> product >> majorRelease >> minorRelease >> maintRelease >> revision;
            qCDebug(dcZigbeeNetwork()).nospace().noquote() << "Controller versions: Transport rev: " << transportRevision << " Product: " << product
                                                           << " Version: " << majorRelease << "." << minorRelease << "." << maintRelease
                                                           << " Revision: " << revision;

            m_networkConfiguration.znpVersion = static_cast<Ti::ZnpVersion>(product);
            setFirmwareVersion(QString("%0(%1) - %2.%3.%4.%5")
                               .arg(QMetaEnum::fromType<Ti::ZnpVersion>().valueToKey(product))
                               .arg(transportRevision)
                               .arg(majorRelease)
                               .arg(minorRelease)
                               .arg(maintRelease)
                               .arg(revision));

            qCDebug(dcZigbeeController()) << "Reading IEEE address";

            ZigbeeInterfaceTiReply *getIeeeAddrReply = readNvItem(Ti::NvItemIdPanId);
            connect(getIeeeAddrReply, &ZigbeeInterfaceTiReply::finished, initReply, [=](){

                ZigbeeInterfaceTiReply *getExtAddrReply = sendCommand(Ti::SubSystemSys, Ti::SYSCommandGetExtAddress);
                connect(getExtAddrReply, &ZigbeeInterfaceTiReply::finished, initReply, [=](){
                    if (getExtAddrReply->statusCode() != Ti::StatusCodeSuccess) {
                        qCWarning(dcZigbeeController()) << "Call to getDeviceInfo failed:" << getExtAddrReply->statusCode();
                        initReply->finish(getExtAddrReply->statusCode());
                        return;
                    }

                    PAYLOAD_STREAM(getExtAddrReply->responsePayload());
                    quint64 ieeeAddress;
                    stream >> ieeeAddress;
                    m_networkConfiguration.ieeeAddress = ZigbeeAddress(ieeeAddress);
                    qCDebug(dcZigbeeController()) << "IEEE address:" << m_networkConfiguration.ieeeAddress.toString();
                    initReply->finish();

                    m_controllerState = ControllerStateInitialized;
                    emit controllerStateChanged(ControllerStateInitialized);
                });
            });
        });
    });
}


ZigbeeInterfaceTiReply *ZigbeeBridgeControllerTi::commission(Ti::DeviceLogicalType deviceType, quint16 panId, const ZigbeeChannelMask &channelMask)
{
    ZigbeeInterfaceTiReply *reply = new ZigbeeInterfaceTiReply(this, 30000);

    ZigbeeInterfaceTiReply *resetReply = factoryReset();
    connect(resetReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){

        // Make sure the controller is set to normal startup mode, so it will keep the commissioned settings on next reboot
        NEW_PAYLOAD;
        stream << static_cast<quint8>(Ti::StartupModeNormal);
        ZigbeeInterfaceTiReply *startupOptionReply = writeNvItem(Ti::NvItemIdStartupOption, payload);
        connect(startupOptionReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){

            NEW_PAYLOAD;
            stream << static_cast<quint8>(deviceType);
            ZigbeeInterfaceTiReply *deviceTypeReply = writeNvItem(Ti::NvItemIdLogicalType, payload);
            connect(deviceTypeReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){

                NEW_PAYLOAD;
                stream << static_cast<quint8>(0x01);
                ZigbeeInterfaceTiReply *deviceTypeReply = writeNvItem(Ti::NvItemIdZdoDirectCb, payload);
                connect(deviceTypeReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){

                    NEW_PAYLOAD;
                    stream << panId;
                    ZigbeeInterfaceTiReply *panIdReply = writeNvItem(Ti::NvItemIdPanId, payload);
                    connect(panIdReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){

                        NEW_PAYLOAD;
                        stream << ZigbeeUtils::generateRandomPanId();
                        stream << ZigbeeUtils::generateRandomPanId();
                        stream << ZigbeeUtils::generateRandomPanId();
                        stream << ZigbeeUtils::generateRandomPanId();
                        ZigbeeInterfaceTiReply *panIdReply = writeNvItem(Ti::NvItemIdExtendedPanId, payload);
                        connect(panIdReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){
                            ZigbeeInterfaceTiReply *panIdReply = writeNvItem(Ti::NvItemIdApsUseExtPanId, payload);
                            connect(panIdReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){

                                NEW_PAYLOAD;
                                stream << channelMask.toUInt32();
                                ZigbeeInterfaceTiReply *channelsReply = writeNvItem(Ti::NvItemIdChanList, payload);
                                connect(channelsReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){

                                    // TODO: commission nwk key
                                    // The adapter will generate a key, but we could provision our own so we could re-apply when restoring a backup
//                                    NEW_PAYLOAD;
//                                    stream << static_cast<quint8>(0x01);
//                                    ZigbeeInterfaceTiReply *channelsReply = writeNvItem(Ti::NvItemIdPreCfgKeysEnable, payload);
//                                    connect(channelsReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){

//                                        NEW_PAYLOAD;
//                                        stream << <128 bit data>;
//                                        ZigbeeInterfaceTiReply *channelsReply = writeNvItem(Ti::NvItemIdPreCfgKey, payload);
//                                        connect(channelsReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){

                                            // For zStack12 we're done here.
                                            if (m_networkConfiguration.znpVersion == Ti::zStack12) {
                                                reply->finish();
                                                return;
                                            }

                                            // zStack3x requires channels to be commissioned via AppCnf subsystem BdbCommissioning
                                            NEW_PAYLOAD;
                                            stream << static_cast<quint8>(1); // Primary channel
                                            stream << static_cast<quint32>(channelMask.toUInt32());
                                            ZigbeeInterfaceTiReply *commissionReply = sendCommand(Ti::SubSystemAppCnf, Ti::AppCnfCommandBdbSetChannel, payload);
                                            connect(commissionReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){

                                                NEW_PAYLOAD;
                                                stream << static_cast<quint8>(0); // Non-primary channel
                                                stream << static_cast<quint32>(0);
                                                ZigbeeInterfaceTiReply *commissionReply = sendCommand(Ti::SubSystemAppCnf, Ti::AppCnfCommandBdbSetChannel, payload);
                                                connect(commissionReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){

                                                    NEW_PAYLOAD;
                                                    stream << static_cast<quint8>(0x04);;
                                                    ZigbeeInterfaceTiReply *commissionReply = sendCommand(Ti::SubSystemAppCnf, Ti::AppCnfCommandBdbStartCommissioning, payload);
                                                    connect(commissionReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){
                                                        reply->finish();
                                                    });
                                                });
                                            });
//                                        });
//                                    });
                                });
                            });
                        });
                    });
                });
            });
        });
    });
    return reply;
}

void ZigbeeBridgeControllerTi::postStartup()
{    
    // Reading Network Info
    ZigbeeInterfaceTiReply *networkInfoReply = sendCommand(Ti::SubSystemZDO, Ti::ZDOCommandExtNwkInfo);
    connect(networkInfoReply, &ZigbeeInterfaceTiReply::finished, this, [=](){
        if (networkInfoReply->statusCode() != Ti::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Failed to read network info" << networkInfoReply->statusCode();
            return;
        }
        quint8 devState, channel;
        quint16 shortAddr, panId, parentAddr;
        quint64 extendedPanId, parentExtAddr;
        {
            PAYLOAD_STREAM(networkInfoReply->responsePayload());
            stream >> shortAddr >> devState >> panId >> parentAddr >> extendedPanId >> parentExtAddr >> channel;
        }

        m_networkConfiguration.panId = panId;
        m_networkConfiguration.extendedPanId = extendedPanId;
        m_networkConfiguration.currentChannel = channel;
        m_networkConfiguration.shortAddress = shortAddr;
        qCDebug(dcZigbeeController()) << "PAN ID:" << ZigbeeUtils::convertUint16ToHexString(m_networkConfiguration.panId);
        qCDebug(dcZigbeeController()) << "Short addr:" << ZigbeeUtils::convertUint16ToHexString(m_networkConfiguration.shortAddress);
        qCDebug(dcZigbeeController()) << "Extended Pan ID:" << ZigbeeUtils::convertUint64ToHexString(m_networkConfiguration.extendedPanId);
        qCDebug(dcZigbeeController()) << "Device state:" << devState;
        qCDebug(dcZigbeeController()) << "Channel:" << channel;
        qCDebug(dcZigbeeController()) << "IEEE address:" << m_networkConfiguration.ieeeAddress.toString();

        // Registering for the ZDO raw message callback
        NEW_PAYLOAD;
        stream << static_cast<quint16>(ZigbeeClusterLibrary::ClusterIdUnknown);
        ZigbeeInterfaceTiReply *registerCallbackReply = sendCommand(Ti::SubSystemZDO, Ti::ZDOCommandMsgCbRegister, payload);
        connect(registerCallbackReply, &ZigbeeInterfaceTiReply::finished, this, [=](){
            if (registerCallbackReply->statusCode() != Ti::StatusCodeSuccess) {
                qCWarning(dcZigbeeInterface()) << "Failed to register ZDO msg callback";
                return;
            }
            qCDebug(dcZigbeeController()) << "ZDO message callback registered";

            // Fetching active endpoints from controller
            ZigbeeInterfaceTiReply *reply = new ZigbeeInterfaceTiReply(this);
            NEW_PAYLOAD;
            stream << static_cast<quint16>(0x0000); // dstaddr
            stream << static_cast<quint16>(0x0000); // networkOfInterest
            sendCommand(Ti::SubSystemZDO, Ti::ZDOCommandActiveEpReq, payload);
            waitFor(reply, Ti::SubSystemZDO, Ti::ZDOCommandActiveEpRsp);
            connect(reply, &ZigbeeInterfaceTiReply::finished, this, [=](){
                PAYLOAD_STREAM(reply->responsePayload());
                quint8 status, activeEpCount;
                quint16 srcAddr, nwkAddr;
                stream >> srcAddr >> status >> nwkAddr >> activeEpCount;

                for (int i = 0; i < activeEpCount; i++) {
                    quint8 endpointId;
                    stream >> endpointId;
                    m_registeredEndpointIds.append(endpointId);
                }

                m_controllerState = ControllerStateRunning;
                emit controllerStateChanged(ControllerStateRunning);
            });

        });
    });
}

ZigbeeInterfaceTiReply *ZigbeeBridgeControllerTi::factoryReset()
{
    ZigbeeInterfaceTiReply *reply = new ZigbeeInterfaceTiReply(this);

    // Setting startup option to 4 to perform a clear on reset
    // Sending a reset request
    // Setting startup option back to 0 to boot into normal mode again

    ZigbeeInterfaceTiReply *deleteNIBReply = deleteNvItem(Ti::NvItemIdNIB);
    connect(deleteNIBReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){

        NEW_PAYLOAD;
        stream << (quint8)Ti::StartupModeClean;
        ZigbeeInterfaceTiReply *writeStartupOptionReply = writeNvItem(Ti::NvItemIdStartupOption, payload);
        connect(writeStartupOptionReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){
            if (writeStartupOptionReply->statusCode() != Ti::StatusCodeSuccess) {
                reply->finish(writeStartupOptionReply->statusCode());
                return;
            }

            ZigbeeInterfaceTiReply *resetReply = reset();
            connect(resetReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){

                NEW_PAYLOAD;
                stream << (quint8)Ti::StartupModeNormal;
                ZigbeeInterfaceTiReply *writeStartupOptionReply = writeNvItem(Ti::NvItemIdStartupOption, payload);
                connect(writeStartupOptionReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){
                    reply->finish(writeStartupOptionReply->statusCode());
                });
            });
        });
    });

    return reply;
}

ZigbeeInterfaceTiReply *ZigbeeBridgeControllerTi::requestSendRequest(const ZigbeeNetworkRequest &request)
{
    Ti::TxOptions tiTxOptions = Ti::TxOptionNone;
    tiTxOptions |= (request.txOptions().testFlag(Zigbee::ZigbeeTxOptionAckTransmission) ? Ti::TxOptionApsAck : Ti::TxOptionNone);
    tiTxOptions |= (request.txOptions().testFlag(Zigbee::ZigbeeTxOptionSecurityEnabled) ? Ti::TxOptionApsSecurity : Ti::TxOptionNone);

    NEW_PAYLOAD;
    stream << static_cast<quint8>(request.destinationAddressMode());
    if (request.destinationAddressMode() == Zigbee::DestinationAddressModeIeeeAddress) {
        stream << request.destinationIeeeAddress().toUInt64();
    } else {
        stream << static_cast<quint64>(request.destinationShortAddress());
    }

    stream << request.destinationEndpoint();
    stream << static_cast<quint16>(0x0000); // Intra-pan
    stream << request.sourceEndpoint();
    stream << request.clusterId();
    stream << request.requestId();
    stream << static_cast<quint8>(tiTxOptions);
    stream << request.radius();
    stream << static_cast<quint16>(request.asdu().length());

    QByteArray asdu = request.asdu();

    // If the the entire packet fits into the MTU, can send it as is
    // otherwise we'll have to send the packet without payload and provide the payload using StoreData instead
    if (asdu.length() < MT_RPC_DATA_MAX - 20) {
        for (int i = 0; i < asdu.length(); i++) {
            stream << static_cast<quint8>(asdu.at(i));
        }
        return sendCommand(Ti::SubSystemAF, Ti::AFCommandDataRequestExt, payload);
    }

    // NOTE: Leaving those prints as warnings for now as I didn't get the chance to test this much
    // so if anything goes wrong, it would appear in the logs unconditionally.
    qCWarning(dcZigbeeController()) << "Splitting huge packet into chunks!";
    qCWarning(dcZigbeeController()) << "Full packet payload:" << asdu.toHex() << "LEN:" << asdu.length();
    ZigbeeInterfaceTiReply *lastReply = nullptr;
    int i = 0;
    while (!asdu.isEmpty()) {
        QByteArray chunk = asdu.left(qMin(asdu.length(), 252));
        asdu.remove(0, chunk.size());
        qCWarning(dcZigbeeController()) << "Chunk" << i << ":" << chunk.toHex() << "LEN:" << chunk.length();

        NEW_PAYLOAD;
        stream << static_cast<quint16>(i++);
        stream << static_cast<quint8>(chunk.length());
        lastReply = sendCommand(Ti::SubSystemAF, Ti::AFCommandDataStore, chunk);
    }
    return lastReply;
}

void ZigbeeBridgeControllerTi::sendNextRequest()
{
    // Check if there is a reply request to send
    if (m_replyQueue.isEmpty())
        return;

    // Check if there is currently a running reply
    if (m_currentReply)
        return;

    m_currentReply = m_replyQueue.dequeue();
    qCDebug(dcZigbeeController()) << "-->" << m_currentReply->subSystem() << QHash<Ti::SubSystem, QMetaEnum>({
            { Ti::SubSystemSys, QMetaEnum::fromType<Ti::SYSCommand>() },
            { Ti::SubSystemMAC, QMetaEnum::fromType<Ti::MACCommand>() },
            { Ti::SubSystemAF, QMetaEnum::fromType<Ti::AFCommand>() },
            { Ti::SubSystemZDO, QMetaEnum::fromType<Ti::ZDOCommand>() },
            { Ti::SubSystemSAPI, QMetaEnum::fromType<Ti::SAPICommand>() },
            { Ti::SubSystemUtil, QMetaEnum::fromType<Ti::UtilCommand>() },
            { Ti::SubSystemDebug, QMetaEnum::fromType<Ti::DebugCommand>() },
            { Ti::SubSystemApp, QMetaEnum::fromType<Ti::AppCommand>() },
            { Ti::SubSystemAppCnf, QMetaEnum::fromType<Ti::AppCnfCommand>() },
            { Ti::SubSystemGreenPower, QMetaEnum::fromType<Ti::GreenPowerCommand>() }
        }).value(m_currentReply->subSystem()).valueToKey(m_currentReply->command())
        << m_currentReply->requestPayload().toHex();

    m_interface->sendPacket(Ti::CommandTypeSReq, m_currentReply->subSystem(), m_currentReply->command(), m_currentReply->requestPayload());
    m_currentReply->m_timer->start();
}

ZigbeeInterfaceTiReply *ZigbeeBridgeControllerTi::sendCommand(Ti::SubSystem subSystem, quint8 command, const QByteArray &payload, int timeout)
{
    // Create the reply
    ZigbeeInterfaceTiReply *reply = new ZigbeeInterfaceTiReply(subSystem, command, this, payload, timeout);

    // Make sure we clean up on timeout
    connect(reply, &ZigbeeInterfaceTiReply::timeout, this, [reply](){
        qCWarning(dcZigbeeController()) << "Reply timeout" << reply;
        // Note: send next reply with the finished signal
    });

    // Auto delete the object on finished
    connect(reply, &ZigbeeInterfaceTiReply::finished, reply, [this, reply](){
        if (m_currentReply == reply) {
            m_currentReply = nullptr;
            QMetaObject::invokeMethod(this, "sendNextRequest", Qt::QueuedConnection);
        }
    });

    m_replyQueue.enqueue(reply);

    QMetaObject::invokeMethod(this, "sendNextRequest", Qt::QueuedConnection);
    return reply;
}

ZigbeeInterfaceTiReply *ZigbeeBridgeControllerTi::readNvItem(Ti::NvItemId itemId, quint16 offset)
{
    NEW_PAYLOAD;
    stream << static_cast<quint16>(itemId);
    stream << offset;
    return sendCommand(Ti::SubSystemSys, Ti::SYSCommandOsalNvReadExt, payload);
}

ZigbeeInterfaceTiReply *ZigbeeBridgeControllerTi::writeNvItem(Ti::NvItemId itemId, const QByteArray &data, quint16 offset)
{
    qCDebug(dcZigbeeController()) << "Writing NV item:" << itemId << data.toHex();
    NEW_PAYLOAD;
    stream << static_cast<quint16>(itemId);
    stream << offset;
    stream << static_cast<quint16>(data.length());
    payload.append(data);
    return sendCommand(Ti::SubSystemSys, Ti::SYSCommandOsalNvWriteExt, payload);
}

ZigbeeInterfaceTiReply *ZigbeeBridgeControllerTi::deleteNvItem(Ti::NvItemId itemId)
{
    qCDebug(dcZigbeeController()) << "Deleting NV item:" << itemId;
    ZigbeeInterfaceTiReply *reply = new ZigbeeInterfaceTiReply(this);

    NEW_PAYLOAD;
    stream << static_cast<quint16>(itemId);
    ZigbeeInterfaceTiReply *getLengthReply = sendCommand(Ti::SubSystemSys, Ti::SYSCommandOsalNvLength, payload);
    connect(getLengthReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){
        if (getLengthReply->statusCode() != Ti::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Error getting NV item length.";
            reply->finish(getLengthReply->statusCode());
            return;
        }
        quint16 length;
        {
            PAYLOAD_STREAM(getLengthReply->responsePayload());
            stream >> length;
        }

        NEW_PAYLOAD;
        stream << static_cast<quint16>(itemId);
        stream << length;
        ZigbeeInterfaceTiReply *deleteReply = sendCommand(Ti::SubSystemSys, Ti::SYSCommandOsalNvDelete, payload);
        connect(deleteReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){
            if (deleteReply->statusCode() != Ti::StatusCodeSuccess) {
                qCWarning(dcZigbeeController()) << "Error deleting NV item";
            }
            reply->finish(deleteReply->statusCode());
        });
    });
    return reply;
}

void ZigbeeBridgeControllerTi::retrieveHugeMessage(const Zigbee::ApsdeDataIndication &pendingIndication, quint32 timestamp, quint16 dataLength)
{
    // Suppressing clang analyzer warning about leaking "indication", since we'll actually
    // clean it up in the capturing lambda when the last request finishes.
#ifndef __clang_analyzer__
    Zigbee::ApsdeDataIndication *indication = new Zigbee::ApsdeDataIndication(pendingIndication);
#endif

    quint8 chunkSize = 0;
    quint16 maxChunkSize = 253;
    for (quint16 i = 0; i * maxChunkSize < dataLength; i++) {
        chunkSize = qMin(maxChunkSize, static_cast<quint16>(dataLength - i));
        NEW_PAYLOAD;
        stream << timestamp;
        stream << i;
        stream << chunkSize;
        ZigbeeInterfaceTiReply *reply = sendCommand(Ti::SubSystemAF, Ti::AFCommandDataRetrieve, payload);
        // Note, capturing copies of i and chunksize, but a
        connect(reply, &ZigbeeInterfaceTiReply::finished, this, [this, reply, indication, i, maxChunkSize, dataLength](){
            PAYLOAD_STREAM(reply->responsePayload());
            quint8 status, len;
            stream >> status >> len;
            if (status != 0x00) {
                qCWarning(dcZigbeeController()) << "Failed to retrieve large payload chunk!" << status;
            }
            indication->asdu.append(reply->responsePayload().right(len));

            if (i * maxChunkSize >= dataLength) {
                // This is the last one...
                emit apsDataIndicationReceived(*indication);
                delete indication;
            }
        });
    }
}

ZigbeeInterfaceTiReply *ZigbeeBridgeControllerTi::start()
{
    NEW_PAYLOAD;
    stream << static_cast<quint16>(100); // Startup delay
    return sendCommand(Ti::SubSystemZDO, Ti::ZDOCommandStartupFromApp, payload);
}

ZigbeeInterfaceTiReply *ZigbeeBridgeControllerTi::registerEndpoint(quint8 endpointId, Zigbee::ZigbeeProfile profile, quint16 deviceId, quint8 deviceVersion)
{
    if (m_registeredEndpointIds.contains(endpointId)) {
        ZigbeeInterfaceTiReply *reply = new ZigbeeInterfaceTiReply(this);
        QTimer::singleShot(0, reply, [=](){
            reply->finish(Ti::StatusCodeSuccess);
        });
        return reply;
    }

    NEW_PAYLOAD;
    stream << endpointId;
    stream << static_cast<quint16>(profile);
    stream << deviceId;
    stream << deviceVersion;
    stream << static_cast<quint8>(0x00); // latency requirement
    stream << static_cast<quint8>(0x00); // num input clusters
//    stream << static_cast<quint16>(0x0000); // input clusters
    stream << static_cast<quint8>(0x00); // num outout clusters
//    stream << static_cast<quint16>(0x0000); // output clusters


    ZigbeeInterfaceTiReply *reply = sendCommand(Ti::SubSystemAF, Ti::AFCommandRegister, payload);
    connect(reply, &ZigbeeInterfaceTiReply::finished, this, [=](){
        PAYLOAD_STREAM(reply->responsePayload());
        quint8 status;
        stream >> status;
        if (status == Ti::StatusCodeSuccess) {
            m_registeredEndpointIds.append(endpointId);
        }
        reply->m_statusCode = static_cast<Ti::StatusCode>(status);
    });
    return reply;
}

ZigbeeInterfaceTiReply *ZigbeeBridgeControllerTi::addEndpointToGroup(quint8 endpointId, quint16 groupId)
{
    ZigbeeInterfaceTiReply *reply = new ZigbeeInterfaceTiReply(this);
    NEW_PAYLOAD;
    stream << endpointId;
    stream << groupId;
    stream << static_cast<quint8>(0x00); // Group name length
    ZigbeeInterfaceTiReply *findGroupReply = sendCommand(Ti::SubSystemZDO, Ti::ZDOCommandExtFindGroup, payload);
    connect(findGroupReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){
        quint8 status;
        PAYLOAD_STREAM(findGroupReply->responsePayload());
        stream >> status;
        if (status == 0x00) {
            qCDebug(dcZigbeeController()) << "Group already existing.";
            reply->finish();
        } else {
            NEW_PAYLOAD;
            stream << endpointId;
            stream << groupId;
            stream << static_cast<quint8>(0x00);
            ZigbeeInterfaceTiReply *addGroupReply = sendCommand(Ti::SubSystemZDO, Ti::ZDOCommandExtAddGroup, payload);
            connect(addGroupReply, &ZigbeeInterfaceTiReply::finished, reply, [=](){
                reply->finish(addGroupReply->statusCode());
            });
        }
    });
    return reply;
}

ZigbeeInterfaceTiReply *ZigbeeBridgeControllerTi::requestPermitJoin(quint8 seconds, const quint16 &networkAddress)
{
    NEW_PAYLOAD;
    stream << static_cast<quint8>(networkAddress == Zigbee::BroadcastAddressAllRouters ? 0x0F : 0x02);
    stream << static_cast<quint16>(networkAddress);
    stream << seconds;
    stream << static_cast<quint8>(0x00); // tcsignificance
    ZigbeeInterfaceTiReply *reply = sendCommand(Ti::SubSystemZDO, Ti::ZDOCommandMgmtPermitJoinReq, payload);

    ZigbeeInterfaceTiReply *waitForJoinRsp = new ZigbeeInterfaceTiReply(this);
    waitFor(waitForJoinRsp, Ti::SubSystemZDO, Ti::ZDOCommandMgmtPermitJoinRsp);
    connect(waitForJoinRsp, &ZigbeeInterfaceTiReply::finished, this, [=](){
        // zStack actually has an indication for permit join state changes which, when working,
        // gives the current permit join state and the remaining seconds.
        // Sadly, this doesn't seem to work for zStack3x0 and also seems a bit buggy on zStack12.
        // So instead of relying on that, let's try to stay in sync with a timer :/
        emit permitJoinStateChanged(seconds);
        m_permitJoinTimer.start(seconds * 1000);
    });

    return reply;
}

void ZigbeeBridgeControllerTi::waitFor(ZigbeeInterfaceTiReply *reply, Ti::SubSystem subSystem, quint8 command)
{
    WaitData waitData;
    waitData.subSystem = subSystem;
    waitData.command = command;

    m_waitFors.insert(reply, waitData);
    connect(reply, &ZigbeeInterfaceTiReply::finished, this, [=](){
        m_waitFors.remove(reply);
    });
}

void ZigbeeBridgeControllerTi::waitFor(ZigbeeInterfaceTiReply *reply, Ti::SubSystem subSystem, quint8 command, const QByteArray &payload)
{
    WaitData waitData;
    waitData.subSystem = subSystem;
    waitData.command = command;
    waitData.payload = payload;
    waitData.comparePayload = true;

    m_waitFors.insert(reply, waitData);
    connect(reply, &ZigbeeInterfaceTiReply::finished, this, [=](){
        m_waitFors.remove(reply);
    });
}

void ZigbeeBridgeControllerTi::onInterfaceAvailableChanged(bool available)
{
    qCDebug(dcZigbeeController()) << "Interface available changed" << available;
    if (!available) {
        // Clean up any pending replies
        while (!m_replyQueue.isEmpty()) {
            ZigbeeInterfaceTiReply *reply = m_replyQueue.dequeue();
            reply->abort();
        }
    }

    setAvailable(available);
    sendNextRequest();
}

void ZigbeeBridgeControllerTi::onInterfacePacketReceived(Ti::SubSystem subSystem, Ti::CommandType commandType, quint8 command, const QByteArray &payload)
{

    qCDebug(dcZigbeeController()) << "<--" << subSystem << commandType <<
    QHash<Ti::SubSystem, QMetaEnum>({
        { Ti::SubSystemSys, QMetaEnum::fromType<Ti::SYSCommand>() },
        { Ti::SubSystemMAC, QMetaEnum::fromType<Ti::MACCommand>() },
        { Ti::SubSystemAF, QMetaEnum::fromType<Ti::AFCommand>() },
        { Ti::SubSystemZDO, QMetaEnum::fromType<Ti::ZDOCommand>() },
        { Ti::SubSystemSAPI, QMetaEnum::fromType<Ti::SAPICommand>() },
        { Ti::SubSystemUtil, QMetaEnum::fromType<Ti::UtilCommand>() },
        { Ti::SubSystemDebug, QMetaEnum::fromType<Ti::DebugCommand>() },
        { Ti::SubSystemApp, QMetaEnum::fromType<Ti::AppCommand>() },
        { Ti::SubSystemAppCnf, QMetaEnum::fromType<Ti::AppCnfCommand>() },
        { Ti::SubSystemGreenPower, QMetaEnum::fromType<Ti::GreenPowerCommand>() }
    }).value(subSystem).valueToKey(command)
    << payload.toHex();

    if (commandType == Ti::CommandTypeSRsp) {
        if (m_currentReply && m_currentReply->command() == command) {
            m_currentReply->m_statusCode = Ti::StatusCodeSuccess;
            m_currentReply->m_responsePayload = payload;
            emit m_currentReply->finished();
        } else {
            qCWarning(dcZigbeeController()) << "Received a reply while not expecting it!";
        }
        return;
    }

    if (commandType == Ti::CommandTypeAReq) {

        switch (subSystem) {
        case Ti::SubSystemSys:
            switch (command) {
            case Ti::SYSCommandResetInd: {
                PAYLOAD_STREAM(payload);
                quint8 reason, transportRev, productId, majorRel, minorRel, hwRev;
                stream >> reason >> transportRev >> productId >> majorRel >> minorRel >> hwRev;
                qCDebug(dcZigbeeController()) << "Controller reset:" << static_cast<Ti::ResetReason>(reason);
                qCDebug(dcZigbeeController()) << "Transport revision:" << transportRev << "Product ID:" << productId << "Major:" << majorRel << "Minor:" << minorRel << "HW Rev:" << hwRev;
                break;
            }
            default:
                qCDebug(dcZigbeeController()) << "Unhandled system command";
            }
            break;
        case Ti::SubSystemZDO:
            switch (command) {
            case Ti::ZDOCommandStateChangeInd:
                qCDebug(dcZigbeeController()) << "Device state changed!" << payload.at(0);
                if (payload.at(0) == 0x09) {
                    // We're not emitting state changed right away, need to do some post setup routine
                    postStartup();
                }
                break;
            case Ti::ZDOCommandPermitJoinInd:
                qCDebug(dcZigbeeController()) << "Permit join indication" << payload.at(0);
                // Note: This doesn't seem to work for zStack3x0 and also is a bit buggy for zStack12
                // See requestPermitJoin() for the workaround.
                emit permitJoinStateChanged(payload.at(0));
                break;
            case Ti::ZDOCommandMgmtPermitJoinRsp:
                qCDebug(dcZigbeeController()) << "PermitJoinRsp received:" << payload;
                // Silencing this. We'll use PermitJoinInd to update the state as that indicates start and end
                // In theory we'd need to check if the network address in the payload matches with what we reqeusted....
                break;
            case Ti::ZDOCommandTcDeviceInd:
                qCDebug(dcZigbeeController()) << "Device join indication recived:" << payload;
                break;
            case Ti::ZDOCommandEndDeviceAnnceInd: {
                PAYLOAD_STREAM(payload);
                quint16 shortAddress, nwkAddress;
                quint64 ieeeAddress;
                quint8 capabilities;
                stream >> shortAddress >> nwkAddress >> ieeeAddress >> capabilities;
                qCDebug(dcZigbeeController()) << "End device announce indication received:" << payload;
                emit deviceIndication(shortAddress, ZigbeeAddress(ieeeAddress), capabilities);
                break;
            }
            case Ti::ZDOCommandLeaveInd: {
                PAYLOAD_STREAM(payload);
                quint16 srcAddr;
                quint64 srcIeeeAddr;
                quint8 request, remove, rejoin;
                stream >> srcAddr >> srcIeeeAddr >> request >> remove >> rejoin;
                emit nodeLeft(ZigbeeAddress(srcIeeeAddr), request, remove, rejoin);
                break;
            }
            case Ti::ZDOCommandNodeDescRsp:
            case Ti::ZDOCommandPowerDescRsp:
            case Ti::ZDOCommandSimpleDescRsp:
            case Ti::ZDOCommandActiveEpRsp:
            case Ti::ZDOCommandBindRsp:
                // silencing these as we're using the raw data in MsgCbIncoming instead
                // nymea-zigbee parses this on its own.
                break;
            case Ti::ZDOCommandMsgCbIncoming: {
                qCDebug(dcZigbeeController()) << "Incoming ZDO message:" << payload.toHex();
                quint16 srcAddr, clusterId, macDstAddr;
                quint8 wasBroadcast, securityInUse, transactionSequenceNumber;

                PAYLOAD_STREAM(payload);
                stream >> srcAddr;
                stream >> wasBroadcast;
                stream >> clusterId;
                stream >> securityInUse;
                stream >> transactionSequenceNumber;
                stream >> macDstAddr;

                QByteArray adpu = payload.right(payload.length() - 9);

                if (clusterId == ZigbeeDeviceProfile::ZdoCommand::DeviceAnnounce
                        || clusterId == ZigbeeDeviceProfile::ZdoCommand::MgmtPermitJoinResponse) {
                    // Silencing those as we're using the proper z-Stack API for them
                    qCDebug(dcZigbeeController()) << "Ignoring raw ZDO message for command" << static_cast<ZigbeeDeviceProfile::ZdoCommand>(clusterId);
                    return;
                }

                QByteArray asdu;
                QDataStream asduStream(&asdu, QIODevice::WriteOnly);
                asduStream.setByteOrder(QDataStream::LittleEndian);
                asduStream << transactionSequenceNumber;
                asdu.append(adpu);

                Zigbee::ApsdeDataIndication indication;
                indication.destinationAddressMode = Zigbee::DestinationAddressModeShortAddress;
                indication.sourceAddressMode = Zigbee::DestinationAddressModeShortAddress;
                indication.asdu = asdu;
                indication.clusterId = clusterId;
                indication.sourceShortAddress = srcAddr;
                emit apsDataIndicationReceived(indication);

                break;
            }
            case Ti::ZDOCommandSrcRtgInd: {
                PAYLOAD_STREAM(payload);
                quint16 srcAddr;
                quint8 relayCount;
                stream >> srcAddr;
                stream >> relayCount;
                QDebug dbg = qDebug(dcZigbeeController());
                dbg << "Node" << ZigbeeUtils::convertUint16ToHexString(srcAddr) << "is routed" << (relayCount == 0 ? "directly" : "via " + QString::number(relayCount) + " hops:" );
                for (int i = 0; i < relayCount; i++) {
                    quint16 relayAddr;
                    stream >> relayAddr;
                    dbg << ZigbeeUtils::convertUint16ToHexString(relayAddr);
                }
                break;
            }
            default:
                qCWarning(dcZigbeeController()) << "Unhandled ZDO AREQ notification";
            }
            break;
        case Ti::SubSystemAF:
            switch (command) {
            case Ti::AFCommandIncomingMsg: {
                quint8 srcEndpoint, dstEndpoint, wasBroadcast, lqi, securityUse, transactionSequenceNumber, dataLen, status;
                quint16 groupId, clusterId, srcAddr, addrOfInterest;
                quint32 timestamp;

                PAYLOAD_STREAM(payload);
                stream >> groupId;
                stream >> clusterId;
                stream >> srcAddr;
                stream >> srcEndpoint;
                stream >> dstEndpoint;
                stream >> wasBroadcast;
                stream >> lqi;
                stream >> securityUse;
                stream >> timestamp;
                stream >> transactionSequenceNumber;
                stream >> dataLen;

                QByteArray asdu;
                for (int i = 0; i < dataLen; i++) {
                    quint8 byte;
                    stream >> byte;
                    asdu.append(byte);
                }

                stream >> addrOfInterest;
                stream >> status;

                Zigbee::ApsdeDataIndication indication;
                indication.destinationAddressMode = Zigbee::DestinationAddressModeShortAddress;
                indication.sourceAddressMode = Zigbee::DestinationAddressModeShortAddress;
                indication.sourceEndpoint = srcEndpoint;
                indication.sourceShortAddress = srcAddr;
                indication.destinationEndpoint = dstEndpoint;
                indication.clusterId = clusterId;
                indication.profileId = Zigbee::ZigbeeProfileHomeAutomation;
                indication.lqi = lqi;
                indication.asdu = asdu;
                emit apsDataIndicationReceived(indication);
                break;
            }
            case Ti::AFCommandIncomingMsgExt: {
                quint8 srcAddrMode, srcEndpoint, dstEndpoint, wasBroadcast, lqi, securityUse, transactionSequenceNumber, macSrcAddr, radius;
                quint16 groupId, clusterId, srcPanId, dataLen;
                quint32 timestamp;
                quint64 srcAddr;

                // If the payload is missing (meaning the overall packet size is exactly 29), the payload is huge and is
                // not contained in thie packet but needs to be retrieved separately.
                bool hugePacket = payload.length() == 29;

                PAYLOAD_STREAM(payload);
                stream >> groupId;
                stream >> clusterId;
                stream >> srcAddrMode;
                stream >> srcAddr;
                stream >> srcEndpoint;
                stream >> srcPanId;
                stream >> dstEndpoint;
                stream >> wasBroadcast;
                stream >> lqi;
                stream >> securityUse;
                stream >> timestamp;
                stream >> transactionSequenceNumber;
                stream >> dataLen;

                QByteArray asdu;
                if (!hugePacket) {
                    for (int i = 0; i < dataLen; i++) {
                        quint8 byte;
                        stream >> byte;
                        asdu.append(byte);
                    }
                }

                stream >> macSrcAddr;
                stream >> radius;

                Zigbee::ApsdeDataIndication indication;
                indication.destinationAddressMode = Zigbee::DestinationAddressModeShortAddress;
                indication.sourceAddressMode = static_cast<Zigbee::DestinationAddressMode>(srcAddrMode);
                indication.sourceEndpoint = srcEndpoint;
                indication.sourceIeeeAddress = srcAddr;
                indication.destinationEndpoint = dstEndpoint;
                indication.clusterId = clusterId;
                indication.profileId = Zigbee::ZigbeeProfileHomeAutomation;
                indication.lqi = lqi;
                indication.asdu = asdu;
                if (!hugePacket) {
                    emit apsDataIndicationReceived(indication);
                } else {
                    retrieveHugeMessage(indication, timestamp, dataLen);
                }
                break;
            }
            case Ti::AFCommandDataConfirm: {
                quint8 status, endpoint, transactionSequenceNumber;
                PAYLOAD_STREAM(payload);
                stream >> status >> endpoint >> transactionSequenceNumber;
                Zigbee::ApsdeDataConfirm confirm;
                confirm.requestId = transactionSequenceNumber;
                confirm.destinationEndpoint = endpoint;
                confirm.zigbeeStatusCode = status;
                emit apsDataConfirmReceived(confirm);
                break;
            }
            default:
                qCWarning(dcZigbeeController()) << "Unhandled AF AREQ notification";
            }
            break;
        case Ti::SubSystemAppCnf:
            switch (command) {
            case Ti::AppCnfCommandBdbCommissioningNotification:{
                PAYLOAD_STREAM(payload);
                quint8 status, commissioningMode, remainingCommissioningModes;
                stream >> status >> commissioningMode >> remainingCommissioningModes;
                qCDebug(dcZigbeeController()) << "BDB commissioning notification received. Status:" << status << "Mode:" << commissioningMode << "Remaining:" << remainingCommissioningModes;
                break;
            }
            default:
                qCWarning(dcZigbeeController()) << "Unhandled AppCnf AREQ notification";
            }
            break;
        default:
            qCWarning(dcZigbeeController()) << "Unhandled AREQ notification";
        }

        foreach (ZigbeeInterfaceTiReply *reply, m_waitFors.keys()) {
            WaitData waitData = m_waitFors.value(reply);
            if (waitData.subSystem == subSystem && waitData.command == command) {
                if (!waitData.comparePayload || waitData.payload == payload) {
                    qCDebug(dcZigbeeController()) << "awaited event received.";
                    reply->m_responsePayload = payload;
                    reply->finish();
                }
            }
        }
    }

    else if (commandType == Ti::CommandTypeSReq) {
        qCWarning(dcZigbeeController()) << "Unhandled incoming SREQ command:" << subSystem << command;
    }
}

bool ZigbeeBridgeControllerTi::enable(const QString &serialPort, qint32 baudrate)
{
    return m_interface->enable(serialPort, baudrate);
}

void ZigbeeBridgeControllerTi::disable()
{
    m_interface->disable();
}

QDebug operator<<(QDebug debug, const TiNetworkConfiguration &configuration)
{
    debug.nospace() << "Network configuration: " << "\n";
    debug.nospace() << " - IEEE address: " << configuration.ieeeAddress.toString() << "\n";
    debug.nospace() << " - NWK address: " << ZigbeeUtils::convertUint16ToHexString(configuration.shortAddress) << "\n";
    debug.nospace() << " - PAN ID: " << ZigbeeUtils::convertUint16ToHexString(configuration.panId) << " (" << configuration.panId << ")\n";
    debug.nospace() << " - Extended PAN ID: " << ZigbeeUtils::convertUint64ToHexString(configuration.extendedPanId) << "\n";
    debug.nospace() << " - Channel mask: " << ZigbeeChannelMask(configuration.channelMask) << "\n";
    debug.nospace() << " - Channel: " << configuration.currentChannel << "\n";
    debug.nospace() << " - ZNP version: " << ZigbeeUtils::convertUint16ToHexString(configuration.znpVersion) << "\n";
    return debug.space();
}

