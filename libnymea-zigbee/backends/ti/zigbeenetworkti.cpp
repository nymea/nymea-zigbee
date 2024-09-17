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

#include "zdo/zigbeedeviceprofile.h"
#include "zigbeenetworkti.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"
#include "zigbeenetworkdatabase.h"

#include <QDataStream>
#ifndef DISABLE_QCA
#include <QtCrypto>
#endif

ZigbeeNetworkTi::ZigbeeNetworkTi(const QUuid &networkUuid, QObject *parent) :
    ZigbeeNetwork(networkUuid, parent)
{
    m_controller = new ZigbeeBridgeControllerTi(this);
    connect(m_controller, &ZigbeeBridgeControllerTi::availableChanged, this, &ZigbeeNetworkTi::onControllerAvailableChanged);
    connect(m_controller, &ZigbeeBridgeControllerTi::controllerStateChanged, this, &ZigbeeNetworkTi::onControllerStateChanged);
    connect(m_controller, &ZigbeeBridgeControllerTi::firmwareVersionChanged, this, &ZigbeeNetworkTi::firmwareVersionChanged);
    connect(m_controller, &ZigbeeBridgeControllerTi::permitJoinStateChanged, this, &ZigbeeNetworkTi::onPermitJoinStateChanged);
    connect(m_controller, &ZigbeeBridgeControllerTi::deviceIndication, this, &ZigbeeNetworkTi::onDeviceIndication);
    connect(m_controller, &ZigbeeBridgeControllerTi::apsDataIndicationReceived, this, &ZigbeeNetworkTi::onApsDataIndicationReceived);
    connect(m_controller, &ZigbeeBridgeControllerTi::apsDataConfirmReceived, this, &ZigbeeNetworkTi::onApsDataConfirmReceived);
    connect(m_controller, &ZigbeeBridgeControllerTi::nodeLeft, this, &ZigbeeNetworkTi::onNodeLeaveIndication);
}

ZigbeeBridgeController *ZigbeeNetworkTi::bridgeController() const
{
    if (!m_controller)
        return nullptr;

    return m_controller;
}

Zigbee::ZigbeeBackendType ZigbeeNetworkTi::backendType() const
{
    return Zigbee::ZigbeeBackendTypeTi;
}

ZigbeeNetworkReply *ZigbeeNetworkTi::sendRequest(const ZigbeeNetworkRequest &request)
{
    ZigbeeNetworkReply *reply = createNetworkReply(request);

    // Finish the reply right away if the network is offline
    if (!m_controller->available() || state() == ZigbeeNetwork::StateOffline || state() == ZigbeeNetwork::StateStopping) {
        finishNetworkReply(reply, ZigbeeNetworkReply::ErrorNetworkOffline);
        return reply;
    }

    if (state() == ZigbeeNetwork::StateStarting) {
        m_requestQueue.append(reply);
        return reply;
    }

    ZigbeeInterfaceTiReply *interfaceReply = m_controller->requestSendRequest(request);
    connect(interfaceReply, &ZigbeeInterfaceTiReply::finished, reply, [this, reply, interfaceReply](){
        if (interfaceReply->statusCode() != Ti::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Could not send request to controller." << interfaceReply->statusCode();
            finishNetworkReply(reply, ZigbeeNetworkReply::ErrorInterfaceError);
            return;
        }

        finishNetworkReply(reply, ZigbeeNetworkReply::ErrorNoError);
    });

    return reply;
}

void ZigbeeNetworkTi::setPermitJoining(quint8 duration, quint16 address)
{
    if (duration > 0) {
        qCDebug(dcZigbeeNetwork()) << "Set permit join for" << duration << "s on" << ZigbeeUtils::convertUint16ToHexString(address);
    } else {
        qCDebug(dcZigbeeNetwork()) << "Disable permit join on"<< ZigbeeUtils::convertUint16ToHexString(address);
    }

    ZigbeeInterfaceTiReply *requestPermitJoinReply = m_controller->requestPermitJoin(duration, address);
    connect(requestPermitJoinReply, &ZigbeeInterfaceTiReply::finished, this, [=](){
        if (requestPermitJoinReply->statusCode() != Ti::StatusCodeSuccess) {
            qCWarning(dcZigbeeNetwork()) << "Could not set permit join to" << duration << ZigbeeUtils::convertUint16ToHexString(address) << requestPermitJoinReply->statusCode();
            return;
        }
        qCDebug(dcZigbeeNetwork()) << "Permit join request finished successfully:" << duration;

        setPermitJoiningState(true, duration);

        // Opening the green power network too
        // Todo: This should probably be somewhere else, but not yet sure how other backeds deal with this
        QByteArray payload;
        QDataStream stream(&payload, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << static_cast<quint8>(0x0b); // options
        stream << static_cast<quint16>(duration);


        // Build ZCL frame control
        ZigbeeClusterLibrary::FrameControl frameControl;
        frameControl.frameType = ZigbeeClusterLibrary::FrameTypeClusterSpecific;
        frameControl.manufacturerSpecific = false;
        frameControl.direction = ZigbeeClusterLibrary::DirectionServerToClient;
        frameControl.disableDefaultResponse = true;

        // Build ZCL header
        ZigbeeClusterLibrary::Header header;
        header.frameControl = frameControl;
        header.command = 0x02;// TODO: ZigbeeClusterGreenPower::ClusterCommandCommissioningMode;
        header.transactionSequenceNumber = generateSequenceNumber();

        // Build ZCL frame
        ZigbeeClusterLibrary::Frame frame;
        frame.header = header;
        frame.payload = payload;

        ZigbeeNetworkRequest request;
        request.setRequestId(generateSequenceNumber());
        request.setDestinationAddressMode(Zigbee::DestinationAddressModeShortAddress);
        request.setDestinationShortAddress(0xFFFD);
        request.setDestinationEndpoint(242); // Green Power Endpoint
        request.setProfileId(Zigbee::ZigbeeProfileDevice); // ZDP
        request.setClusterId(0x21);
        request.setTxOptions(static_cast<Zigbee::ZigbeeTxOption>(0x00)); // FIXME: There should be TxOptionsNone  I guess...
        request.setSourceEndpoint(242); // Green Power Endpoint
        request.setRadius(30); // FIXME: There should be a more clever way to figure out the radius
        request.setAsdu(ZigbeeClusterLibrary::buildFrame(frame));

        m_controller->requestSendRequest(request);
    });
}

void ZigbeeNetworkTi::initController()
{
    qCDebug(dcZigbeeNetwork()) << "Initializing controller";
    setState(StateStarting);
    setError(ErrorNoError);

    ZigbeeInterfaceTiReply *initReply = m_controller->init();
    connect(initReply, &ZigbeeInterfaceTiReply::finished, this, [=](){
        if (initReply->statusCode() != Ti::StatusCodeSuccess) {
            qCWarning(dcZigbeeNetwork()) << "Error initializing controller";
            setState(StateUninitialized);
            setError(ErrorZigbeeError);
            return;
        }
    });
}

void ZigbeeNetworkTi::commissionController()
{
    qCDebug(dcZigbeeNetwork()) << "Commissioning controller";

    quint16 panId = ZigbeeUtils::generateRandomPanId();

    ZigbeeInterfaceTiReply *commissionReply = m_controller->commission(Ti::DeviceLogicalTypeCoordinator, panId, channelMask());
    connect(commissionReply, &ZigbeeInterfaceTiReply::finished, this, [=](){
        if (commissionReply->statusCode() != Ti::StatusCodeSuccess) {
            qCWarning(dcZigbeeNetwork()) << "Error commissioning controller.";
            setState(StateUninitialized);
            setError(ErrorZigbeeError);
            return;
        }

        qCDebug(dcZigbeeNetwork()) << "Controller commissioned";

        startControllerNetwork();
        setMacAddress(m_controller->networkConfiguration().ieeeAddress);
    });
}

void ZigbeeNetworkTi::startControllerNetwork()
{
    setState(StateStarting);
    qCDebug(dcZigbeeNetwork()) << "Starting network on controller...";
    ZigbeeInterfaceTiReply *startReply = m_controller->start();
    connect(startReply, &ZigbeeInterfaceTiReply::finished, this, [=]() {
        if (startReply->statusCode() != Ti::StatusCodeSuccess) {
            qCWarning(dcZigbeeNetwork()) << "Error starting network.";
            setState(StateOffline);
            setError(ErrorZigbeeError);
            return;
        }
    });
}

void ZigbeeNetworkTi::processGreenPowerFrame(const Zigbee::ApsdeDataIndication &indication)
{
    ZigbeeClusterLibrary::Frame inputFrame = ZigbeeClusterLibrary::parseFrameData(indication.asdu);
    QDataStream inputStream(inputFrame.payload);
    inputStream.setByteOrder(QDataStream::LittleEndian);
    quint8 commandId, payloadSize;
    quint16 options;
    quint32 srcId, frameCounter;
    inputStream >> options >> srcId >> frameCounter >> commandId >> payloadSize;
    QByteArray commandFrame = inputFrame.payload.right(payloadSize);
    qCWarning(dcZigbeeNetwork()) << "Green Power frame:" << options << srcId << frameCounter << commandId << payloadSize << commandFrame.toHex();

    if (commandId == 0xE0) {
        qCWarning(dcZigbeeNetwork()) << "Green power commissioning";
        QDataStream commandStream(commandFrame);
        commandStream.setByteOrder(QDataStream::LittleEndian);
        quint8 deviceId, inputOptions, extendedOptions;
        QByteArray securityKey;
        quint32 keyMic;
        quint32 outgoingCounter;
        commandStream >> deviceId >> inputOptions >> extendedOptions;
        for (int i = 0; i < 16; i++) {
            quint8 byte;
            commandStream >> byte;
            securityKey.append(byte);
        }
        commandStream >> keyMic >> outgoingCounter;

        // Send commissioning reply
        QByteArray payload;
        QDataStream stream(&payload, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        ZigbeeDataType options = ZigbeeDataType(static_cast<quint32>(0x00e548), Zigbee::Uint24); // options
        for (int i = 0; i < options.data().length(); i++) {
            stream << static_cast<quint8>(options.data().at(i));
        }
        stream << srcId;
        stream << static_cast<quint16>(0x0b84); // Green Power group as configured during startup
        stream << deviceId;
        stream << outgoingCounter;
        payload.append(encryptSecurityKey(srcId, securityKey));

        // Build ZCL frame control
        ZigbeeClusterLibrary::FrameControl frameControl;
        frameControl.frameType = ZigbeeClusterLibrary::FrameTypeClusterSpecific;
        frameControl.manufacturerSpecific = false;
        frameControl.direction = ZigbeeClusterLibrary::DirectionServerToClient;
        frameControl.disableDefaultResponse = true;

        // Build ZCL header
        ZigbeeClusterLibrary::Header header;
        header.frameControl = frameControl;
        header.command = 0x01; // TODO: ZigbeeClusterGreenPower::ClusterCommandPairing;
        header.transactionSequenceNumber = generateSequenceNumber() - 1;

        // Build ZCL frame
        ZigbeeClusterLibrary::Frame frame;
        frame.header = header;
        frame.payload = payload;

        ZigbeeNetworkRequest request;
        request.setRequestId(generateSequenceNumber() + 1);
        request.setDestinationAddressMode(Zigbee::DestinationAddressModeShortAddress);
        request.setDestinationShortAddress(0xFFFD);
        request.setDestinationEndpoint(242); // Green Power Endpoint
        request.setProfileId(Zigbee::ZigbeeProfileDevice); // ZDP
        request.setClusterId(0x21);
        request.setTxOptions(static_cast<Zigbee::ZigbeeTxOption>(0x00)); // FIXME: There should be TxOptionsNone  I guess...
        request.setSourceEndpoint(242); // Green Power Endpoint
        request.setRadius(30); // FIXME: There should be a more clever way to figure out the radius
        request.setAsdu(ZigbeeClusterLibrary::buildFrame(frame));

        m_controller->requestSendRequest(request);

        QStringList l;
        for (int i = 0; i < request.asdu().length(); i++) {
            l.append(QString::number(static_cast<quint8>(request.asdu().at(i))));
        }

        // TODO: create the GreenPower Node here once GreenPower support is added properly
        // emit greenPowerDeviceJoined(srcId, deviceId, indication.sourceIeeeAddress())
    }
}

QByteArray ZigbeeNetworkTi::encryptSecurityKey(quint32 sourceId, const QByteArray &securityKey)
{
#ifndef DISABLE_QCA
    QByteArray sourceIdArray;
    sourceIdArray.append(static_cast<quint8>(sourceId & 0x000000ff));
    sourceIdArray.append(static_cast<quint8>((sourceId & 0x0000ff00) >> 8));
    sourceIdArray.append(static_cast<quint8>((sourceId & 0x00ff0000) >> 16));
    sourceIdArray.append(static_cast<quint8>((sourceId & 0xff000000) >> 24));

    QByteArray nonce(13, Qt::Uninitialized);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            nonce[4 * i + j] = sourceIdArray.at(j);
        }
    }
    nonce[12] = 0x05;

    QByteArray zigbeeLinkKey = securityConfiguration().globalTrustCenterLinkKey().toByteArray();

    QCA::Initializer init;
    QCA::Cipher cipher("aes128", QCA::Cipher::CCM, QCA::Cipher::DefaultPadding, QCA::Encode, zigbeeLinkKey, nonce);
    QByteArray encrypted = cipher.update(securityKey).toByteArray();
    encrypted.append(cipher.final().toByteArray());
    return encrypted;
#else
    Q_UNUSED(sourceId)
    qCWarning(dcZigbeeNetwork()) << "Greenpower encryption requires AES-128-CCM (requires qca-qt5-2 >= 2.2.0)";
    return securityKey;
#endif
}

void ZigbeeNetworkTi::onControllerAvailableChanged(bool available)
{
    if (!available) {
        qCWarning(dcZigbeeNetwork()) << "Hardware controller is not available any more.";
        setError(ErrorHardwareUnavailable);
        setPermitJoiningState(false);
        setState(StateOffline);
        setError(ErrorHardwareUnavailable);
    } else {
        setPermitJoiningState(false);
        setState(StateOffline);
        setError(ErrorNoError);
        qCDebug(dcZigbeeNetwork()) << "Hardware controller is now available.";
        initController();
    }
}

void ZigbeeNetworkTi::onControllerStateChanged(ZigbeeBridgeControllerTi::ControllerState state)
{
    switch (state) {
    case ZigbeeBridgeControllerTi::ControllerStateDown:
        setState(StateOffline);
        break;
    case ZigbeeBridgeControllerTi::ControllerStateInitialized:

        // The mac address of the controller will be stored locally when the coordinator node replies
        // to the node descriptor request which includes the IEEE address.
        // If we don't know the mac address yet, it means that we've never had the zigbee network up and running
        // => start the commissioning procedure, else, directly start the network
        // Note: if the user messes around with the stick by provisioning stuff with another instance/software, it
        // will not be detected currently and the "wrong" network starts up.
        qCDebug(dcZigbeeNetwork()) << "Controller initialized";
        qCDebug(dcZigbeeNetwork()) << "Stored MAC address:" << macAddress() << "Controller MAC address:" << m_controller->networkConfiguration().ieeeAddress;

        if (macAddress() != ZigbeeAddress("00:00:00:00:00:00:00:00") && m_controller->networkConfiguration().ieeeAddress != macAddress()) {
            qCWarning(dcZigbeeNetwork()) << "The controller MAC address changed. Please connect the original controller or create a new network.";
            setState(StateUninitialized);
            setError(ErrorHardwareModuleChanged);
            stopNetwork();
            return;
        }
        if (macAddress() == ZigbeeAddress("00:00:00:00:00:00:00")) {
            // Controller network is not commissioned yet
            qCDebug(dcZigbeeNetwork()) << "Controller is not comissioned yet. Comissioning now...";
            commissionController();
            return;
        }

        qCDebug(dcZigbeeNetwork()) << "Controller is ready and commissioned.";
        startControllerNetwork();

        break;
    case ZigbeeBridgeControllerTi::ControllerStateRunning: {
        qCDebug(dcZigbeeNetwork()) << "Controller network running. Registering endpoints on controller..";

        setPanId(m_controller->networkConfiguration().panId);
        setExtendedPanId(m_controller->networkConfiguration().extendedPanId);
        setChannel(m_controller->networkConfiguration().currentChannel);

        // TODO: This should be public API of libnymea-zigbee so that the application layer (e.g. nymea-plugins)
        // can register the endpoints it needs for the particular application/device.
        // Fow now we're registering HomeAutomation, LightLink and GreenPower endpoints.
        m_controller->registerEndpoint(1, Zigbee::ZigbeeProfileHomeAutomation, 5, 0, {ZigbeeClusterLibrary::ClusterIdOtaUpgrade});
        m_controller->registerEndpoint(12, Zigbee::ZigbeeProfileLightLink, 5, 0);

        // The Green Power endpoing is a bit special, it also needs to be added to a group
        ZigbeeInterfaceTiReply *registerLLEndpointReply = m_controller->registerEndpoint(242, Zigbee::ZigbeeProfileGreenPower, 5, 0);
        connect(registerLLEndpointReply, &ZigbeeInterfaceTiReply::finished, this, [=]() {
            if (registerLLEndpointReply->statusCode() != Ti::StatusCodeSuccess) {
                qCWarning(dcZigbeeNetwork()) << "Error registering GreenPower endpoint.";
                setState(StateOffline);
                return;
            }
            qCDebug(dcZigbeeNetwork()) << "Registered GreenPower endpoint on coordinator node.";

            ZigbeeInterfaceTiReply *addGEndpointGroupReply = m_controller->addEndpointToGroup(242, 0x0b84);
            connect(addGEndpointGroupReply, &ZigbeeInterfaceTiReply::finished, this, [=]() {
                if (addGEndpointGroupReply->statusCode() != Ti::StatusCodeSuccess) {
                    qCWarning(dcZigbeeNetwork()) << "Error adding GreenPower endpoint to group.";
                    setState(StateOffline);
                    return;
                }

                // Now we're done. If this is a first start (no coordinator node loaded from configs) we'll add
                // ourselves now and start the inspection.
                if (!m_coordinatorNode) {
                    qCDebug(dcZigbeeNetwork()) << "Initializing coordinator node:" << m_controller->networkConfiguration();
                    ZigbeeNode *coordinatorNode = createNode(m_controller->networkConfiguration().shortAddress, m_controller->networkConfiguration().ieeeAddress, this);
                    m_coordinatorNode = coordinatorNode;
                    addUnitializedNode(coordinatorNode);
                }


                ZigbeeInterfaceTiReply *ledReply = m_controller->setLed(false);
                connect(ledReply, &ZigbeeInterfaceTiReply::finished, this, [=]() {

                    setState(StateRunning);

                    // Introspecing ourselves on every start. Most of the times this wouldn't be needed, but if the above
                    // endpoints are changed (e.g. on a future upgrade), we'll want to refresh.
                    m_coordinatorNode->startInitialization();
                    connect(m_coordinatorNode, &ZigbeeNode::stateChanged, this, [=](ZigbeeNode::State state){
                        if (state == ZigbeeNode::StateInitialized) {
                            setNodeInformation(m_coordinatorNode, "z-Stack", "", bridgeController()->firmwareVersion());
                        }
                    });

                    while (!m_requestQueue.isEmpty()) {
                        ZigbeeNetworkReply *reply = m_requestQueue.takeFirst();
                        ZigbeeInterfaceTiReply *interfaceReply = m_controller->requestSendRequest(reply->request());
                        connect(interfaceReply, &ZigbeeInterfaceTiReply::finished, reply, [this, reply, interfaceReply](){
                            if (interfaceReply->statusCode() != Ti::StatusCodeSuccess) {
                                qCWarning(dcZigbeeController()) << "Could send request to controller." << interfaceReply->statusCode();
                                finishNetworkReply(reply, ZigbeeNetworkReply::ErrorInterfaceError);
                                return;
                            }
                            finishNetworkReply(reply, ZigbeeNetworkReply::ErrorNoError);
                        });
                    }
                });
            });
        });

        break;
    }
    }
}

void ZigbeeNetworkTi::onPermitJoinStateChanged(quint8 duration)
{
    setPermitJoiningState(duration > 0, duration);
    m_controller->setLed(duration > 0);
}

void ZigbeeNetworkTi::onDeviceIndication(quint16 shortAddress, const ZigbeeAddress &ieeeAddress, quint8 macCapabilities)
{
    onDeviceAnnounced(shortAddress, ieeeAddress, macCapabilities);
}

void ZigbeeNetworkTi::onNodeLeaveIndication(const ZigbeeAddress &ieeeAddress, bool request, bool remove, bool rejoin)
{
    qCDebug(dcZigbeeNetwork()) << "Received node leave indication" << ieeeAddress.toString() << "request:" << request << "remove:" << remove << "rejoining:" << rejoin;
    if (!hasNode(ieeeAddress)) {
        qCDebug(dcZigbeeNetwork()) << "Node left the network" << ieeeAddress.toString();
        return;
    }

    ZigbeeNode *node = getZigbeeNode(ieeeAddress);
    qCDebug(dcZigbeeNetwork()) << node << "left the network";
    removeNode(node);
}

void ZigbeeNetworkTi::onApsDataConfirmReceived(const Zigbee::ApsdeDataConfirm &confirm)
{
    qCDebug(dcZigbeeNetwork()) << "Data confirm received:" << confirm;
}

void ZigbeeNetworkTi::onApsDataIndicationReceived(const Zigbee::ApsdeDataIndication &indication)
{
    // If it's for the green power endpoint, we'll have to do a commissioning.
    // TODO: This should probably be in ZigbeeNode or ZigbeeDeviceObject, but not yet sure how other backends deal with it
    if (indication.destinationEndpoint == 242) {
        processGreenPowerFrame(indication);
    }

    // Check if this indocation is related to any pending reply
    if (indication.profileId == Zigbee::ZigbeeProfileDevice) {
        handleZigbeeDeviceProfileIndication(indication);
        return;
    }

    // Else let the node handle this indication
    handleZigbeeClusterLibraryIndication(indication);
}

void ZigbeeNetworkTi::startNetwork()
{
    loadNetwork();

    if (!m_controller->enable(serialPortName(), serialBaudrate())) {
        setPermitJoiningState(false);
        setState(StateOffline);
        setError(ErrorHardwareUnavailable);
        return;
    }

    setPermitJoiningState(false);
}

void ZigbeeNetworkTi::stopNetwork()
{
    setState(StateStopping);
    m_controller->disable();
    setState(StateOffline);
}

void ZigbeeNetworkTi::reset()
{
    qCDebug(dcZigbeeNetwork()) << "Resetting controller.";
    m_controller->reset();
}

void ZigbeeNetworkTi::factoryResetNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Factory resetting network and forget all information. This cannot be undone.";

    ZigbeeInterfaceTiReply *reply = m_controller->factoryReset();
    connect(reply, &ZigbeeInterfaceTiReply::finished, this, [=](){
        qCDebug(dcZigbeeNetwork()) << "Factory reset reply finished" << reply->statusCode();

        m_controller->disable();
        clearSettings();
        setMacAddress(ZigbeeAddress("00:00:00:00:00:00"));
        setState(StateOffline);
        setError(ErrorNoError);
        qCDebug(dcZigbeeNetwork()) << "The factory reset is finished. Restarting with a fresh network.";
        startNetwork();
    });
}

void ZigbeeNetworkTi::destroyNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Destroy network and delete the database";
    m_controller->disable();
    clearSettings();
}
