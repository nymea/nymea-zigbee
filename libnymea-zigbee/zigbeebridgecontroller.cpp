#include "zigbeebridgecontroller.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeBridgeController::ZigbeeBridgeController(QObject *parent) :
    QObject(parent)
{
    m_interface = new ZigbeeInterface(this);
    connect(m_interface, &ZigbeeInterface::availableChanged, this, &ZigbeeBridgeController::availableChanged);
    connect(m_interface, &ZigbeeInterface::messageReceived, this, &ZigbeeBridgeController::onMessageReceived);
}

bool ZigbeeBridgeController::available() const
{
    return m_interface->available();
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandResetController()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeReset, QByteArray()));
    request.setDescription("Reset controller");
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandSoftResetController()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeZllFactoryNew, QByteArray()));
    request.setDescription("Soft reset controller");
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandErasePersistantData()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeErasePersistentData, QByteArray()));
    request.setDescription("Erase persistent data");

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandGetVersion()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeGetVersion, QByteArray()));
    request.setDescription("Get version");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeVersionList);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandSetExtendedPanId(quint64 extendedPanId)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << extendedPanId;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSetExtendetPanId, data));
    request.setDescription("Set extended PAN id " + QString::number(extendedPanId) + " " + ZigbeeUtils::convertUint64ToHexString(extendedPanId));

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandSetChannelMask(quint32 channelMask)
{
    // Note: 10 < value < 27 -> using sinle channel value
    //       0x07fff800 select from all channels 11 - 26
    //       0x2108800 primary zigbee light link channels 11, 15, 20, 25

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << channelMask;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSetChannelMask, data));
    request.setDescription("Set channel mask " + ZigbeeUtils::convertByteArrayToHexString(data));

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandSetNodeType(ZigbeeNode::NodeType nodeType)
{
    quint8 deviceTypeValue = 0;
    if (nodeType == ZigbeeNode::NodeTypeEndDevice) {
        qCWarning(dcZigbeeController()) << "Set the controller as EndDevice is not allowed. Default to coordinator node type.";
        deviceTypeValue = static_cast<quint8>(ZigbeeNode::NodeTypeCoordinator);
    } else {
        deviceTypeValue = static_cast<quint8>(nodeType);
    }

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << deviceTypeValue;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSetDeviceType, data));

    switch (nodeType) {
    case ZigbeeNode::NodeTypeCoordinator:
        request.setDescription("Set device type coordinator");
        break;
    case ZigbeeNode::NodeTypeRouter:
        request.setDescription("Set device type router");
        break;
    default:
        break;
    }

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandStartNetwork()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeStartNetwork, QByteArray()));
    request.setDescription("Start network");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNetworkJoinedFormed);
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandStartScan()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeStartScan, QByteArray()));
    request.setDescription("Start scan");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNetworkJoinedFormed);
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandPermitJoin(quint16 targetAddress, const quint8 advertisingIntervall, bool tcSignificance)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << targetAddress;
    stream << advertisingIntervall;
    stream << static_cast<quint8>(tcSignificance);

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypePermitJoiningRequest, data));
    request.setDescription("Permit joining request on " + ZigbeeUtils::convertUint16ToHexString(targetAddress) + " for " + QString::number(advertisingIntervall) + "[s]");

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandGetPermitJoinStatus()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeGetPermitJoining, QByteArray()));
    request.setDescription("Get permit joining status");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeGetPermitJoiningResponse);
    request.setTimoutIntervall(1000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandRequestLinkQuality(quint16 shortAddress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;
    stream << static_cast<quint8>(0);

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeManagementLqiRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeManagementLqiResponse);
    request.setDescription("Request link quality request for " + ZigbeeUtils::convertUint16ToHexString(shortAddress));
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandEnableWhiteList()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeNetworkWhitelistEnable, QByteArray()));
    request.setDescription("Enable whitelist");

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandInitiateTouchLink()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeInitiateTouchlink, QByteArray()));
    request.setDescription("Initiate touch link");

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandTouchLinkFactoryReset()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeTouchlinkFactoryReset, QByteArray()));
    request.setDescription("Touch link factory reset");

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandNetworkAddressRequest(quint16 targetAddress, quint64 extendedAddress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << targetAddress;
    stream << extendedAddress;
    stream << static_cast<quint8>(1);
    stream << static_cast<quint8>(0);

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeNetworkAdressRequest, data));
    request.setDescription("Network address request on " + ZigbeeUtils::convertUint16ToHexString(targetAddress));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNetworkAdressResponse);
    request.setTimoutIntervall(1000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandSetSecurityStateAndKey(quint8 keyState, quint8 keySequence, quint8 keyType, const QString &key)
{
    // Note: calls ZPS_vAplSecSetInitialSecurityState

    // Key state:
    //      ZPS_ZDO_PRECONFIGURED_LINK_KEY = 3
    //          This key will be used to encrypt the network key. This is the master or manufacturer key

    //      ZPS_ZDO_ZLL_LINK_KEY = 4
    //          This key will be generated by the trust center.

    // Key Type:
    //      ZPS_APS_UNIQUE_LINK_KEY =

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << keyState;
    stream << keySequence;
    stream << keyType;
    stream << QByteArray::fromHex(key.toUtf8());

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSetSecurity, data));
    request.setDescription("Set security configuration");

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandAuthenticateDevice(const ZigbeeAddress &ieeeAddress, const QString &key)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << ieeeAddress.toUInt64();
    stream << QByteArray::fromHex(key.toUtf8());

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeAuthenticateDeviceRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeAuthenticateDeviceResponse);
    request.setDescription(QString("Authenticate device %1").arg(ieeeAddress.toString()));
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandNodeDescriptorRequest(quint16 shortAddress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeNodeDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNodeDescriptorRsponse);
    request.setDescription("Node descriptor request for " + ZigbeeUtils::convertUint16ToHexString(shortAddress));
    request.setTimoutIntervall(10000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandSimpleDescriptorRequest(quint16 shortAddress, quint8 endpoint)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;
    stream << endpoint;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSimpleDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeSimpleDescriptorResponse);
    request.setDescription("Simple node descriptor request for " + ZigbeeUtils::convertUint16ToHexString(shortAddress) + " endpoint " + QString::number(endpoint));
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandPowerDescriptorRequest(quint16 shortAddress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypePowerDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypePowerDescriptorResponse);
    request.setDescription("Node power descriptor request for " + ZigbeeUtils::convertUint16ToHexString(shortAddress));
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeController::commandUserDescriptorRequest(quint16 shortAddress, quint16 address)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;
    stream << address;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeUserDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeUserDescriptorResponse);
    request.setDescription("Node user descriptor request for " + ZigbeeUtils::convertUint16ToHexString(shortAddress) + " " + ZigbeeUtils::convertUint16ToHexString(address));
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

void ZigbeeBridgeController::sendMessage(ZigbeeInterfaceReply *reply)
{
    if (!reply)
        return;

    m_currentReply = reply;
    qCDebug(dcZigbeeController()) << "Sending request:" << reply->request().description();

    m_interface->sendMessage(reply->request().message());
    reply->startTimer(reply->request().timeoutIntervall());
}

void ZigbeeBridgeController::onMessageReceived(const ZigbeeInterfaceMessage &message)
{
    // Check if we have a current reply
    if (m_currentReply) {
        if (message.messageType() == Zigbee::MessageTypeStatus) {
            // We have a status message for the current reply
            m_currentReply->setStatusMessage(message);
            qCDebug(dcZigbeeController()) << "Current request" << m_currentReply->request().description() << "status message received";
            // TODO: check if success, if not, finish reply
        } else if (m_currentReply->request().expectsAdditionalMessage() &&
                   message.messageType() == m_currentReply->request().expectedAdditionalMessageType()) {
            m_currentReply->setAdditionalMessage(message);
            qCDebug(dcZigbeeController()) << "Current request" << m_currentReply->request().description() << "additional message received";
        } else {
            // Not a reply related message
            qCDebug(dcZigbeeController()) << "Current request" << m_currentReply->request().description() << "but not related message received";
            emit messageReceived(message);
            return;
        }

        // Check if request is complete
        if (m_currentReply->isComplete()) {
            qCDebug(dcZigbeeController()) << "Current request" << m_currentReply->request().description() << "is complete!";
            m_currentReply->setFinished();

            // Note: the request class has to take care about the reply object
            m_currentReply = nullptr;

            if (!m_replyQueue.isEmpty())
                sendMessage(m_replyQueue.dequeue());

            return;
        }
    } else {
        // Not a reply message
        emit messageReceived(message);
    }

}

void ZigbeeBridgeController::onReplyTimeout()
{
    m_currentReply->setFinished();
    m_currentReply = nullptr;

    if (!m_replyQueue.isEmpty())
        sendMessage(m_replyQueue.dequeue());

}

bool ZigbeeBridgeController::enable(const QString &serialPort, qint32 baudrate)
{
    return m_interface->enable(serialPort, baudrate);
}

void ZigbeeBridgeController::disable()
{
    m_interface->disable();
}

ZigbeeInterfaceReply *ZigbeeBridgeController::sendRequest(const ZigbeeInterfaceRequest &request)
{
    // Create Reply
    ZigbeeInterfaceReply *reply = new ZigbeeInterfaceReply(request);
    connect(reply, &ZigbeeInterfaceReply::timeout, this, &ZigbeeBridgeController::onReplyTimeout);

    // If reply running, enqueue, else send request
    if (m_currentReply) {
        m_replyQueue.enqueue(reply);
    } else {
        sendMessage(reply);
    }

    return reply;
}
