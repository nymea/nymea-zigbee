#include "zigbeenetworkmanager.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QDateTime>
#include <QDataStream>
#include <QSettings>

ZigbeeNetworkManager::ZigbeeNetworkManager(QObject *parent) :
    ZigbeeNetwork(ZigbeeNetwork::ControlerTypeNxp, parent)
{

    //connect(controller(), &ZigbeeBridgeController::messageReceived, this, &ZigbeeNetworkManager::onMessageReceived);

    //Q_UNUSED(channel)
    //    if (controller()->available()) {
    //        qCDebug(dcZigbeeNetwork()) << "Bridge controller started successfully on" << controller;
    //    } else {
    //        qCCritical(dcZigbeeNetwork()) << "The zigbee controller is not available on" << controller;
    //        return;
    //    }

    //    QSettings settings;
    //    qCDebug(dcZigbeeNetwork()) << "Loading settings from" << settings.fileName();
    //    settings.beginGroup("Network");
    //    m_extendedPanId = static_cast<quint64>(settings.value("panId", 0).toLongLong());
    //    qCDebug(dcZigbeeNetwork()) << "Loading saved pan id" << m_extendedPanId;
    //    if (m_extendedPanId == 0) {
    //        m_extendedPanId = generateRandomPanId();
    //        settings.setValue("panId", m_extendedPanId);
    //    }
    //    settings.endGroup();

    //    qCDebug(dcZigbeeNetwork()) << "PAN Id:" << m_extendedPanId << ZigbeeUtils::convertUint64ToHexString(m_extendedPanId);

    //    // Create channel mask
    //    // Note: normal number passed, that specific channel will be used || Bitfield: all channels would be 0x07FFF800
    //    quint32 channelMask = 0;
    //    if (channel == 0) {
    //        qCDebug(dcZigbeeNetwork()) << "Using quitest channel for the zigbee network.";
    //    } else {
    //        channelMask |= 1 << (channel);
    //        qCDebug(dcZigbeeNetwork()) << "Using channel" << channel << "for the zigbee network.";
    //    }

    //    loadNetwork();
    //    resetController();
    //    getVersion();
    //    init();
}

QString ZigbeeNetworkManager::controllerVersion() const
{
    return m_controllerVersion;
}

bool ZigbeeNetworkManager::networkRunning() const
{
    return state() == ZigbeeNetwork::StateRunning;
}

void ZigbeeNetworkManager::setStartingState(ZigbeeNetworkManager::StartingState state)
{
    if (m_startingState == state)
        return;

    qCDebug(dcZigbeeNetwork()) << state;
    m_startingState = state;

    switch (m_startingState) {
    case StartingStateNone:
        break;
    case StartingStateErase: {
        ZigbeeInterfaceReply *reply = m_controller->commandErasePersistantData();
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandErasePersistentDataFinished);
        break;
    }
    case StartingStateReset: {
        ZigbeeInterfaceReply *reply = m_controller->commandResetController();
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandResetControllerFinished);
        break;
    }
    case StartingStateGetVersion: {
        ZigbeeInterfaceReply *reply = m_controller->commandGetVersion();
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandGetVersionFinished);
        break;
    }
    case StartingStateSetPanId: {
        if (extendedPanId() == 0) {
            setExtendedPanId(ZigbeeUtils::generateRandomPanId());
        }
        ZigbeeInterfaceReply *reply = m_controller->commandSetExtendedPanId(extendedPanId());
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandSetExtendedPanIdFinished);
        break;
    }
    case StartingStateSetChannel: {
        // Create channel mask
        // Note: normal number passed, that specific channel will be used || Bitfield: all channels would be 0x07FFF800
        quint32 channelMask = 0;
        if (channel() == 0) {
            qCDebug(dcZigbeeNetwork()) << "Autoselect quitest channel for the zigbee network.";
        } else {
            channelMask |= 1 << (channel());
            qCDebug(dcZigbeeNetwork()) << "Using channel" << channel() << "for the zigbee network.";
        }
        ZigbeeInterfaceReply *reply = m_controller->commandSetChannelMask(channelMask);
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandSetChannelMaskFinished);
        break;
    }
    case StartingStateSetSecurity: {
        ZigbeeInterfaceReply *reply = m_controller->commandSetSecurityStateAndKey(4, 0, 1, "5A6967426565416C6C69616E63653039");
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandSetSecurityFinished);
        break;
    }
    case StartingStateStartNetwork: {
        ZigbeeInterfaceReply *reply = m_controller->commandStartNetwork();
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandStartNetworkFinished);
        break;
    }
    case StartingStateReadeNodeDescriptor: {
        ZigbeeInterfaceReply *reply = m_controller->commandNodeDescriptorRequest(shortAddress());
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandNodeDescriptorRequestFinished);
        break;
    }
    case StartingStateReadSimpleDescriptor: {
        ZigbeeInterfaceReply *reply = m_controller->commandSimpleDescriptorRequest(shortAddress(), endPoint());
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandSimpleDescriptorRequestFinished);
        break;
    }
    case StartingStateReadPowerDescriptor: {
        ZigbeeInterfaceReply *reply = m_controller->commandPowerDescriptorRequest(shortAddress());
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandPowerDescriptorRequestFinished);
        break;
    }
    }
}


//void ZigbeeNetworkManager::parseNetworkFormed(const QByteArray &data)
//{
//    // Parse network status
//    quint8 networkStatus = static_cast<quint8>(data.at(0));
//    QString networkStatusString;

//    if (networkStatus == 0) {
//        networkStatusString = "joined";
//    } else if (networkStatus == 1) {
//        networkStatusString = "created";
//    } else if (networkStatus >= 128 && networkStatus <= 244) {
//        networkStatusString = "failed: Zigbee event code: " + QString::number(networkStatus);
//    } else {
//        networkStatusString = "unknown";
//    }

//    quint16 shortAddress = ZigbeeUtils::convertByteArrayToUint16(data.mid(1, 2));
//    quint64 extendedAddress = ZigbeeUtils::convertByteArrayToUint64(data.mid(3, 8));

//    // Parse network channel
//    quint8 channel = static_cast<quint8>(data.at(11));

//    qCDebug(dcZigbeeNetwork()).noquote() << "Network" << networkStatusString;
//    qCDebug(dcZigbeeNetwork()) << "    Address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
//    qCDebug(dcZigbeeNetwork()) << "    Extended address:" << ZigbeeAddress(extendedAddress);
//    qCDebug(dcZigbeeNetwork()) << "    Channel:" << channel;

//    // Set the node information
//    setShortAddress(shortAddress);
//    setExtendedAddress(ZigbeeAddress(extendedAddress));

//}

//void ZigbeeNetworkManager::requestMatchDescriptor(const quint16 &shortAddress, const Zigbee::ZigbeeProfile &profile)
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
//    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onRequestMatchDescriptorFinished);
//}

void ZigbeeNetworkManager::onCommandResetControllerFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkManager::onCommandErasePersistentDataFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    if (m_startingState == StartingStateErase) setStartingState(StartingStateReset);
}

void ZigbeeNetworkManager::onCommandGetVersionFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
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

    m_controllerVersion = QString("%1.%2").arg(majorVersion).arg(minorVersion);
    qCDebug(dcZigbeeNetwork()) << "Controller version:" << m_controllerVersion;

    if (m_startingState == StartingStateGetVersion) setStartingState(StartingStateSetPanId);
}

void ZigbeeNetworkManager::onCommandSetExtendedPanIdFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    if (m_startingState == StartingStateSetPanId) setStartingState(StartingStateSetChannel);
}

void ZigbeeNetworkManager::onCommandSetChannelMaskFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeNetwork()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    if (m_startingState == StartingStateSetChannel) setStartingState(StartingStateSetSecurity);
}

void ZigbeeNetworkManager::onCommandSetDeviceTypeFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkManager::onCommandStartNetworkFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeNetwork()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    qCDebug(dcZigbeeController()) << reply->additionalMessage();
    processNetworkFormed(reply->additionalMessage());
    if (m_startingState == StartingStateStartNetwork) setStartingState(StartingStateReadeNodeDescriptor);
}

void ZigbeeNetworkManager::onCommandStartScanFinished()
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

void ZigbeeNetworkManager::onCommandGetPermitJoiningStatusFinished()
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

void ZigbeeNetworkManager::onCommandPermitJoiningFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkManager::onCommandRequestMatchDescriptorFinished()
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

void ZigbeeNetworkManager::onCommandSetSecurityFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    if (m_startingState == StartingStateSetSecurity) setStartingState(StartingStateStartNetwork);
}

void ZigbeeNetworkManager::onCommandNetworkAddressRequestFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    qCDebug(dcZigbeeController()) << reply->additionalMessage();

    quint8 sequenceNumber = static_cast<quint8>(reply->additionalMessage().data().at(0));
    quint8 statusCode = static_cast<quint8>(reply->additionalMessage().data().at(1));
    quint64 ieeeAddress = ZigbeeUtils::convertByteArrayToUint64(reply->additionalMessage().data().mid(2, 8));
    quint16 shortAddress = ZigbeeUtils::convertByteArrayToUint16(reply->additionalMessage().data().mid(10, 2));
    quint8 deviceCount = static_cast<quint8>(reply->additionalMessage().data().at(12));
    quint8 startIndex = static_cast<quint8>(reply->additionalMessage().data().at(13));

    qCDebug(dcZigbeeNetwork()) << "Network address response:";
    qCDebug(dcZigbeeNetwork()) << "    SQN:" << sequenceNumber;
    qCDebug(dcZigbeeNetwork()) << "    Status:" << statusCode;
    qCDebug(dcZigbeeNetwork()) << "    Address:" << shortAddress << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbeeNetwork()) << "    Extended address:" << ZigbeeAddress(ieeeAddress);
    qCDebug(dcZigbeeNetwork()) << "    Deice count:" << deviceCount;
    qCDebug(dcZigbeeNetwork()) << "    Start index:" << startIndex;

}

void ZigbeeNetworkManager::onCommandAuthenticateDeviceFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    qCDebug(dcZigbeeController()) << reply->additionalMessage();

    quint64 gatewayIeeeAddress = ZigbeeUtils::convertByteArrayToUint64(reply->additionalMessage().data().mid(0, 8));
    QString encryptedKey = reply->additionalMessage().data().mid(8, 16).toHex();
    QByteArray mic = reply->additionalMessage().data().mid(24, 4);
    quint64 initiatorIeeeAddress = ZigbeeUtils::convertByteArrayToUint64(reply->additionalMessage().data().mid(28, 8));
    quint8 activeKeySequenceNumber = static_cast<quint8>(reply->additionalMessage().data().at(36));
    quint8 channel = static_cast<quint8>(reply->additionalMessage().data().at(37));
    quint16 shortPan = ZigbeeUtils::convertByteArrayToUint16(reply->additionalMessage().data().mid(38, 2));
    quint64 extendedPanId = ZigbeeUtils::convertByteArrayToUint64(reply->additionalMessage().data().mid(40, 8));


    qCDebug(dcZigbeeNetwork()) << "Authentication response:";
    qCDebug(dcZigbeeNetwork()) << "    Gateways address:" << ZigbeeAddress(gatewayIeeeAddress);
    qCDebug(dcZigbeeNetwork()) << "    Key:" << encryptedKey;
    qCDebug(dcZigbeeNetwork()) << "    MIC:" << mic.toHex();
    qCDebug(dcZigbeeNetwork()) << "    Initiator address:" << ZigbeeAddress(initiatorIeeeAddress);
    qCDebug(dcZigbeeNetwork()) << "    Active key sequence number:" << activeKeySequenceNumber;
    qCDebug(dcZigbeeNetwork()) << "    Channel:" << channel;
    qCDebug(dcZigbeeNetwork()) << "    Short PAN ID:" << ZigbeeUtils::convertUint16ToHexString(shortPan);
    qCDebug(dcZigbeeNetwork()) << "    Extended PAN ID:" << extendedPanId << ZigbeeUtils::convertUint64ToHexString(extendedPanId);
}

void ZigbeeNetworkManager::processNetworkFormed(const ZigbeeInterfaceMessage &message)
{
    // Parse network status
    QByteArray data = message.data();
    quint8 networkStatus = static_cast<quint8>(data.at(0));
    QString networkStatusString;

    if (networkStatus == 0) {
        networkStatusString = "joined";
    } else if (networkStatus == 1) {
        networkStatusString = "created";
    } else if (networkStatus >= 128 && networkStatus <= 244) {
        networkStatusString = "failed: Zigbee event code: " + QString::number(networkStatus);
    } else {
        networkStatusString = "unknown";
    }

    quint16 shortAddress = ZigbeeUtils::convertByteArrayToUint16(data.mid(1, 2));
    quint64 extendedAddress = ZigbeeUtils::convertByteArrayToUint64(data.mid(3, 8));

    // Parse network channel
    quint8 channel = static_cast<quint8>(data.at(11));

    qCDebug(dcZigbeeNetwork()).noquote() << "Network" << networkStatusString;
    qCDebug(dcZigbeeNetwork()) << "    Address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbeeNetwork()) << "    Extended address:" << ZigbeeAddress(extendedAddress);
    qCDebug(dcZigbeeNetwork()) << "    Channel:" << channel;

    // Set the node information
    setShortAddress(shortAddress);
    setExtendedAddress(ZigbeeAddress(extendedAddress));
    setChannel(channel);

    addNode(this);

}

void ZigbeeNetworkManager::onCommandEnableWhitelistFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkManager::onCommandNodeDescriptorRequestFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";

    quint8 sequenceNumber = static_cast<quint8>(reply->additionalMessage().data().at(0));
    quint8 status = static_cast<quint8>(reply->additionalMessage().data().at(1));

    Q_UNUSED(sequenceNumber)
    Q_UNUSED(status)

    quint16 shortAddress = static_cast<quint16>(reply->additionalMessage().data().at(2));
    shortAddress <<= 8;
    shortAddress |= reply->additionalMessage().data().at(3);

    quint16 manufacturerCode = static_cast<quint16>(reply->additionalMessage().data().at(4));
    manufacturerCode <<= 8;
    manufacturerCode |= reply->additionalMessage().data().at(5);

    quint16 maximalRxSize = static_cast<quint16>(reply->additionalMessage().data().at(6));
    maximalRxSize <<= 8;
    maximalRxSize |= reply->additionalMessage().data().at(7);

    quint16 maximalTxSize = static_cast<quint16>(reply->additionalMessage().data().at(8));
    maximalTxSize <<= 8;
    maximalTxSize |= reply->additionalMessage().data().at(9);

    quint16 serverMask = static_cast<quint16>(reply->additionalMessage().data().at(10));
    serverMask <<= 8;
    serverMask |= reply->additionalMessage().data().at(11);

    quint8 descriptorFlag = static_cast<quint8>(reply->additionalMessage().data().at(12));
    quint8 macFlags = static_cast<quint8>(reply->additionalMessage().data().at(13));
    quint8 maxBufferSize = static_cast<quint8>(reply->additionalMessage().data().at(14));

    quint16 bitField = static_cast<quint16>(reply->additionalMessage().data().at(15));
    bitField <<= 8;
    bitField |= reply->additionalMessage().data().at(16);

    // Get node object
    ZigbeeNode *node = getZigbeeNode(shortAddress);

    // Set node data
    node->setManufacturerCode(manufacturerCode);
    node->setMaximumRxSize(maximalRxSize);
    node->setMaximumTxSize(maximalTxSize);
    node->setMaximumBufferSize(maxBufferSize);
    node->setServerMask(serverMask);
    node->setDescriptorFlag(descriptorFlag);
    node->setMacCapabilitiesFlag(macFlags);

    // Parse bit field
    bool isCoordinator = ((bitField >> 0) & 0x0001);
    bool isRouter = ((bitField >> 1) & 0x0001);
    bool isEndDevice = ((bitField >> 2) & 0x0001);

    node->setComplexDescriptorAvailable((bitField >> 3) & 0x0001);
    node->setUserDescriptorAvailable((bitField >> 4) & 0x0001);

    if (isCoordinator && !isRouter && !isEndDevice) {
        node->setNodeType(NodeTypeCoordinator);
    } else if (!isCoordinator && isRouter && !isEndDevice) {
        node->setNodeType(NodeTypeRouter);
    } else if (!isCoordinator && !isRouter && isEndDevice) {
        node->setNodeType(NodeTypeEndDevice);
    }


    //    qCDebug(dcZigbeeNetwork()) << "Node descriptor:";
    //    qCDebug(dcZigbeeNetwork()) << "    Node type:" << node->nodeType();
    //    qCDebug(dcZigbeeNetwork()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    //    qCDebug(dcZigbeeNetwork()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    //    qCDebug(dcZigbeeNetwork()) << "    Short address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    //    qCDebug(dcZigbeeNetwork()) << "    Manufacturer code:" << ZigbeeUtils::convertUint16ToHexString(manufacturerCode);
    //    qCDebug(dcZigbeeNetwork()) << "    Maximum Rx size:" << ZigbeeUtils::convertUint16ToHexString(node->maximumRxSize());
    //    qCDebug(dcZigbeeNetwork()) << "    Maximum Tx size:" << ZigbeeUtils::convertUint16ToHexString(node->maximumTxSize());
    //    qCDebug(dcZigbeeNetwork()) << "    Server mask:" << ZigbeeUtils::convertUint16ToHexString(serverMask);
    //    qCDebug(dcZigbeeNetwork()) << "        Primary Trust center:" << m_isPrimaryTrustCenter;
    //    qCDebug(dcZigbeeNetwork()) << "        Backup Trust center:" << m_isBackupTrustCenter;
    //    qCDebug(dcZigbeeNetwork()) << "        Primary Binding cache:" << m_isPrimaryBindingCache;
    //    qCDebug(dcZigbeeNetwork()) << "        Backup Binding cache:" << m_isBackupBindingCache;
    //    qCDebug(dcZigbeeNetwork()) << "        Primary Discovery cache:" << m_isPrimaryDiscoveryCache;
    //    qCDebug(dcZigbeeNetwork()) << "        Backup Discovery cache:" << m_isBackupDiscoveryCache;
    //    qCDebug(dcZigbeeNetwork()) << "        Network Manager:" << m_isNetworkManager;
    //    qCDebug(dcZigbeeNetwork()) << "    Descriptor flag:" << ZigbeeUtils::convertByteToHexString(descriptorFlag);
    //    qCDebug(dcZigbeeNetwork()) << "        Extended active endpoint list available:" << extendedActiveEndpointListAvailable;
    //    qCDebug(dcZigbeeNetwork()) << "        Extended simple descriptor list available:" << extendedSimpleDescriptorListAvailable;
    //    qCDebug(dcZigbeeNetwork()) << "    MAC flags:" << ZigbeeUtils::convertByteToHexString(macFlags);
    //    qCDebug(dcZigbeeNetwork()) << "        Receiver on when idle:" << m_receiverOnWhenIdle;
    //    qCDebug(dcZigbeeNetwork()) << "        Security capability:" << m_securityCapability;
    //    qCDebug(dcZigbeeNetwork()) << "    Maximum buffer size:" << ZigbeeUtils::convertByteToHexString(maxBufferSize);
    //    qCDebug(dcZigbeeNetwork()) << "    Bit field:" << ZigbeeUtils::convertUint16ToHexString(bitField);
    //    qCDebug(dcZigbeeNetwork()) << "        Is coordinator:" << isCoordinator;
    //    qCDebug(dcZigbeeNetwork()) << "        Is router:" << isRouter;
    //    qCDebug(dcZigbeeNetwork()) << "        Is end device:" << isEndDevice;
    //    qCDebug(dcZigbeeNetwork()) << "        Complex desciptor available:" << complexDescriptorAvailable;
    //    qCDebug(dcZigbeeNetwork()) << "        User desciptor available:" << userDescriptorAvailable;

    if (m_startingState == StartingStateReadeNodeDescriptor) setStartingState(StartingStateReadSimpleDescriptor);
}

void ZigbeeNetworkManager::onCommandSimpleDescriptorRequestFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    qCDebug(dcZigbeeController()) << reply->additionalMessage();

    quint8 sequenceNumber = static_cast<quint8>(reply->additionalMessage().data().at(0));
    quint8 status = static_cast<quint8>(reply->additionalMessage().data().at(1));

    quint16 nwkAddress = static_cast<quint16>(reply->additionalMessage().data().at(2));
    nwkAddress <<= 8;
    nwkAddress |= reply->additionalMessage().data().at(3);

    quint8 length = static_cast<quint8>(reply->additionalMessage().data().at(4));

    if (length == 0) {
        qCWarning(dcZigbeeNetwork()) << "Length 0";
        return;
    }

    quint8 endPoint = static_cast<quint8>(reply->additionalMessage().data().at(5));

    quint16 profileId = static_cast<quint16>(reply->additionalMessage().data().at(6));
    profileId <<= 8;
    profileId |= reply->additionalMessage().data().at(7);

    quint16 deviceId = static_cast<quint16>(reply->additionalMessage().data().at(8));
    deviceId <<= 8;
    deviceId |= reply->additionalMessage().data().at(9);

    quint8 bitField = static_cast<quint8>(reply->additionalMessage().data().at(10));

    qCDebug(dcZigbeeNetwork()) << "Node simple descriptor:";
    qCDebug(dcZigbeeNetwork()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbeeNetwork()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbeeNetwork()) << "    Nwk address:" << ZigbeeUtils::convertUint16ToHexString(nwkAddress);
    qCDebug(dcZigbeeNetwork()) << "    Lenght:" << ZigbeeUtils::convertByteToHexString(length);
    qCDebug(dcZigbeeNetwork()) << "    End Point:" << ZigbeeUtils::convertByteToHexString(endPoint);
    qCDebug(dcZigbeeNetwork()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));

    if (profileId == Zigbee::ZigbeeProfileLightLink) {
        qCDebug(dcZigbeeNetwork()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::LightLinkDevice>(deviceId);
    } else {
        qCDebug(dcZigbeeNetwork()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::HomeAutomationDevice>(deviceId);
    }

    qCDebug(dcZigbeeNetwork()) << "    Bit field:" << ZigbeeUtils::convertByteToHexString(bitField);

    quint8 inputClusterCount = static_cast<quint8>(reply->additionalMessage().data().at(10));

    qCDebug(dcZigbeeNetwork()) << "    Input clusters:";
    QByteArray inputClusterListData = reply->additionalMessage().data().mid(11, inputClusterCount * 2);
    for (int i = 0; i < inputClusterListData.count(); i+=2) {
        quint16 clusterId = static_cast<quint16>(inputClusterListData.at(i));
        clusterId <<= 8;
        clusterId |= inputClusterListData .at(i+1);

        qCDebug(dcZigbeeNetwork()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
    }

    quint8 outputClusterCount = static_cast<quint8>(reply->additionalMessage().data().at(12 + inputClusterCount * 2));

    qCDebug(dcZigbeeNetwork()) << "    Output clusters:";
    QByteArray outputClusterListData = reply->additionalMessage().data().mid(12 + inputClusterCount * 2, outputClusterCount * 2);
    for (int i = 0; i < outputClusterListData.count(); i+=2) {
        quint16 clusterId = static_cast<quint16>(outputClusterListData.at(i));
        clusterId <<= 8;
        clusterId |= outputClusterListData .at(i+1);

        qCDebug(dcZigbeeNetwork()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
    }

    if (m_startingState == StartingStateReadSimpleDescriptor) setStartingState(StartingStateReadPowerDescriptor);
}

void ZigbeeNetworkManager::onCommandPowerDescriptorRequestFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    qCDebug(dcZigbeeController()) << reply->additionalMessage();

    quint8 sequenceNumber = static_cast<quint8>(reply->additionalMessage().data().at(0));
    quint8 status = static_cast<quint8>(reply->additionalMessage().data().at(1));

    quint16 bitField = static_cast<quint16>(reply->additionalMessage().data().at(2));
    bitField <<= 8;
    bitField |= reply->additionalMessage().data().at(3);

    // Bit 0 - 3 Power mode
    // 0000: Receiver configured according to “Receiver on when idle” MAC flag in the Node Descriptor
    // 0001: Receiver switched on periodically
    // 0010: Receiver switched on when stimulated, e.g. by pressing a button

    if (!ZigbeeUtils::checkBitUint16(bitField, 0) && !ZigbeeUtils::checkBitUint16(bitField, 1)) {
        m_powerMode = PowerModeAlwaysOn;
    } else if (ZigbeeUtils::checkBitUint16(bitField, 0) && !ZigbeeUtils::checkBitUint16(bitField, 1)) {
        m_powerMode = PowerModeOnPeriodically;
    } else if (!ZigbeeUtils::checkBitUint16(bitField, 0) && ZigbeeUtils::checkBitUint16(bitField, 1)) {
        m_powerMode = PowerModeOnWhenStimulated;
    }

    // Bit 4 - 7 Available power sources
    // Bit 0: Permanent mains supply
    // Bit 1: Rechargeable battery
    // Bit 2: Disposable battery
    // Bit 4: Reserved

    if (ZigbeeUtils::checkBitUint16(bitField, 4)) {
        m_availablePowerSources.append(PowerSourcePermanentMainSupply);
    } else if (ZigbeeUtils::checkBitUint16(bitField, 5)) {
        m_availablePowerSources.append(PowerSourceRecharchableBattery);
    } else if (ZigbeeUtils::checkBitUint16(bitField, 6)) {
        m_availablePowerSources.append(PowerSourceDisposableBattery);
    }

    // Bit 8 - 11 Active source: according to the same schema as available power sources
    if (ZigbeeUtils::checkBitUint16(bitField, 8)) {
        m_powerSource = PowerSourcePermanentMainSupply;
    } else if (ZigbeeUtils::checkBitUint16(bitField, 9)) {
        m_powerSource = PowerSourceRecharchableBattery;
    } else if (ZigbeeUtils::checkBitUint16(bitField, 10)) {
        m_powerSource = PowerSourceDisposableBattery;
    }

    // Bit 12 - 15: Battery level if available
    // 0000: Critically low
    // 0100: Approximately 33%
    // 1000: Approximately 66%
    // 1100: Approximately 100% (near fully charged)

    if (!ZigbeeUtils::checkBitUint16(bitField, 14) && !ZigbeeUtils::checkBitUint16(bitField, 15)) {
        m_powerLevel = PowerLevelCriticalLow;
    } else if (ZigbeeUtils::checkBitUint16(bitField, 14) && !ZigbeeUtils::checkBitUint16(bitField, 15)) {
        m_powerLevel = PowerLevelLow;
    } else if (!ZigbeeUtils::checkBitUint16(bitField, 14) && ZigbeeUtils::checkBitUint16(bitField, 15)) {
        m_powerLevel = PowerLevelOk;
    } else if (ZigbeeUtils::checkBitUint16(bitField, 14) && ZigbeeUtils::checkBitUint16(bitField, 15)) {
        m_powerLevel = PowerLevelFull;
    }

    qCDebug(dcZigbeeNetwork()) << "Node power descriptor:";
    qCDebug(dcZigbeeNetwork()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbeeNetwork()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbeeNetwork()) << "    Bitfiled:" << ZigbeeUtils::convertUint16ToHexString(bitField);
    qCDebug(dcZigbeeNetwork()) << "    Power mode:" << m_powerMode;
    qCDebug(dcZigbeeNetwork()) << "    Available power sources:";
    foreach (const PowerSource &source, m_availablePowerSources) {
        qCDebug(dcZigbeeNetwork()) << "        " << source;
    }
    qCDebug(dcZigbeeNetwork()) << "    Power source:" << m_powerSource;
    qCDebug(dcZigbeeNetwork()) << "    Power level:" << m_powerLevel;

    if (m_startingState == StartingStateReadPowerDescriptor) {
        setStartingState(StartingStateNone);
        setState(StateRunning);
    }
}

void ZigbeeNetworkManager::onCommandInitiateTouchLinkFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkManager::onCommandTouchLinkFactoryResetFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkManager::onCommandRequestLinkQualityFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    qCDebug(dcZigbeeController()) << reply->additionalMessage();

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

        foreach (ZigbeeNode *node, nodes()) {
            if (node->extendedAddress() == ZigbeeAddress(ieeeAddress)) {
                node->setShortAddress(shortAddress);
            }
        }

    }
}

void ZigbeeNetworkManager::processLoggingMessage(const ZigbeeInterfaceMessage &message)
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

void ZigbeeNetworkManager::processFactoryNewRestart(const ZigbeeInterfaceMessage &message)
{
    quint8 controllerStatus = static_cast<quint8>(message.data().at(0));
    QString controllerStatusString;
    switch (controllerStatus) {
    case 0:
        controllerStatusString = "startup";
        break;
    case 2:
        controllerStatusString = "NRF start";
        break;
    case 6:
        controllerStatusString = "running";
        break;
    default:
        break;
    }


    qCDebug(dcZigbeeNetwork()) << "Restart finished. Current controller state:" << controllerStatusString;
    if (m_startingState == StartingStateReset) setStartingState(StartingStateGetVersion);
}

void ZigbeeNetworkManager::processNodeClusterList(const ZigbeeInterfaceMessage &message)
{
    quint8 sourceEndpoint = static_cast<quint8>(message.data().at(0));

    quint16 profileId = static_cast<quint8>(message.data().at(1));
    profileId <<= 8;
    profileId |= static_cast<quint8>(message.data().at(2));

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

void ZigbeeNetworkManager::processNodeAttributeList(const ZigbeeInterfaceMessage &message)
{
    quint8 sourceEndpoint = static_cast<quint8>(message.data().at(0));

    quint16 profileId = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(1, 2));
    quint16 clusterId = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(3, 2));

    qCDebug(dcZigbeeController()) << "Node attribute list received:";
    qCDebug(dcZigbeeController()) << "    Souce endpoint:" << sourceEndpoint;
    qCDebug(dcZigbeeController()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));
    qCDebug(dcZigbeeController()) << "    Cluster ID:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));

    QByteArray attributeListData = message.data().right(message.data().count() - 5);

    for (int i = 0; i < attributeListData.count(); i += 2) {
        quint16 attribute = ZigbeeUtils::convertByteArrayToUint16(attributeListData.mid(i, 2));
        qCDebug(dcZigbeeController()) << "        Attribute:" << ZigbeeUtils::convertUint16ToHexString(attribute);
    }
}

void ZigbeeNetworkManager::processNodeCommandIdList(const ZigbeeInterfaceMessage &message)
{
    quint8 sourceEndpoint = static_cast<quint8>(message.data().at(0));

    quint16 profileId = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(1, 2));
    quint16 clusterId = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(3, 2));

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

void ZigbeeNetworkManager::processDeviceAnnounce(const ZigbeeInterfaceMessage &message)
{
    quint16 shortAddress = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(0, 2));
    quint64 ieeeAddress = ZigbeeUtils::convertByteArrayToUint64(message.data().mid(2, 8));

    quint8 macCapability = static_cast<quint8>(message.data().at(10));

    qCDebug(dcZigbeeNetwork()) << "Device announced:";
    qCDebug(dcZigbeeNetwork()) << "    Address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbeeNetwork()) << "    Extended address:" << ZigbeeAddress(ieeeAddress);
    qCDebug(dcZigbeeNetwork()) << "    Mac capabilities:" << ZigbeeUtils::convertByteToHexString(macCapability);

    ZigbeeNode *node = new ZigbeeNode(this);
    node->setShortAddress(shortAddress);
    node->setExtendedAddress(ZigbeeAddress(ieeeAddress));

    // FIXME: check if node already added, and if we have to update it
    // Start node initialization process
    addNode(node);

    //node->init();

    //saveNetwork();
}

void ZigbeeNetworkManager::processAttributeReport(const ZigbeeInterfaceMessage &message)
{
    quint8 sequenceNumber = static_cast<quint8>(message.data().at(0));
    quint16 sourceAddress = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(1, 2));
    quint8 endPoint = static_cast<quint8>(message.data().at(3));
    quint16 clusterId = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(4, 2));
    quint16 attributeId = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(6, 2));
    quint8 attributStatus = static_cast<quint8>(message.data().at(8));
    quint8 attributDataType = static_cast<quint8>(message.data().at(9));

    quint16 attributeSize = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(10, 2));
    QByteArray data = message.data().mid(12);
    Zigbee::DataType dataType = static_cast<Zigbee::DataType>(attributDataType);

    qCDebug(dcZigbeeNetwork()) << "Attribute report:";
    qCDebug(dcZigbeeNetwork()) << "    SQN:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbeeNetwork()) << "    Source address:" << ZigbeeUtils::convertUint16ToHexString(sourceAddress);
    qCDebug(dcZigbeeNetwork()) << "    End point:" << ZigbeeUtils::convertByteToHexString(endPoint);
    qCDebug(dcZigbeeNetwork()) << "    Cluster:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
    qCDebug(dcZigbeeNetwork()) << "    Attribut id:" << ZigbeeUtils::convertUint16ToHexString(attributeId);
    qCDebug(dcZigbeeNetwork()) << "    Attribut status:" << ZigbeeUtils::convertByteToHexString(attributStatus);
    qCDebug(dcZigbeeNetwork()) << "    Attribut data type:" << dataType;
    qCDebug(dcZigbeeNetwork()) << "    Attribut size:" << attributeSize;
    qCDebug(dcZigbeeNetwork()) << "    Data:" << ZigbeeUtils::convertByteArrayToHexString(data);

    switch (dataType) {
    case Zigbee::CharString:
        qCDebug(dcZigbeeNetwork()) << "    Data(converted)" << QString::fromUtf8(data);
        break;
    case Zigbee::Bool:
        qCDebug(dcZigbeeNetwork()) << "    Data(converted)" << QVariant(data).toBool();
        break;
    default:
        break;
    }

    // TODO: find node and set attribute value

}

void ZigbeeNetworkManager::processLeaveIndication(const ZigbeeInterfaceMessage &message)
{
    quint16 shortAddress = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(0, 2));
    quint8 rejoining = static_cast<quint8>(message.data().at(2));

    qCDebug(dcZigbeeNetwork()) << "Node leaving:" << ZigbeeUtils::convertUint16ToHexString(shortAddress) << rejoining;

    // TODO: remove node
}

void ZigbeeNetworkManager::processRestartProvisioned(const ZigbeeInterfaceMessage &message)
{
    if (message.data().isEmpty())
        return;

    quint8 status = static_cast<quint8>(message.data().at(0));
    switch (status) {
    case 0:
        qCDebug(dcZigbeeNetwork()) << "Restart provisioned: start up";
        break;
    case 2:
        qCDebug(dcZigbeeNetwork()) << "Restart provisioned: NFN start";
        break;
    case 6:
        qCDebug(dcZigbeeNetwork()) << "Restart provisioned: running";
        break;
    default:
        qCDebug(dcZigbeeNetwork()) << "Restart provisioned: unknown";
        break;
    }

}

void ZigbeeNetworkManager::startNetwork()
{
    if (m_controller) {
        delete m_controller;
        m_controller = nullptr;
    }

    m_controller = new ZigbeeBridgeController(this);
    connect(m_controller, &ZigbeeBridgeController::messageReceived, this, &ZigbeeNetworkManager::onMessageReceived);
    connect(m_controller, &ZigbeeBridgeController::availableChanged, this, &ZigbeeNetworkManager::onControllerAvailableChanged);

    if (!m_controller->enable(serialPortName(), serialBaudrate())) {
        setState(StateDisconnected);
        setStartingState(StartingStateNone);
        setError(ErrorHardwareUnavailable);
    } else {
        // Reset
        setState(StateStarting);
        setStartingState(StartingStateErase);
    }
}

void ZigbeeNetworkManager::stopNetwork()
{

}

void ZigbeeNetworkManager::onMessageReceived(const ZigbeeInterfaceMessage &message)
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
    case Zigbee::MessageTypeLeaveIndication:
        processLeaveIndication(message);
        break;
    case Zigbee::MessageTypeNetworkJoinedFormed:
        processNetworkFormed(message);
        break;
    case Zigbee::MessageTypeRestartProvisioned:
        processRestartProvisioned(message);
        break;
    default:
        qCDebug(dcZigbeeController()) << "Message received:" << message;
        break;
    }
}

void ZigbeeNetworkManager::onControllerAvailableChanged(bool available)
{
    qCDebug(dcZigbeeNetwork()) << "Hardware controller is" << (available ? "now available" : "not available");

    if (!available) {
        setError(ErrorHardwareUnavailable);
        setState(StateDisconnected);
    } else {
        setError(ErrorNoError);
        setState(StateStarting);
        setStartingState(StartingStateErase);
    }
}


