#include "zigbeenetworkmanager.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QDateTime>
#include <QDataStream>
#include <QSettings>

ZigbeeNetworkManager::ZigbeeNetworkManager(QObject *parent) :
    ZigbeeNetwork(ZigbeeNetwork::ControlerTypeNxp, parent)
{

}

QString ZigbeeNetworkManager::controllerFirmwareVersion() const
{
    return m_controllerFirmwareVersion;
}

ZigbeeBridgeController *ZigbeeNetworkManager::controller() const
{
    return m_controller;
}

bool ZigbeeNetworkManager::networkRunning() const
{
    return state() == ZigbeeNetwork::StateRunning;
}

bool ZigbeeNetworkManager::permitJoining() const
{
    return m_permitJoining;
}

void ZigbeeNetworkManager::setPermitJoining(bool permitJoining)
{
    if (m_permitJoining == permitJoining)
        return;

    ZigbeeInterfaceReply *reply = m_controller->commandPermitJoin(0, (permitJoining ? 255 : 0));
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandPermitJoiningFinished);
}

void ZigbeeNetworkManager::setStartingState(ZigbeeNetworkManager::StartingState state)
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
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandErasePersistentDataFinished);
        break;
    }
    case StartingStateReset: {
        m_networkRunning = false;
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Reset controller";
        ZigbeeInterfaceReply *reply = m_controller->commandResetController();
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandResetControllerFinished);
        break;
    }
    case StartingStateGetVersion: {
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Get controller version";
        ZigbeeInterfaceReply *reply = m_controller->commandGetVersion();
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandGetVersionFinished);
        break;
    }
    case StartingStateSetPanId: {
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Set PAN ID";
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
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Set channel mask";
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
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Set security configuration";
        ZigbeeInterfaceReply *reply = m_controller->commandSetSecurityStateAndKey(4, 0, 1, "5A6967426565416C6C69616E63653039");
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandSetSecurityFinished);
        break;
    }
    case StartingStateSetNodeType: {
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Set node type";
        ZigbeeInterfaceReply *reply = m_controller->commandSetNodeType(ZigbeeNode::NodeTypeCoordinator);
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandSetNodeTypeFinished);
        break;
    }
    case StartingStateStartNetwork: {
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Starting network";
        ZigbeeInterfaceReply *reply = m_controller->commandStartNetwork();
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandStartNetworkFinished);
        break;
    }
    case StartingStateGetPermitJoinStatus: {
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Get permit join status";
        ZigbeeInterfaceReply *reply = m_controller->commandGetPermitJoinStatus();
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandGetPermitJoiningStatusFinished);
        break;
    }
    case StartingStateReadeNodeDescriptor: {
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Read coordinator node descriptor";
        ZigbeeInterfaceReply *reply = m_controller->commandNodeDescriptorRequest(shortAddress());
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandNodeDescriptorRequestFinished);
        break;
    }
    case StartingStateReadSimpleDescriptor: {
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Read coordinator simple descriptor";
        ZigbeeInterfaceReply *reply = m_controller->commandSimpleDescriptorRequest(shortAddress(), endPoint());
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandSimpleDescriptorRequestFinished);
        break;
    }
    case StartingStateReadPowerDescriptor: {
        qCDebug(dcZigbeeNetwork()) << "Starting state changed: Read coordinator power descriptor";
        ZigbeeInterfaceReply *reply = m_controller->commandPowerDescriptorRequest(shortAddress());
        connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandPowerDescriptorRequestFinished);
        break;
    }
    }
}

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
    //if (m_startingState == StartingStateReset && !m_networkRunning) setStartingState(StartingStateGetVersion);
}

void ZigbeeNetworkManager::onCommandSoftResetControllerFinished()
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

    m_factoryResetting = false;

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    if (m_startingState == StartingStateErase) {
        setStartingState(StartingStateReset);
        //        setState(StateStarting);
    }
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

    m_controllerFirmwareVersion = QString("%1.%2").arg(majorVersion).arg(minorVersion);
    qCDebug(dcZigbeeNetwork()) << "Controller version:" << m_controllerFirmwareVersion;

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

void ZigbeeNetworkManager::onCommandSetNodeTypeFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    if (m_startingState == StartingStateSetNodeType) setStartingState(StartingStateStartNetwork);
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
    if (m_startingState == StartingStateStartNetwork) setStartingState(StartingStateGetPermitJoinStatus);
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

    m_permitJoining = static_cast<bool>(reply->additionalMessage().data().at(0));
    emit permitJoiningChanged(m_permitJoining);

    if (m_startingState == StartingStateGetPermitJoinStatus) setStartingState(StartingStateReadeNodeDescriptor);
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

    // Read the permit joining status back in order to update the state
    ZigbeeInterfaceReply *getJoiningReply = m_controller->commandGetPermitJoinStatus();
    connect(getJoiningReply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandGetPermitJoiningStatusFinished);
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
    if (m_startingState == StartingStateSetSecurity) setStartingState(StartingStateSetNodeType);
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

    bool success = false;

    if (networkStatus == 0) {
        networkStatusString = "joined";
        success = true;
    } else if (networkStatus == 1) {
        networkStatusString = "created";
        success = true;
    } else if (networkStatus >= 128 && networkStatus <= 244) {
        networkStatusString = "failed: Zigbee event code: " + QString::number(networkStatus);
    } else {
        networkStatusString = "unknown";
    }

    if (!success) {
        qCWarning(dcZigbeeNetwork()) << "Forming network failed" << networkStatusString;
        setError(ErrorZigbeeError);
        setStartingState(StartingStateNone);
        setState(StateDisconnected);
        m_networkRunning = false;
        return;
    }

    quint16 shortAddress = ZigbeeUtils::convertByteArrayToUint16(data.mid(1, 2));
    quint64 extendedAddress = ZigbeeUtils::convertByteArrayToUint64(data.mid(3, 8));

    // Parse network channel
    quint8 channel = static_cast<quint8>(data.at(11));

    qCDebug(dcZigbeeNetwork()).noquote() << "Network" << networkStatusString;
    qCDebug(dcZigbeeNetwork()) << "    Address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbeeNetwork()) << "    Extended address:" << ZigbeeAddress(extendedAddress);
    qCDebug(dcZigbeeNetwork()) << "    Channel:" << channel;
    qCDebug(dcZigbeeNetwork()) << "    Extended PAN ID:" << extendedPanId();
    qCDebug(dcZigbeeNetwork()) << "    Permit joining:" << permitJoining();

    m_networkRunning = true;

    // Set the node information
    setShortAddress(shortAddress);
    setExtendedAddress(ZigbeeAddress(extendedAddress));
    setChannel(channel);

    addUnitializedNode(this);
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
    qCDebug(dcZigbeeController()) << reply->additionalMessage();


    QByteArray data = reply->additionalMessage().data();

    quint8 sequenceNumber = 0;
    quint8 status = 0;
    quint16 shortAddress = 0;
    quint16 manufacturerCode = 0;
    quint16 maximalRxSize = 0;
    quint16 maximalTxSize = 0;
    quint16 serverMask = 0;
    quint8 descriptorFlag = 0;
    quint8 macFlags = 0;
    quint8 maxBufferSize = 0;
    quint16 bitField = 0;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> sequenceNumber;
    stream >> status;
    stream >> shortAddress;
    stream >> manufacturerCode;
    stream >> maximalRxSize;
    stream >> maximalTxSize;
    stream >> serverMask;
    stream >> descriptorFlag;
    stream >> macFlags;
    stream >> maxBufferSize;
    stream >> bitField;

    // Get node object
    ZigbeeNode *node = getZigbeeNode(shortAddress);
    if (!node) {
        qCWarning(dcZigbeeNetwork()) << "Could not find node for address" << shortAddress << ZigbeeUtils::convertUint16ToHexString(shortAddress);
        return;
    }

    // Set node data
    node->setManufacturerCode(manufacturerCode);
    node->setMaximumRxSize(maximalRxSize);
    node->setMaximumTxSize(maximalTxSize);
    node->setMaximumBufferSize(maxBufferSize);
    node->setServerMask(serverMask);
    node->setMacCapabilitiesFlag(macFlags);
    node->setDescriptorFlag(descriptorFlag);

    // Parse bit field
    // 0-2 Bit = logical type, 0 = coordinator, 1 = router, 2 = end device
    if (!ZigbeeUtils::checkBitUint16(bitField, 0) && !ZigbeeUtils::checkBitUint16(bitField, 1)) {
        node->setNodeType(NodeTypeCoordinator);
    } else if (!ZigbeeUtils::checkBitUint16(bitField, 0) && ZigbeeUtils::checkBitUint16(bitField, 1)) {
        node->setNodeType(NodeTypeRouter);
    } else if (ZigbeeUtils::checkBitUint16(bitField, 0) && !ZigbeeUtils::checkBitUint16(bitField, 1)) {
        node->setNodeType(NodeTypeEndDevice);
    }

    node->setComplexDescriptorAvailable((bitField >> 3) & 0x0001);
    node->setUserDescriptorAvailable((bitField >> 4) & 0x0001);

    qCDebug(dcZigbeeNetwork()) << "Node descriptor:";
    qCDebug(dcZigbeeNetwork()) << "    Node type:" << node->nodeType();
    qCDebug(dcZigbeeNetwork()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbeeNetwork()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbeeNetwork()) << "    Short address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbeeNetwork()) << "    Manufacturer code:" << ZigbeeUtils::convertUint16ToHexString(manufacturerCode);
    qCDebug(dcZigbeeNetwork()) << "    Maximum Rx size:" << ZigbeeUtils::convertUint16ToHexString(node->maximumRxSize());
    qCDebug(dcZigbeeNetwork()) << "    Maximum Tx size:" << ZigbeeUtils::convertUint16ToHexString(node->maximumTxSize());
    qCDebug(dcZigbeeNetwork()) << "    Maximum buffer size:" << ZigbeeUtils::convertByteToHexString(node->maximumBufferSize());
    qCDebug(dcZigbeeNetwork()) << "    Server mask:" << ZigbeeUtils::convertUint16ToHexString(serverMask);
    qCDebug(dcZigbeeNetwork()) << "        Primary Trust center:" << node->isPrimaryTrustCenter();
    qCDebug(dcZigbeeNetwork()) << "        Backup Trust center:" << node->isBackupTrustCenter();
    qCDebug(dcZigbeeNetwork()) << "        Primary Binding cache:" << node->isPrimaryBindingCache();
    qCDebug(dcZigbeeNetwork()) << "        Backup Binding cache:" << node->isBackupBindingCache();
    qCDebug(dcZigbeeNetwork()) << "        Primary Discovery cache:" << node->isPrimaryDiscoveryCache();
    qCDebug(dcZigbeeNetwork()) << "        Backup Discovery cache:" << node->isBackupDiscoveryCache();
    qCDebug(dcZigbeeNetwork()) << "        Network Manager:" << node->isNetworkManager();
    qCDebug(dcZigbeeNetwork()) << "    Descriptor flag:" << ZigbeeUtils::convertByteToHexString(descriptorFlag);
    qCDebug(dcZigbeeNetwork()) << "        Extended active endpoint list available:" << node->extendedActiveEndpointListAvailable();
    qCDebug(dcZigbeeNetwork()) << "        Extended simple descriptor list available:" << node->extendedSimpleDescriptorListAvailable();
    qCDebug(dcZigbeeNetwork()) << "    MAC flags:" << ZigbeeUtils::convertByteToHexString(macFlags);
    qCDebug(dcZigbeeNetwork()) << "        Alternate PAN coordinator:" << node->alternatePanCoordinator();
    qCDebug(dcZigbeeNetwork()) << "        Device type:" << node->deviceType();
    qCDebug(dcZigbeeNetwork()) << "        Power source flag main power:" << node->powerSourceFlagMainPower();
    qCDebug(dcZigbeeNetwork()) << "        Receiver on when idle:" << node->receiverOnWhenIdle();
    qCDebug(dcZigbeeNetwork()) << "        Security capability:" << node->securityCapability();
    qCDebug(dcZigbeeNetwork()) << "        Allocate address:" << node->allocateAddress();
    qCDebug(dcZigbeeNetwork()) << "    Bit field:" << ZigbeeUtils::convertUint16ToHexString(bitField);
    qCDebug(dcZigbeeNetwork()) << "        Complex desciptor available:" << node->complexDescriptorAvailable();
    qCDebug(dcZigbeeNetwork()) << "        User desciptor available:" << node->userDescriptorAvailable();

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

    QByteArray data = reply->additionalMessage().data();

    quint8 sequenceNumber = 0;
    quint8 status = 0;
    quint16 shortAddress = 0;
    quint8 length = 0;
    quint8 endPoint = 0;
    quint16 profileId = 0;
    quint16 deviceId = 0;
    quint8 bitField = 0;
    quint8 inputClusterCount = 0;
    quint8 outputClusterCount = 0;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> sequenceNumber;
    stream >> status;
    stream >> shortAddress;
    stream >> length;

    if (length == 0) {
        qCWarning(dcZigbeeNetwork()) << "Simple node descriptior has a length of 0.";
        return;
    }

    stream >> endPoint;
    stream >> profileId;
    stream >> deviceId;
    stream >> bitField;

    qCDebug(dcZigbeeNetwork()) << "Node simple descriptor:";
    qCDebug(dcZigbeeNetwork()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbeeNetwork()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbeeNetwork()) << "    Nwk address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbeeNetwork()) << "    Lenght:" << ZigbeeUtils::convertByteToHexString(length);
    qCDebug(dcZigbeeNetwork()) << "    End Point:" << ZigbeeUtils::convertByteToHexString(endPoint);
    qCDebug(dcZigbeeNetwork()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));

    if (profileId == Zigbee::ZigbeeProfileLightLink) {
        qCDebug(dcZigbeeNetwork()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::LightLinkDevice>(deviceId);
    } else {
        qCDebug(dcZigbeeNetwork()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::HomeAutomationDevice>(deviceId);
    }

    qCDebug(dcZigbeeNetwork()) << "    Bit field:" << ZigbeeUtils::convertByteToHexString(bitField);

    stream >> inputClusterCount;
    qCDebug(dcZigbeeNetwork()) << "    Input clusters: (" << inputClusterCount << ")";
    for (int i = 0; i < inputClusterCount; i+=1) {
        quint16 clusterId = 0;
        stream >> clusterId;
        qCDebug(dcZigbeeNetwork()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
    }

    stream >> outputClusterCount;
    qCDebug(dcZigbeeNetwork()) << "    Output clusters: (" << outputClusterCount << ")";
    for (int i = 0; i < outputClusterCount; i+=1) {
        if (stream.atEnd()) {
            qCWarning(dcZigbeeNode()) << "Data stream already at the end but more data expected. Looks like the firmware doesn't provide more data.";
            break;
        }

        quint16 clusterId = 0;
        stream >> clusterId;
        qCDebug(dcZigbeeNetwork()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
    }

    // Get node object
    ZigbeeNode *node = getZigbeeNode(shortAddress);
    if (!node) {
        qCWarning(dcZigbeeNetwork()) << "Could not find node for address" << shortAddress << ZigbeeUtils::convertUint16ToHexString(shortAddress);
        if (m_startingState == StartingStateReadSimpleDescriptor) setStartingState(StartingStateReadPowerDescriptor);
        return;
    }

    // Set node data
    node->setEndPoint(endPoint);
    node->setZigbeeProfile(static_cast<Zigbee::ZigbeeProfile>(profileId));
    node->setDeviceId(deviceId);

    if (m_startingState == StartingStateReadSimpleDescriptor) setStartingState(StartingStateReadPowerDescriptor);
}

void ZigbeeNetworkManager::onCommandPowerDescriptorRequestFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    // Note: get the short address from the request data
    QByteArray requestData = reply->request().message().data();
    quint16 shortAddress;
    QDataStream stream(&requestData, QIODevice::ReadOnly);
    stream >> shortAddress;

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        // Note: the power descriptor is the last request from the initialization
        ZigbeeNode *node = getZigbeeNode(shortAddress);
        if (!node) {
            qCWarning(dcZigbeeNetwork()) << "Could not find node for address" << shortAddress << ZigbeeUtils::convertUint16ToHexString(shortAddress);
            if (m_startingState == StartingStateReadSimpleDescriptor) setStartingState(StartingStateReadPowerDescriptor);
            return;
        }
        if (node->state() != ZigbeeNode::StateInitialized) {
            node->setState(ZigbeeNode::StateInitialized);
        }
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    qCDebug(dcZigbeeController()) << reply->additionalMessage();

    quint8 sequenceNumber = static_cast<quint8>(reply->additionalMessage().data().at(0));
    quint8 status = static_cast<quint8>(reply->additionalMessage().data().at(1));

    quint16 bitField = static_cast<quint16>(reply->additionalMessage().data().at(2));
    bitField <<= 8;
    bitField |= static_cast<quint8>(reply->additionalMessage().data().at(3));

    // Bit 0 - 3 Power mode
    // 0000: Receiver configured according to “Receiver on when idle” MAC flag in the Node Descriptor
    // 0001: Receiver switched on periodically
    // 0010: Receiver switched on when stimulated, e.g. by pressing a button

    ZigbeeNode::PowerMode powerMode = PowerModeAlwaysOn;
    if (!ZigbeeUtils::checkBitUint16(bitField, 0) && !ZigbeeUtils::checkBitUint16(bitField, 1)) {
        powerMode = PowerModeAlwaysOn;
    } else if (ZigbeeUtils::checkBitUint16(bitField, 0) && !ZigbeeUtils::checkBitUint16(bitField, 1)) {
        powerMode = PowerModeOnPeriodically;
    } else if (!ZigbeeUtils::checkBitUint16(bitField, 0) && ZigbeeUtils::checkBitUint16(bitField, 1)) {
        powerMode = PowerModeOnWhenStimulated;
    }

    // Bit 4 - 7 Available power sources
    // Bit 0: Permanent mains supply
    // Bit 1: Rechargeable battery
    // Bit 2: Disposable battery
    // Bit 4: Reserved

    QList<ZigbeeNode::PowerSource> availablePowerSources;
    if (ZigbeeUtils::checkBitUint16(bitField, 4)) {
        availablePowerSources.append(PowerSourcePermanentMainSupply);
    } else if (ZigbeeUtils::checkBitUint16(bitField, 5)) {
        availablePowerSources.append(PowerSourceRecharchableBattery);
    } else if (ZigbeeUtils::checkBitUint16(bitField, 6)) {
        availablePowerSources.append(PowerSourceDisposableBattery);
    }

    // Bit 8 - 11 Active source: according to the same schema as available power sources
    ZigbeeNode::PowerSource powerSource = PowerSourcePermanentMainSupply;
    if (ZigbeeUtils::checkBitUint16(bitField, 8)) {
        powerSource = PowerSourcePermanentMainSupply;
    } else if (ZigbeeUtils::checkBitUint16(bitField, 9)) {
        powerSource = PowerSourceRecharchableBattery;
    } else if (ZigbeeUtils::checkBitUint16(bitField, 10)) {
        powerSource = PowerSourceDisposableBattery;
    }

    // Bit 12 - 15: Battery level if available
    // 0000: Critically low
    // 0100: Approximately 33%
    // 1000: Approximately 66%
    // 1100: Approximately 100% (near fully charged)
    ZigbeeNode::PowerLevel powerLevel = PowerLevelCriticalLow;
    if (!ZigbeeUtils::checkBitUint16(bitField, 14) && !ZigbeeUtils::checkBitUint16(bitField, 15)) {
        powerLevel = PowerLevelCriticalLow;
    } else if (ZigbeeUtils::checkBitUint16(bitField, 14) && !ZigbeeUtils::checkBitUint16(bitField, 15)) {
        powerLevel = PowerLevelLow;
    } else if (!ZigbeeUtils::checkBitUint16(bitField, 14) && ZigbeeUtils::checkBitUint16(bitField, 15)) {
        powerLevel = PowerLevelOk;
    } else if (ZigbeeUtils::checkBitUint16(bitField, 14) && ZigbeeUtils::checkBitUint16(bitField, 15)) {
        powerLevel = PowerLevelFull;
    }

    qCDebug(dcZigbeeNetwork()) << "Node power descriptor:";
    qCDebug(dcZigbeeNetwork()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbeeNetwork()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbeeNetwork()) << "    Bitfiled:" << ZigbeeUtils::convertUint16ToHexString(bitField);
    qCDebug(dcZigbeeNetwork()) << "    Power mode:" << m_powerMode;
    qCDebug(dcZigbeeNetwork()) << "    Available power sources:";
    foreach (const PowerSource &source, availablePowerSources) {
        qCDebug(dcZigbeeNetwork()) << "        " << source;
    }
    qCDebug(dcZigbeeNetwork()) << "    Power source:" << powerSource;
    qCDebug(dcZigbeeNetwork()) << "    Power level:" << powerLevel;

    // Get node object
    ZigbeeNode *node = getZigbeeNode(shortAddress);
    if (!node) {
        qCWarning(dcZigbeeNetwork()) << "Could not find node for address" << shortAddress << ZigbeeUtils::convertUint16ToHexString(shortAddress);
        if (m_startingState == StartingStateReadSimpleDescriptor) setStartingState(StartingStateReadPowerDescriptor);
        return;
    }

    // Set node data
    node->setPowerMode(powerMode);
    node->setPowerSource(powerSource);
    node->setAvailablePowerSources(availablePowerSources);
    node->setPowerLevel(powerLevel);

    // Note: the power descriptor is the last request from the initialization
    if (node->state() != ZigbeeNode::StateInitialized) {
        node->setState(ZigbeeNode::StateInitialized);
    }

    // Note: the power descriptor request for the coordinator is the last step from the network init process
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

    quint8 macCapabilitiesFlag = static_cast<quint8>(message.data().at(10));

    qCDebug(dcZigbeeNetwork()) << "Device announced:";
    qCDebug(dcZigbeeNetwork()) << "    Address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbeeNetwork()) << "    Extended address:" << ZigbeeAddress(ieeeAddress);
    qCDebug(dcZigbeeNetwork()) << "    Mac capabilities:" << ZigbeeUtils::convertByteToHexString(macCapabilitiesFlag);

    ZigbeeNode *node = createNode();
    node->setShortAddress(shortAddress);
    node->setExtendedAddress(ZigbeeAddress(ieeeAddress));
    node->setMacCapabilitiesFlag(macCapabilitiesFlag);

    qCDebug(dcZigbeeNetwork()) << "    Node:" << node;

    // FIXME: check if node already added, and if we have to update it
    addUnitializedNode(node);
    node->setState(StateInitializing);

    ZigbeeInterfaceReply *reply = nullptr;
    reply = m_controller->commandAuthenticateDevice(node->extendedAddress(), securityConfiguration().globalTrustCenterLinkKey());
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandAuthenticateDeviceFinished);

    reply = m_controller->commandNodeDescriptorRequest(node->shortAddress());
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandNodeDescriptorRequestFinished);

    reply = m_controller->commandSimpleDescriptorRequest(node->shortAddress(), endPoint());
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandSimpleDescriptorRequestFinished);

    reply = m_controller->commandPowerDescriptorRequest(node->shortAddress());
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onCommandPowerDescriptorRequestFinished);
}

void ZigbeeNetworkManager::processAttributeReport(const ZigbeeInterfaceMessage &message)
{
    quint8 sequenceNumber = static_cast<quint8>(message.data().at(0));
    quint16 sourceAddress = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(1, 2));
    quint8 endPoint = static_cast<quint8>(message.data().at(3));
    quint16 clusterId = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(4, 2));
    quint16 attributeId = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(6, 2));
    quint8 attributDataType = static_cast<quint8>(message.data().at(8));

    quint16 attributeSize = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(9, 2));
    QByteArray data = message.data().mid(11);
    Zigbee::DataType dataType = static_cast<Zigbee::DataType>(attributDataType);

    qCDebug(dcZigbeeNetwork()) << "Attribute report:";
    qCDebug(dcZigbeeNetwork()) << "    SQN:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbeeNetwork()) << "    Source address:" << ZigbeeUtils::convertUint16ToHexString(sourceAddress);
    qCDebug(dcZigbeeNetwork()) << "    End point:" << ZigbeeUtils::convertByteToHexString(endPoint);
    qCDebug(dcZigbeeNetwork()) << "    Cluster:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
    qCDebug(dcZigbeeNetwork()) << "    Attribut id:" << ZigbeeUtils::convertUint16ToHexString(attributeId);
    qCDebug(dcZigbeeNetwork()) << "    Attribut data type:" << dataType;
    qCDebug(dcZigbeeNetwork()) << "    Attribut size:" << attributeSize;
    qCDebug(dcZigbeeNetwork()) << "    Data:" << ZigbeeUtils::convertByteArrayToHexString(data);

    switch (dataType) {
    case Zigbee::CharString:
        qCDebug(dcZigbeeNetwork()) << "    Data(converted)" << QString::fromUtf8(data);
        break;
    case Zigbee::Bool:
        qCDebug(dcZigbeeNetwork()) << "    Data(converted)" << static_cast<bool>(data.at(0));
        break;
    default:
        break;
    }

    ZigbeeNode *node = getZigbeeNode(sourceAddress);
    if (!node) {
        qCWarning(dcZigbeeNode()) << "Received an attribute report from an unknown node. Ignoring data.";
        return;
    }

    node->setClusterAttribute(static_cast<Zigbee::ClusterId>(clusterId), ZigbeeClusterAttribute(attributeId, dataType, data));
}

void ZigbeeNetworkManager::processLeaveIndication(const ZigbeeInterfaceMessage &message)
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

    if (m_startingState == StartingStateReset) {
        if (m_networkRunning) {
            qCDebug(dcZigbeeNetwork()) << "Reset finished. Network already running. No need to set it up";
            setStartingState(StartingStateGetPermitJoinStatus);
        } else {
            qCDebug(dcZigbeeNetwork()) << "Reset finished. Set up network";
            setStartingState(StartingStateGetVersion);
        }
    }
}

void ZigbeeNetworkManager::startNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Start network...";
    if (m_controller) {
        qCDebug(dcZigbeeNetwork()) << "Clean up old controller...";
        delete m_controller;
        m_controller = nullptr;
    }

    qCDebug(dcZigbeeNetwork()) << "Create new controller...";
    m_controller = new ZigbeeBridgeController(this);
    connect(m_controller, &ZigbeeBridgeController::messageReceived, this, &ZigbeeNetworkManager::onMessageReceived);
    connect(m_controller, &ZigbeeBridgeController::availableChanged, this, &ZigbeeNetworkManager::onControllerAvailableChanged);

    if (!m_controller->enable(serialPortName(), serialBaudrate())) {
        setState(StateDisconnected);
        setStartingState(StartingStateNone);
        setError(ErrorHardwareUnavailable);
    } else {
        // Reset
        setStartingState(StartingStateReset);
        setState(StateStarting);
    }
}

void ZigbeeNetworkManager::stopNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Stopping network...";
    setState(StateStopping);
    if (m_controller) {
        delete m_controller;
        m_controller = nullptr;
    }

    setStartingState(StartingStateNone);
    setState(StateDisconnected);
    setError(ErrorNoError);
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
        qCWarning(dcZigbeeController()) << "Unhandled message received:" << message;
        break;
    }
}

void ZigbeeNetworkManager::onControllerAvailableChanged(bool available)
{
    qCDebug(dcZigbeeNetwork()) << "Hardware controller is" << (available ? "now available" : "not available");

    if (m_factoryResetting && !available) {
        setStartingState(StartingStateReset);
        return;
    }

    if (!available) {
        setError(ErrorHardwareUnavailable);
        setState(StateDisconnected);
        setStartingState(StartingStateNone);
    } else {
        setError(ErrorNoError);
        setState(StateStarting);
        setStartingState(StartingStateReset);
    }
}



void ZigbeeNetworkManager::factoryResetNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Factory reset network and forgett all information. This cannot be undone.";
    clearSettings();

    setState(StateStarting);
    setStartingState(StartingStateErase);
}
