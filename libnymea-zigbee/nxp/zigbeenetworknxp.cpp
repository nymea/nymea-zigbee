/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
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

#include "zigbeenodeendpointnxp.h"
#include "zigbeenetworknxp.h"
#include "loggingcategory.h"
#include "zigbeenodenxp.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeNetworkNxp::ZigbeeNetworkNxp(QObject *parent) :
    ZigbeeNetwork(parent)
{
    m_controller = new ZigbeeBridgeControllerNxp(this);
    connect(m_controller, &ZigbeeBridgeControllerNxp::messageReceived, this, &ZigbeeNetworkNxp::onMessageReceived);
    connect(m_controller, &ZigbeeBridgeControllerNxp::availableChanged, this, &ZigbeeNetworkNxp::onControllerAvailableChanged);
}

ZigbeeBridgeController *ZigbeeNetworkNxp::bridgeController() const
{
    if (m_controller)
        return qobject_cast<ZigbeeBridgeController *>(m_controller);

    return nullptr;
}

void ZigbeeNetworkNxp::setStartingState(ZigbeeNetworkNxp::StartingState state)
{
    if (m_startingState == state)
        return;

    m_startingState = state;

    switch (m_startingState) {
    case StartingStateNone:
        break;
    case StartingStateErase: {
        m_networkRunning = false;
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Erase persistant data";
        ZigbeeInterfaceReply *reply = m_controller->commandErasePersistantData();
        connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply](){
            reply->deleteLater();

            if (reply->status() != ZigbeeInterfaceReply::Success) {
                qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
                // TODO: check error handling
                //return;
            }

            m_factoryResetting = false;
            qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
            setStartingState(StartingStateGetVersion);
        });
        break;
    }
    case StartingStateReset: {
        m_networkRunning = false;
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Reset controller";
        ZigbeeInterfaceReply *reply = m_controller->commandResetController();
        connect(reply, &ZigbeeInterfaceReply::finished, this, [reply](){
            reply->deleteLater();

            if (reply->status() != ZigbeeInterfaceReply::Success) {
                qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
                return;
            }

            qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
            // Note: the controller is now sending a log of cluster and attribte/command information
            // After the reset the state machine will continue
        });
        break;
    }
    case StartingStateGetVersion: {
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Get controller version";
        readControllerVersion();
        break;
    }
    case StartingStateSetPanId: {
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Set PAN ID";
        if (extendedPanId() == 0) {
            setExtendedPanId(ZigbeeUtils::generateRandomPanId());
        }
        ZigbeeInterfaceReply *reply = m_controller->commandSetExtendedPanId(extendedPanId());
        connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply](){
            reply->deleteLater();

            if (reply->status() != ZigbeeInterfaceReply::Success) {
                qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
                return;
            }

            qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
            setStartingState(StartingStateSetChannel);
        });
        break;
    }
    case StartingStateSetChannel: {
        // Create channel mask
        // Note: normal number passed, that specific channel will be used || Bitfield: all channels would be 0x07FFF800
        //       0x07fff800 select from all channels 11 - 26
        //       0x02108800 primary zigbee light link channels 11, 15, 20, 25

        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Set channel mask";
        ZigbeeInterfaceReply *reply = nullptr;
        if (channel() == 0) {
            qCDebug(dcZigbeeNetwork()) << "Autoselect quitest channel for the zigbee network. FIXME: currently hardcoded to 13 due to firmware error.";
            quint32 channelMask = 0;
            channelMask |= 1 << (14);
            reply = m_controller->commandSetChannelMask(channelMask);
        } else {
            quint32 channelMask = 0;
            channelMask |= 1 << (channel());
            qCDebug(dcZigbeeNetwork()) << "Using channel" << channel() << "for the zigbee network.";
            reply = m_controller->commandSetChannelMask(channelMask);
        }
        connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply](){
            reply->deleteLater();

            if (reply->status() != ZigbeeInterfaceReply::Success) {
                qCWarning(dcZigbeeNetwork()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
                return;
            }

            qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
            setStartingState(StartingStateSetSecurity);
        });
        break;
    }
    case StartingStateSetSecurity: {
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Set security configuration";

        qCDebug(dcZigbeeNetwork()) << "Set global trust center link key" << securityConfiguration().globalTrustCenterLinkKey();
        ZigbeeInterfaceReply *reply = m_controller->commandSetSecurityStateAndKey(4, 0, 1, securityConfiguration().globalTrustCenterLinkKey().toByteArray());
        connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply](){
            reply->deleteLater();

            if (reply->status() != ZigbeeInterfaceReply::Success) {
                qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            }

            qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";

            qCDebug(dcZigbeeNetwork()) << "Set network link key" << securityConfiguration().networkKey();
            ZigbeeInterfaceReply *reply2 = m_controller->commandSetSecurityStateAndKey(3, 0, 1, securityConfiguration().networkKey().toByteArray());
            connect(reply2, &ZigbeeInterfaceReply::finished, this, [this, reply2](){
                reply2->deleteLater();

                if (reply2->status() != ZigbeeInterfaceReply::Success) {
                    qCWarning(dcZigbeeController()) << "Could not" << reply2->request().description() << reply2->status() << reply2->statusErrorMessage();
                }

                qCDebug(dcZigbeeController()) << reply2->request().description() << "finished successfully";
                setStartingState(StartingStateSetNodeType);
            });
        });
        break;
    }
    case StartingStateSetNodeType: {
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Set node type";
        ZigbeeInterfaceReply *reply = m_controller->commandSetNodeType(ZigbeeNode::NodeTypeCoordinator);
        connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply](){
            reply->deleteLater();

            if (reply->status() != ZigbeeInterfaceReply::Success) {
                qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
                return;
            }

            qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
            setStartingState(StartingStateStartNetwork);
        });
        break;
    }
    case StartingStateStartNetwork: {
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Starting network";
        ZigbeeInterfaceReply *reply = m_controller->commandStartNetwork();
        connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply](){
            reply->deleteLater();

            if (reply->status() != ZigbeeInterfaceReply::Success) {
                qCWarning(dcZigbeeNetwork()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
                return;
            }

            qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
            qCDebug(dcZigbeeController()) << reply->additionalMessage();
            processNetworkFormed(reply->additionalMessage());
        });
        break;
    }
    }
}

void ZigbeeNetworkNxp::readControllerVersion()
{
    ZigbeeInterfaceReply *reply = m_controller->commandGetVersion();
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            return;
        }

        if (reply->additionalMessage().data().count() != 4) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << ":" << "Invalid payload size";
            return;
        }

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";

        // Parse version
        quint16 majorVersion = ZigbeeUtils::convertByteArrayToUint16(reply->additionalMessage().data().mid(0, 2));
        quint16 minorVersion = ZigbeeUtils::convertByteArrayToUint16(reply->additionalMessage().data().mid(2, 2));

        qCDebug(dcZigbeeNetwork()) << "Controller version:" << QString("%1.%2").arg(majorVersion).arg(minorVersion);
        m_controller->setFirmwareVersionString(QString("%1.%2").arg(majorVersion).arg(minorVersion));
        if (m_startingState == StartingStateGetVersion)
            setStartingState(StartingStateSetPanId);

    });
}

void ZigbeeNetworkNxp::readPermitJoinStatus()
{
    ZigbeeInterfaceReply *reply = m_controller->commandGetPermitJoinStatus();
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            return;
        }

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        qCDebug(dcZigbeeController()) << reply->additionalMessage();

        setPermitJoining(static_cast<bool>(reply->additionalMessage().data().at(0)));
    });
}

ZigbeeNode *ZigbeeNetworkNxp::createNode(QObject *parent)
{
    return new ZigbeeNodeNxp(m_controller, parent);
}

void ZigbeeNetworkNxp::setPermitJoiningInternal(bool permitJoining)
{

//    ZigbeeInterfaceReply *replyGroup = m_controller->commandAddGroup(0x00, 0x0000, 0x01, 0x01, 0x0001);
//    connect(replyGroup, &ZigbeeInterfaceReply::finished, replyGroup, &ZigbeeInterfaceReply::deleteLater);

    // Note: 0xfffc = all routers
    qCDebug(dcZigbeeNetwork()) << "Send request to" << (permitJoining ? "enable" : "disable") << "permit joining network.";
    ZigbeeInterfaceReply *reply = m_controller->commandPermitJoin(0xfffc, (permitJoining ? 255 : 0));
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, permitJoining](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            return;
        }

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        qCDebug(dcZigbeeController()) << reply->additionalMessage();

        if (m_permitJoining == permitJoining)
            return;

        qCDebug(dcZigbeeNetwork()) << "Permit joining changed to" << permitJoining;
        m_permitJoining = permitJoining;
        emit permitJoiningChanged(m_permitJoining);
    });
}

void ZigbeeNetworkNxp::onCommandInitiateTouchLinkFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkNxp::onCommandTouchLinkFactoryResetFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkNxp::onCommandRequestLinkQualityFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    qCDebug(dcZigbeeController()) << reply->additionalMessage();

    // FIXME: parsing

    quint8 sequenceNumber = static_cast<quint8>(reply->additionalMessage().data().at(0));
    quint8 statusCode = static_cast<quint8>(reply->additionalMessage().data().at(1));
    quint8 neighborTableEntries = static_cast<quint8>(reply->additionalMessage().data().at(2));
    quint8 neighborTableListCount = static_cast<quint8>(reply->additionalMessage().data().at(3));
    quint8 startIndex = static_cast<quint8>(reply->additionalMessage().data().at(4));

    qCDebug(dcZigbeeNetwork()) << "LQI response:";
    qCDebug(dcZigbeeNetwork()) << "    SQN:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbeeNetwork()) << "    Status:" << ZigbeeUtils::convertByteToHexString(statusCode);
    qCDebug(dcZigbeeNetwork()) << "    Neighbor table entries:" << neighborTableEntries;
    qCDebug(dcZigbeeNetwork()) << "    Neighbor table list count:" << neighborTableListCount;
    qCDebug(dcZigbeeNetwork()) << "    Start index:" << startIndex;

    int offset = 5;

    // Note: according to docs, if the table has no neigbors the list will be empty
    if (neighborTableEntries == 0) {
        qCDebug(dcZigbeeNetwork()) << "    There are no neigbors";
        return;
    }

    for (int i = startIndex; i < neighborTableListCount; i++) {

        quint16 shortAddress = ZigbeeUtils::convertByteArrayToUint16(reply->additionalMessage().data().mid(offset, 2));
        quint64 panId = ZigbeeUtils::convertByteArrayToUint64(reply->additionalMessage().data().mid(offset + 2, 8));
        quint64 ieeeAddress = ZigbeeUtils::convertByteArrayToUint64(reply->additionalMessage().data().mid(offset + 10, 8));
        quint8 depth = static_cast<quint8>(reply->additionalMessage().data().at(offset + 18));
        quint8 linkQuality = static_cast<quint8>(reply->additionalMessage().data().at(offset + 19));
        quint8 bitMap = static_cast<quint8>(reply->additionalMessage().data().at(offset + 20));

        offset += 21;

        qCDebug(dcZigbeeNetwork()) << "    Neighbor:" << i;
        qCDebug(dcZigbeeNetwork()) << "        Address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
        qCDebug(dcZigbeeNetwork()) << "        PAN id:" << panId;
        qCDebug(dcZigbeeNetwork()) << "        Extended address:" << ZigbeeAddress(ieeeAddress);
        qCDebug(dcZigbeeNetwork()) << "        Depth:" << depth;
        qCDebug(dcZigbeeNetwork()) << "        Link quality:" << linkQuality;
        qCDebug(dcZigbeeNetwork()) << "        BitMap:" << ZigbeeUtils::convertByteToHexString(bitMap);
    }
}

void ZigbeeNetworkNxp::processLoggingMessage(const ZigbeeInterfaceMessage &message)
{
    quint8 logLevel = static_cast<quint8>(message.data().at(0));
    QString logMessage = QString::fromUtf8(message.data().right(message.data().count() - 1));

    QString logLevelString;
    switch (logLevel) {
    case 0:
        logLevelString = "Emergency:";
        break;
    case 1:
        logLevelString = "Alert:";
        break;
    case 2:
        logLevelString = "Critical:";
        break;
    case 3:
        logLevelString = "Error:";
        break;
    case 4:
        logLevelString = "Warning:";
        break;
    case 5:
        logLevelString = "Notice:";
        break;
    case 6:
        logLevelString = "Information:";
        break;
    case 7:
        logLevelString = "Debug:";
        break;
    default:
        logLevelString = "Unknown:";
        break;
    }

    if (logLevel < 5) {
        qCWarning(dcZigbeeController()).noquote() << "ControllerLog:" << logLevelString << logMessage;
    } else {
        qCDebug(dcZigbeeController()).noquote() << "ControllerLog:" << logLevelString << logMessage;
    }
}

void ZigbeeNetworkNxp::processFactoryNewRestart(const ZigbeeInterfaceMessage &message)
{
    quint8 controllerStatus = static_cast<quint8>(message.data().at(0));
    QString controllerStatusString;
    switch (controllerStatus) {
    case 0:
        controllerStatusString = "startup";
        break;
    case 1:
        controllerStatusString = "wait start";
        break;
    case 2:
        controllerStatusString = "NRF start";
        break;
    case 3:
        controllerStatusString = "discovery";
        break;
    case 4:
        controllerStatusString = "network init";
        break;
    case 5:
        controllerStatusString = "rescan";
        break;
    case 6:
        controllerStatusString = "running";
        break;
    default:
        qCWarning(dcZigbeeNetwork()) << "Unhandled controller status" << controllerStatus;
        break;
    }

    qCDebug(dcZigbeeNetwork()) << "Restart finished. Current controller state:" << controllerStatusString;

    if (m_startingState == StartingStateReset)
        setStartingState(StartingStateGetVersion);
}

void ZigbeeNetworkNxp::processNodeClusterList(const ZigbeeInterfaceMessage &message)
{
    quint8 sourceEndpoint = static_cast<quint8>(message.data().at(0));

    quint16 profileId = static_cast<quint8>(message.data().at(1));
    profileId <<= 8;
    profileId |= static_cast<quint8>(message.data().at(2));

    // FIXME:
    return;
    qCDebug(dcZigbeeController()) << "Node cluster list received:";
    qCDebug(dcZigbeeController()) << "    Souce endpoint:" << sourceEndpoint;
    qCDebug(dcZigbeeController()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));


    QByteArray clusterListData = message.data().right(message.data().count() - 3);

    for (int i = 0; i < clusterListData.count(); i += 2) {
        quint16 clusterId = static_cast<quint16>(clusterListData.at(i));
        clusterId <<= 8;
        clusterId |= clusterListData .at(i + 1);

        qCDebug(dcZigbeeController()) << "        Cluster ID:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
    }
}

void ZigbeeNetworkNxp::processNodeAttributeList(const ZigbeeInterfaceMessage &message)
{
    quint8 sourceEndpoint = static_cast<quint8>(message.data().at(0));


    quint16 profileId = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(1, 2));
    quint16 clusterId = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(3, 2));

    // FIXME:
    return;
    qCDebug(dcZigbeeController()) << "Node attribute list received:";

    qCDebug(dcZigbeeController()) << "    Souce endpoint:" << sourceEndpoint;
    qCDebug(dcZigbeeController()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));
    qCDebug(dcZigbeeController()) << "    Cluster ID:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));

    QByteArray attributeListData = message.data().right(message.data().count() - 5);
    if (attributeListData.count() % 2 != 0) {
        qCWarning(dcZigbeeController()) << "Attribute list is incomplete. Truncate last byte";
        attributeListData = attributeListData.left(attributeListData.count() - 1);
    }

    qCWarning(dcZigbeeController()) << "Attributes" << ZigbeeUtils::convertByteArrayToHexString(attributeListData);
    for (int i = 0; i < attributeListData.count(); i += 2) {
        quint16 attribute = ZigbeeUtils::convertByteArrayToUint16(attributeListData.mid(i, 2));
        qCDebug(dcZigbeeController()) << "        Attribute:" << ZigbeeUtils::convertUint16ToHexString(attribute);
    }
}

void ZigbeeNetworkNxp::processNodeCommandIdList(const ZigbeeInterfaceMessage &message)
{
    quint8 sourceEndpoint = static_cast<quint8>(message.data().at(0));
    quint16 profileId = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(1, 2));
    quint16 clusterId = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(3, 2));

    // FIXME:
    return;
    qCDebug(dcZigbeeController()) << "Node command list received:";
    qCDebug(dcZigbeeController()) << "    Souce endpoint:" << sourceEndpoint;
    qCDebug(dcZigbeeController()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));
    qCDebug(dcZigbeeController()) << "    Cluster ID:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));

    QByteArray commandListData = message.data().right(message.data().count() - 5);

    for (int i = 0; i < commandListData.count(); i++) {
        quint8 attribute = static_cast<quint8>(commandListData.at(i));
        qCDebug(dcZigbeeController()) << "        Command:" << ZigbeeUtils::convertByteToHexString(attribute);
    }
}

void ZigbeeNetworkNxp::processDeviceAnnounce(const ZigbeeInterfaceMessage &message)
{
    QByteArray data = message.data();

    quint16 shortAddress = 0;
    quint64 ieeeAddress = 0;
    quint8 macCapabilitiesFlag = 0;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> shortAddress >> ieeeAddress >> macCapabilitiesFlag;

    ZigbeeAddress nodeIeeeAddress(ieeeAddress);

    qCDebug(dcZigbeeNetwork()) << "Device announced:";
    qCDebug(dcZigbeeNetwork()) << "    Address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbeeNetwork()) << "    Extended address:" << nodeIeeeAddress;
    qCDebug(dcZigbeeNetwork()) << "    Mac capabilities:" << ZigbeeUtils::convertByteToHexString(macCapabilitiesFlag);

    // Check if we already have a node with this
    if (hasNode(nodeIeeeAddress)) {
        qCDebug(dcZigbeeNetwork()) << "Node already registered in the network.";
        ZigbeeNode *node = getZigbeeNode(nodeIeeeAddress);
        if (node->shortAddress() != shortAddress) {
            qCWarning(dcZigbeeNetwork()) << "The node changed the NWK address from" << ZigbeeUtils::convertUint16ToHexString(node->shortAddress()) << "to" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
            // FIMXE: check if we should reinitialize the node
        }
        return;
    }


    ZigbeeNodeNxp *node = new ZigbeeNodeNxp(m_controller, this);
    node->setShortAddress(shortAddress);
    node->setExtendedAddress(ZigbeeAddress(ieeeAddress));
    node->setMacCapabilitiesFlag(macCapabilitiesFlag);

    node->startInitialization();
    addUnitializedNode(node);

//    qCDebug(dcZigbeeNetwork()) << "Start authenticating" << node;
//    ZigbeeInterfaceReply *reply = m_controller->commandAuthenticateDevice(node->extendedAddress(), securityConfiguration().networkKey().toByteArray());
//    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, node](){
//        reply->deleteLater();

//        if (reply->status() != ZigbeeInterfaceReply::Success) {
//            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
//            return;
//        }

//        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
//        qCDebug(dcZigbeeController()) << reply->additionalMessage();

//        quint64 gatewayIeeeAddress = ZigbeeUtils::convertByteArrayToUint64(reply->additionalMessage().data().mid(0, 8));
//        QString encryptedKey = reply->additionalMessage().data().mid(8, 16).toHex();
//        QByteArray mic = reply->additionalMessage().data().mid(24, 4);
//        quint64 initiatorIeeeAddress = ZigbeeUtils::convertByteArrayToUint64(reply->additionalMessage().data().mid(28, 8));
//        quint8 activeKeySequenceNumber = static_cast<quint8>(reply->additionalMessage().data().at(36));
//        quint8 channel = static_cast<quint8>(reply->additionalMessage().data().at(37));
//        quint16 shortPan = ZigbeeUtils::convertByteArrayToUint16(reply->additionalMessage().data().mid(38, 2));
//        quint64 extendedPanId = ZigbeeUtils::convertByteArrayToUint64(reply->additionalMessage().data().mid(40, 8));

//        qCDebug(dcZigbeeNetwork()) << "Authentication response for" << node;
//        qCDebug(dcZigbeeNetwork()) << "    Gateways address:" << ZigbeeAddress(gatewayIeeeAddress);
//        qCDebug(dcZigbeeNetwork()) << "    Key:" << ZigbeeNetworkKey(encryptedKey).toString();
//        qCDebug(dcZigbeeNetwork()) << "    MIC:" << mic.toHex();
//        qCDebug(dcZigbeeNetwork()) << "    Initiator address:" << ZigbeeAddress(initiatorIeeeAddress);
//        qCDebug(dcZigbeeNetwork()) << "    Active key sequence number:" << activeKeySequenceNumber;
//        qCDebug(dcZigbeeNetwork()) << "    Channel:" << channel;
//        qCDebug(dcZigbeeNetwork()) << "    Short PAN ID:" << ZigbeeUtils::convertUint16ToHexString(shortPan);
//        qCDebug(dcZigbeeNetwork()) << "    Extended PAN ID:" << extendedPanId << ZigbeeUtils::convertUint64ToHexString(extendedPanId);

//        node->startInitialization();
//        addUnitializedNode(node);
//    });


}

void ZigbeeNetworkNxp::processAttributeReport(const ZigbeeInterfaceMessage &message)
{
    QByteArray data = message.data();
    quint8 sequenceNumber = 0;
    quint16 sourceAddress = 0;
    quint8 endpointId = 0;
    quint16 clusterId = 0;
    quint16 attributeId = 0;
    quint8 attributeStatus = 0;
    quint8 attributDataType = 0;
    quint16 dataSize = 0;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> sequenceNumber >> sourceAddress >> endpointId >> clusterId >> attributeId >> attributeStatus >> attributDataType >> dataSize;

    Zigbee::DataType dataType = static_cast<Zigbee::DataType>(attributDataType);
    QByteArray attributeData = data.right(dataSize);

    if (attributeData.length() != dataSize) {
        qCWarning(dcZigbeeNetwork()) << "HACK" << attributeData.length() << "!=" << dataSize;
        // Note: the NXP firmware for JN5169 has a bug here and does not send the attributeStatus.
        // Repars data without attribute status
        sequenceNumber = 0;
        sourceAddress = 0;
        endpointId = 0;
        clusterId = 0;
        attributeId = 0;
        attributeStatus = 0;
        attributDataType = 0;
        dataSize = 0;

        QDataStream alternativeStream(&data, QIODevice::ReadOnly);
        alternativeStream >> sequenceNumber >> sourceAddress >> endpointId >> clusterId >> attributeId >> attributDataType >> dataSize;

        dataType = static_cast<Zigbee::DataType>(attributDataType);
        attributeData = data.right(dataSize);
    }

    qCDebug(dcZigbeeNetwork()) << "Attribute report:";
    qCDebug(dcZigbeeNetwork()) << "    SQN:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbeeNetwork()) << "    Source address:" << ZigbeeUtils::convertUint16ToHexString(sourceAddress);
    qCDebug(dcZigbeeNetwork()) << "    End point:" << ZigbeeUtils::convertByteToHexString(endpointId);
    qCDebug(dcZigbeeNetwork()) << "    Cluster:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
    qCDebug(dcZigbeeNetwork()) << "    Attribut id:" << ZigbeeUtils::convertUint16ToHexString(attributeId);
    qCDebug(dcZigbeeNetwork()) << "    Attribut status:" <<  static_cast<Zigbee::ZigbeeStatus>(attributeStatus);
    qCDebug(dcZigbeeNetwork()) << "    Attribut data type:" << dataType;
    qCDebug(dcZigbeeNetwork()) << "    Attribut size:" << dataSize;
    qCDebug(dcZigbeeNetwork()) << "    Data:" << ZigbeeUtils::convertByteArrayToHexString(attributeData);

    switch (dataType) {
    case Zigbee::CharString:
        qCDebug(dcZigbeeNetwork()) << "    Data(converted)" << QString::fromUtf8(attributeData);
        break;
    case Zigbee::Bool:
        qCDebug(dcZigbeeNetwork()) << "    Data(converted)" << static_cast<bool>(attributeData.at(0));
        break;
    default:
        break;
    }

    ZigbeeNodeNxp *node = qobject_cast<ZigbeeNodeNxp *>(getZigbeeNode(sourceAddress));
    if (!node) {
        qCWarning(dcZigbeeNode()) << "Received an attribute report from an unknown node. Ignoring data.";
        return;
    }

    ZigbeeClusterAttributeReport attributeReport;
    attributeReport.sourceAddress = sourceAddress;
    attributeReport.endpointId = endpointId;
    attributeReport.clusterId = static_cast<Zigbee::ClusterId>(clusterId);
    attributeReport.attributeId = attributeId;
    attributeReport.attributeStatus = static_cast<Zigbee::ZigbeeStatus>(attributeStatus);
    attributeReport.dataType = dataType;
    attributeReport.data = attributeData;

    node->setClusterAttributeReport(attributeReport);
}

void ZigbeeNetworkNxp::processReadAttributeResponse(const ZigbeeInterfaceMessage &message)
{
    QByteArray data = message.data();

    quint8 sequenceNumber = 0;
    quint16 sourceAddress = 0;
    quint8 endpointId = 0;
    quint16 clusterId = 0;
    quint16 attributeId = 0;
    quint8 attributeStatus = 0;
    quint8 attributDataType = 0;
    quint16 dataSize = 0;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> sequenceNumber >> sourceAddress >> endpointId >> clusterId >> attributeId >> attributeStatus >> attributDataType >> dataSize;

    Zigbee::DataType dataType = static_cast<Zigbee::DataType>(attributDataType);
    QByteArray attributeData = data.right(dataSize);

    if (attributeData.length() != dataSize) {
        qCWarning(dcZigbeeNetwork()) << "HACK" << attributeData.length() << "!=" << dataSize;
        // Note: the NXP firmware for JN5169 has a bug here and does not send the attributeStatus.
        // Repars data without attribute status
        sequenceNumber = 0;
        sourceAddress = 0;
        endpointId = 0;
        clusterId = 0;
        attributeId = 0;
        attributeStatus = 0;
        attributDataType = 0;
        dataSize = 0;

        QDataStream alternativeStream(&data, QIODevice::ReadOnly);
        alternativeStream >> sequenceNumber >> sourceAddress >> endpointId >> clusterId >> attributeId >> attributDataType >> dataSize;

        dataType = static_cast<Zigbee::DataType>(attributDataType);
        attributeData = data.right(dataSize);
    }

    qCDebug(dcZigbeeNetwork()) << "Attribute read response:";
    qCDebug(dcZigbeeNetwork()) << "    SQN:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbeeNetwork()) << "    Source address:" << ZigbeeUtils::convertUint16ToHexString(sourceAddress);
    qCDebug(dcZigbeeNetwork()) << "    End point:" << ZigbeeUtils::convertByteToHexString(endpointId);
    qCDebug(dcZigbeeNetwork()) << "    Cluster:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
    qCDebug(dcZigbeeNetwork()) << "    Attribut id:" << ZigbeeUtils::convertUint16ToHexString(attributeId);
    qCDebug(dcZigbeeNetwork()) << "    Attribut status:" <<  static_cast<Zigbee::ZigbeeStatus>(attributeStatus);
    qCDebug(dcZigbeeNetwork()) << "    Attribut data type:" << dataType;
    qCDebug(dcZigbeeNetwork()) << "    Attribut size:" << dataSize;
    qCDebug(dcZigbeeNetwork()) << "    Data:" << ZigbeeUtils::convertByteArrayToHexString(attributeData);

    switch (dataType) {
    case Zigbee::CharString:
        qCDebug(dcZigbeeNetwork()) << "    Data(converted)" << QString::fromUtf8(attributeData);
        break;
    case Zigbee::Bool:
        qCDebug(dcZigbeeNetwork()) << "    Data(converted)" << static_cast<bool>(attributeData.at(0));
        break;
    default:
        break;
    }


    ZigbeeNodeNxp *node = qobject_cast<ZigbeeNodeNxp *>(getZigbeeNode(sourceAddress));
    if (!node) {
        qCWarning(dcZigbeeNode()) << "Received an attribute report from an unknown node. Ignoring data.";
        return;
    }

    ZigbeeClusterAttributeReport attributeReport;
    attributeReport.sourceAddress = sourceAddress;
    attributeReport.endpointId = endpointId;
    attributeReport.clusterId = static_cast<Zigbee::ClusterId>(clusterId);
    attributeReport.attributeId = attributeId;
    attributeReport.attributeStatus = static_cast<Zigbee::ZigbeeStatus>(attributeStatus);
    attributeReport.dataType = dataType;
    attributeReport.data = attributeData;

    node->setClusterAttributeReport(attributeReport);
}

void ZigbeeNetworkNxp::processLeaveIndication(const ZigbeeInterfaceMessage &message)
{
    QByteArray data = message.data();
    quint64 extendedAddress = 0;
    bool rejoining = 0;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> extendedAddress;
    stream >> rejoining;

    ZigbeeAddress address(extendedAddress);
    qCDebug(dcZigbeeNetwork()) << "Node leaving indication:" << address.toString() << "rejoining:" << rejoining;

    ZigbeeNode *node = getZigbeeNode(address);
    if (node) removeNode(node);
}

void ZigbeeNetworkNxp::processRestartProvisioned(const ZigbeeInterfaceMessage &message)
{
    if (message.data().isEmpty())
        return;

    quint8 status = static_cast<quint8>(message.data().at(0));
    switch (status) {
    case 0:
        qCDebug(dcZigbeeNetwork()) << "Restart provisioned: start up";
        break;
    case 1:
        qCDebug(dcZigbeeNetwork()) << "Restart provisioned: wait start";
        break;
    case 2:
        qCDebug(dcZigbeeNetwork()) << "Restart provisioned: NFN start";
        break;
    case 3:
        qCDebug(dcZigbeeNetwork()) << "Restart provisioned: discovery";
        break;
    case 4:
        qCDebug(dcZigbeeNetwork()) << "Restart provisioned: network init";
        break;
    case 5:
        qCDebug(dcZigbeeNetwork()) << "Restart provisioned: rescan";
        break;
    case 6:
        qCDebug(dcZigbeeNetwork()) << "Restart provisioned: running";
        break;
    default:
        qCDebug(dcZigbeeNetwork()) << "Restart provisioned: unknown";
        break;
    }

    if (m_startingState == StartingStateReset) {
        if (m_networkRunning) {
            qCDebug(dcZigbeeNetwork()) << "Reset finished. Network already running. No need to set it up";
            readControllerVersion();

            // FIXME: get network status
            //setStartingState(StartingStateGetPermitJoinStatus);
        } else {
            qCDebug(dcZigbeeNetwork()) << "Reset finished. Set up network";
            setStartingState(StartingStateGetVersion);
        }
    }
}

void ZigbeeNetworkNxp::processRouterDiscoveryConfirm(const ZigbeeInterfaceMessage &message)
{
    if (message.data().isEmpty())
        return;

    quint8 status = static_cast<quint8>(message.data().at(0));
    Zigbee::ZigbeeNwkLayerStatus networkStatus = static_cast<Zigbee::ZigbeeNwkLayerStatus>(message.data().at(0));
    qCDebug(dcZigbeeNetwork()) << "Router discovery confirm received" << status << networkStatus;
}

void ZigbeeNetworkNxp::processDefaultResponse(const ZigbeeInterfaceMessage &message)
{
    // Parse network status
    QByteArray data = message.data();
    quint8 sequenceNumber = 0;
    quint16 shortAddress = 0;
    quint8 sourceEndpoint = 0;
    quint8 destinationEndpoint = 0;
    quint16 clusterId = 0;
    quint8 commandId = 0;
    quint8 commandStatus = 0;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> sequenceNumber >> shortAddress >> sourceEndpoint >> destinationEndpoint >> clusterId >> commandId >> commandStatus;

    qCDebug(dcZigbeeNetwork()).noquote() << "Default response" << sequenceNumber;
    qCDebug(dcZigbeeNetwork()) << "    Short address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbeeNetwork()) << "    Source endpoint:" << ZigbeeUtils::convertByteToHexString(sourceEndpoint);
    qCDebug(dcZigbeeNetwork()) << "    Destination endpoint:" << ZigbeeUtils::convertByteToHexString(sourceEndpoint);
    qCDebug(dcZigbeeNetwork()) << "    Destination endpoint:" << static_cast<Zigbee::ClusterId>(clusterId);
    qCDebug(dcZigbeeNetwork()) << "    Command:" << ZigbeeUtils::convertByteToHexString(commandId);
    qCDebug(dcZigbeeNetwork()) << "    Command status:" << static_cast<Zigbee::ZigbeeStatus>(commandStatus);

}

void ZigbeeNetworkNxp::onMessageReceived(const ZigbeeInterfaceMessage &message)
{
    switch (message.messageType()) {
    case Zigbee::MessageTypeLogging:
        processLoggingMessage(message);
        break;
    case Zigbee::MessageTypeFactoryNewRestart:
        processFactoryNewRestart(message);
        break;
    case Zigbee::MessageTypeNodeClusterList:
        processNodeClusterList(message);
        break;
    case Zigbee::MessageTypeNodeAttributeList:
        processNodeAttributeList(message);
        break;
    case Zigbee::MessageTypeNodeCommandIdList:
        processNodeCommandIdList(message);
        break;
    case Zigbee::MessageTypeDeviceAnnounce:
        processDeviceAnnounce(message);
        break;
    case Zigbee::MessageTypeAttributeReport:
        processAttributeReport(message);
        break;
    case Zigbee::MessageTypeReadAttributeResponse:
        processReadAttributeResponse(message);
        break;
    case Zigbee::MessageTypeLeaveIndication:
        processLeaveIndication(message);
        break;
    case Zigbee::MessageTypeNetworkJoinedFormed:
        processNetworkFormed(message);
        break;
    case Zigbee::MessageTypeRestartProvisioned:
        processRestartProvisioned(message);
        break;
    case Zigbee::MessageTypeRouterDiscoveryConfirm:
        processRouterDiscoveryConfirm(message);
        break;
    case Zigbee::MessageTypeDefaultResponse:
        processDefaultResponse(message);
        break;
    default:
        qCWarning(dcZigbeeController()) << "Unhandled message received:" << message;
        break;
    }
}

void ZigbeeNetworkNxp::onControllerAvailableChanged(bool available)
{
    qCDebug(dcZigbeeNetwork()) << "Hardware controller is" << (available ? "now available" : "not available");

    if (!available) {
        foreach (ZigbeeNode *node, nodes()) {
            qobject_cast<ZigbeeNodeNxp *>(node)->setConnected(false);
        }

        setError(ErrorHardwareUnavailable);
        m_permitJoining = false;
        emit permitJoiningChanged(m_permitJoining);
        setStartingState(StartingStateNone);
        setState(StateOffline);
    } else {
        m_error = ErrorNoError;
        m_permitJoining = false;
        emit permitJoiningChanged(m_permitJoining);
        // Note: if we are factory resetting, erase also the data on the controller before resetting
        if (m_factoryResetting) {
            setStartingState(StartingStateErase);
        } else {
            setStartingState(StartingStateReset);
        }

        setState(StateStarting);
    }
}


//void ZigbeeNetworkNxp::requestMatchDescriptor(const quint16 &shortAddress, const Zigbee::ZigbeeProfile &profile)
//{

//    // TargetAddress profile InputClusterCount InputClusterList OutputClusterCount OutputClusterList

//    Q_UNUSED(profile)

//    QByteArray data;
//    QDataStream stream(&data, QIODevice::WriteOnly);
//    stream << shortAddress;
//    stream << static_cast<quint16>(0xFFFF);
//    stream << static_cast<quint8>(0);
//    stream << static_cast<quint16>(0);
//    stream << static_cast<quint8>(0);
//    stream << static_cast<quint16>(0);

//    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeMatchDescriptorRequest, data));
//    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeMatchDescriptorResponse);
//    request.setDescription("Request match descriptors " + ZigbeeUtils::convertUint16ToHexString(shortAddress));
//    request.setTimoutIntervall(5000);

//    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
//    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkNxp::onRequestMatchDescriptorFinished);
//}

void ZigbeeNetworkNxp::onCommandResetControllerFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkNxp::onCommandSoftResetControllerFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkNxp::onCommandStartScanFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    qCDebug(dcZigbeeController()) << reply->additionalMessage();
    processNetworkFormed(reply->additionalMessage());
}

void ZigbeeNetworkNxp::onCommandRequestMatchDescriptorFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    qCDebug(dcZigbeeController()) << reply->additionalMessage();
}

void ZigbeeNetworkNxp::processNetworkFormed(const ZigbeeInterfaceMessage &message)
{
    // Parse network status
    QByteArray data = message.data();

    quint8 networkStatus = 0;
    quint16 shortAddress = 0;
    quint64 extendedAddress = 0;
    quint8 currentChannel = 0;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> networkStatus >> shortAddress >> extendedAddress >> currentChannel;

    QString networkStatusString;
    if (networkStatus == 0) {
        networkStatusString = "joined";
    } else if (networkStatus == 1) {
        networkStatusString = "formed";
    } else if (networkStatus >= 128 && networkStatus <= 244) {
        networkStatusString = "failed: Zigbee event code: " + QString::number(networkStatus);
    } else {
        networkStatusString = "unknown";
    }

    if (networkStatus != Zigbee::ZigbeeNwkLayerStatusJointNetwork && networkStatus != Zigbee::ZigbeeNwkLayerStatusFormedNetwork) {
        qCWarning(dcZigbeeNetwork()) << "Forming network failed" << networkStatusString;
        m_permitJoining = false;
        emit permitJoiningChanged(m_permitJoining);
        setStartingState(StartingStateNone);
        setState(StateOffline);
        setError(ErrorZigbeeError);
        m_networkRunning = false;
        return;
    }

    setChannel(currentChannel);

    // Parse network channel
    qCDebug(dcZigbeeNetwork()).noquote() << "Network" << networkStatusString;
    qCDebug(dcZigbeeNetwork()) << "    Extended PAN ID:" << extendedPanId();
    qCDebug(dcZigbeeNetwork()) << "    Address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbeeNetwork()) << "    Extended address:" << ZigbeeAddress(extendedAddress);
    qCDebug(dcZigbeeNetwork()) << "    Channel:" << channel();
    qCDebug(dcZigbeeNetwork()) << "    Permit joining:" << permitJoining();

    saveNetwork();
    m_networkRunning = true;

    if (nodes().isEmpty()) {
        // Create coordinator node
        ZigbeeNodeNxp *node = new ZigbeeNodeNxp(m_controller, m_controller);
        node->setShortAddress(shortAddress);
        node->setExtendedAddress(ZigbeeAddress(extendedAddress));
        node->startInitialization();

        connect(node, &ZigbeeNode::stateChanged, this, [this, node](){
            if (node->state() == ZigbeeNode::StateInitialized) {
                m_coordinatorNode = qobject_cast<ZigbeeNode *>(node);
                addNode(m_coordinatorNode);
                qCDebug(dcZigbeeNetwork()) << "Coordinator node initialized. The network is now set up.";
                setState(StateRunning);

//                ZigbeeInterfaceReply *reply = m_controller->commandBindLocalGroup(Zigbee::ClusterIdOnOff, 0x01, 0x0001);
//                connect(reply, &ZigbeeInterfaceReply::finished, this, [reply](){
//                    reply->deleteLater();

//                    if (reply->status() != ZigbeeInterfaceReply::Success) {
//                        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
//                        return;
//                    }

//                    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
//                });
            }
        });
    } else {
        // Primary initialization was already done.
        ZigbeeInterfaceReply *reply = m_controller->commandGetVersion();
        connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply](){
            reply->deleteLater();

            if (reply->status() != ZigbeeInterfaceReply::Success) {
                qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
                return;
            }

            if (reply->additionalMessage().data().count() != 4) {
                qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << ":" << "Invalid payload size";
                return;
            }

            qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";

            // Parse version
            quint16 majorVersion = ZigbeeUtils::convertByteArrayToUint16(reply->additionalMessage().data().mid(0, 2));
            quint16 minorVersion = ZigbeeUtils::convertByteArrayToUint16(reply->additionalMessage().data().mid(2, 2));

            qCDebug(dcZigbeeNetwork()) << "Controller version:" << QString("%1.%2").arg(majorVersion).arg(minorVersion);
            m_controller->setFirmwareVersionString(QString("%1.%2").arg(majorVersion).arg(minorVersion));

            setState(StateRunning);
        });
    }
}

void ZigbeeNetworkNxp::startNetwork()
{
    // FIXME: define if router or coordinator
    qCDebug(dcZigbeeNetwork()) << "Start network...";

    if (state() == StateUninitialized)
        loadNetwork();

    // Do a factory reset if there are no network configuration and create a new one from scratch
    m_factoryResetting = !networkConfigurationAvailable();

    setState(StateOffline);
    // Check if we have to create a pan ID and select the channel
    if (extendedPanId() == 0) {
        setExtendedPanId(ZigbeeUtils::generateRandomPanId());
        qCDebug(dcZigbeeNetwork()) << "Created new PAN ID:" << extendedPanId();
    }

    if (securityConfiguration().networkKey().isNull()) {
        qCDebug(dcZigbeeNetwork()) << "Create a new network key";
        ZigbeeNetworkKey key = ZigbeeNetworkKey::generateKey();
        m_securityConfiguration.setNetworkKey(key);
    }

    qCDebug(dcZigbeeNetwork()) << "Using network link key" << securityConfiguration().networkKey();
    qCDebug(dcZigbeeNetwork()) << "Using global trust center link key" << securityConfiguration().globalTrustCenterLinkKey();

    // TODO: get desired channel, by default use all

    if (!m_controller->enable(serialPortName(), serialBaudrate())) {
        m_permitJoining = false;
        emit permitJoiningChanged(m_permitJoining);
        setState(StateOffline);
        setStartingState(StartingStateNone);
        setError(ErrorHardwareUnavailable);
        return;
    }

    m_permitJoining = false;
    emit permitJoiningChanged(m_permitJoining);
    // Note: wait for the controller available signal and start the initialization there
}

void ZigbeeNetworkNxp::stopNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Stopping network...";
    setState(StateStopping);
    if (m_controller) {
        delete m_controller;
        m_controller = nullptr;
    }

    setStartingState(StartingStateNone);
    m_permitJoining = false;
    emit permitJoiningChanged(m_permitJoining);
    setState(StateOffline);
    setError(ErrorNoError);
}

void ZigbeeNetworkNxp::reset()
{
    qCDebug(dcZigbeeNetwork()) << "Reset the bridge controller.";
    setState(StateOffline);

    ZigbeeInterfaceReply *reply = m_controller->commandResetController();
    connect(reply, &ZigbeeInterfaceReply::finished, this, [reply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            return;
        }

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        // Note: the controller is now sending a log of cluster and attribte/command information
        // After the reset the state machine will continue
    });
}

void ZigbeeNetworkNxp::factoryResetNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Factory reset network and forget all information. This cannot be undone.";
    m_factoryResetting = true;
    clearSettings();
    setState(StateUninitialized);
    qCDebug(dcZigbeeNetwork()) << "The factory reset is finished. Start restart with a fresh network.";
    startNetwork();
}

