#include "zigbeenetworkmanager.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QDateTime>
#include <QDataStream>

ZigbeeNetworkManager::ZigbeeNetworkManager(const QString &serialPort, QObject *parent) :
    ZigbeeNode(parent),
    m_serialPort(serialPort)
{
    // TODO: load PAN id and serial port
    reset();

}

QString ZigbeeNetworkManager::serialPort() const
{
    return m_serialPort;
}

void ZigbeeNetworkManager::setSerialPort(const QString &serialPort)
{
    if (m_serialPort == serialPort)
        return;

    m_serialPort = serialPort;
    reset();
}

QString ZigbeeNetworkManager::controllerVersion() const
{
    return m_controllerVersion;
}

void ZigbeeNetworkManager::reset()
{
    if (m_controller) {
        delete m_controller;
        m_controller = nullptr;
    }

    //m_extendedPanId = generateRandomPanId();
    m_extendedPanId = 1180461015847120384;
    qCDebug(dcZigbee()) << "PAN ID" << m_extendedPanId;

    m_controller = new ZigbeeBridgeController(m_serialPort, this);
    connect(m_controller, &ZigbeeBridgeController::messageReceived, this, &ZigbeeNetworkManager::onMessageReceived);

    if (m_controller->available()) {
        qCDebug(dcZigbee()) << "Bridge controller started successfully on" << m_serialPort;
    } else {
        qCWarning(dcZigbee()) << "The zigbee controller is not available";
    }

    // Call init methods
    erasePersistentData();
    //reset();
    getVersion();
    setExtendedPanId(m_extendedPanId);
    setChannelMask(0);
    setDeviceType(NodeTypeCoordinator);
    startNetwork();
    //startScan();
    getPermitJoiningStatus();
    permitJoining();
    getPermitJoiningStatus();
}

quint64 ZigbeeNetworkManager::generateRandomPanId()
{
    srand(static_cast<int>(QDateTime::currentMSecsSinceEpoch() / 1000));
    srand(qrand());
    return (ULLONG_MAX - 0) * (qrand()/(double)RAND_MAX);
}

void ZigbeeNetworkManager::resetController()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeReset));
    request.setDescription("Reset controller");

    ZigbeeInterfaceReply *reply = m_controller->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onResetControllerFinished);
}

void ZigbeeNetworkManager::erasePersistentData()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeErasePersistentData));
    request.setDescription("Erase persistent data");

    ZigbeeInterfaceReply *reply = m_controller->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onErasePersistentDataFinished);
}

void ZigbeeNetworkManager::sendDataManagerAvailableResponse()
{
    ZigbeeInterfaceMessage message;
    message.setMessageType(Zigbee::MessageTypeDataManagerAvailableResponse);
    message.setData(QByteArray::fromRawData("\x00\x00", 2));

    ZigbeeInterfaceRequest request(message);
    request.setDescription("Data manager available response");

    ZigbeeInterfaceReply *reply = m_controller->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, reply, &ZigbeeInterfaceReply::deleteLater);
}

void ZigbeeNetworkManager::getVersion()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeGetVersion));
    request.setDescription("Get version");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeVersionList);

    ZigbeeInterfaceReply *reply = m_controller->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onGetVersionFinished);
}

void ZigbeeNetworkManager::setExtendedPanId(const quint64 &panId)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << panId;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSetExtendetPanId, data));
    request.setDescription("Set extended PAN ID");

    ZigbeeInterfaceReply *reply = m_controller->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onSetExtendedPanIdFinished);
}

void ZigbeeNetworkManager::setChannelMask(const quint32 &channelMask)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << channelMask;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSetChannelMask, data));
    request.setDescription("Set channel mask");

    ZigbeeInterfaceReply *reply = m_controller->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onSetChannelMaskFinished);
}

void ZigbeeNetworkManager::setDeviceType(const NodeType &deviceType)
{
    quint8 deviceTypeValue = static_cast<quint8>(deviceType);

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << deviceTypeValue;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSetDeviceType, data));
    request.setDescription("Set device type");

    ZigbeeInterfaceReply *reply = m_controller->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onSetDeviceTypeFinished);
}

void ZigbeeNetworkManager::startNetwork()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeStartNetwork));
    request.setDescription("Start network");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNetworkJoinedFormed);
    request.setTimoutIntervall(12000);

    ZigbeeInterfaceReply *reply = m_controller->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onStartNetworkFinished);
}

void ZigbeeNetworkManager::startScan()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeStartScan));
    request.setDescription("Start scan");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNetworkJoinedFormed);
    request.setTimoutIntervall(12000);

    ZigbeeInterfaceReply *reply = m_controller->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onStartScanFinished);
}

void ZigbeeNetworkManager::permitJoining(quint16 targetAddress, const quint8 advertisingIntervall)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << targetAddress;
    stream << advertisingIntervall;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypePermitJoiningRequest, data));
    request.setDescription("Permit joining request");

    ZigbeeInterfaceReply *reply = m_controller->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onPermitJoiningFinished);
}

void ZigbeeNetworkManager::getPermitJoiningStatus()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeGetPermitJoining));
    request.setDescription("Get permit joining status");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeGetPermitJoiningResponse);
    request.setTimoutIntervall(1000);

    ZigbeeInterfaceReply *reply = m_controller->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onGetPermitJoiningStatusFinished);
}

void ZigbeeNetworkManager::requestNodeDescription(const quint16 &shortAddress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeNodeDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNodeDescriptorRsponse);
    request.setDescription("Node descriptor request");
    request.setTimoutIntervall(10000);

    ZigbeeInterfaceReply *reply = m_controller->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onRequestNodeDescriptionFinished);
}

void ZigbeeNetworkManager::requestSimpleNodeDescription(const quint16 &shortAddress, const quint8 &endpoint)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;
    stream << endpoint;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSimpleDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeSimpleDescriptorResponse);
    request.setDescription("Simple node descriptor request");
    request.setTimoutIntervall(10000);

    ZigbeeInterfaceReply *reply = m_controller->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onRequestSimpleNodeDescriptionFinished);
}

void ZigbeeNetworkManager::onResetControllerFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkManager::onErasePersistentDataFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkManager::onGetVersionFinished()
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

    // Parse major version
    quint16 majorVersion = reply->additionalMessage().data().at(0);
    majorVersion <<= 8;
    majorVersion |= reply->additionalMessage().data().at(1);

    // Parse minor version
    quint16 minorVersion = reply->additionalMessage().data().at(2);
    minorVersion <<= 8;
    minorVersion |= reply->additionalMessage().data().at(3);

    m_controllerVersion = QString("%1.%2").arg(majorVersion).arg(minorVersion);
    qCDebug(dcZigbee()) << "Version:" << m_controllerVersion;
}

void ZigbeeNetworkManager::onSetExtendedPanIdFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkManager::onSetChannelMaskFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbee()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkManager::onSetDeviceTypeFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkManager::onStartNetworkFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbee()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    qCDebug(dcZigbeeController()) << reply->additionalMessage();

    // Parse network status
    quint8 networkStatus = static_cast<quint8>(reply->additionalMessage().data().at(0));
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


    // Parse short network address
    quint16 shortAddress = reply->additionalMessage().data().at(1);
    shortAddress <<= 8;
    shortAddress |= reply->additionalMessage().data().at(2);

    // Parse extended network address
    quint64 extendedAddress = reply->additionalMessage().data().at(3);
    extendedAddress <<= 8;
    extendedAddress |= reply->additionalMessage().data().at(4);
    extendedAddress <<= 8;
    extendedAddress |= reply->additionalMessage().data().at(5);
    extendedAddress <<= 8;
    extendedAddress |= reply->additionalMessage().data().at(6);
    extendedAddress <<= 8;
    extendedAddress |= reply->additionalMessage().data().at(7);
    extendedAddress <<= 8;
    extendedAddress |= reply->additionalMessage().data().at(8);
    extendedAddress <<= 8;
    extendedAddress |= reply->additionalMessage().data().at(9);
    extendedAddress <<= 8;
    extendedAddress |= reply->additionalMessage().data().at(10);

    // Parse network channel
    quint8 channel = static_cast<quint8>(reply->additionalMessage().data().at(11));

    qCDebug(dcZigbee()) << "Network" << networkStatusString;
    qCDebug(dcZigbee()) << "       short address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbee()) << "       extended address:" << ZigbeeUtils::convertUint64ToHexString(extendedAddress);
    qCDebug(dcZigbee()) << "       channel:" << channel;

    // Set the node information
    setShortAddress(shortAddress);
    setExtendedAddress(extendedAddress);

    // Request data
    requestNodeDescription(shortAddress);
    requestSimpleNodeDescription(shortAddress);

}

void ZigbeeNetworkManager::onStartScanFinished()
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

void ZigbeeNetworkManager::onGetPermitJoiningStatusFinished()
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

void ZigbeeNetworkManager::onPermitJoiningFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";

}

void ZigbeeNetworkManager::onRequestNodeDescriptionFinished()
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

    quint16 shortAddress = reply->additionalMessage().data().at(2);
    shortAddress <<= 8;
    shortAddress |= reply->additionalMessage().data().at(3);

    quint16 manufacturerCode = reply->additionalMessage().data().at(4);
    manufacturerCode <<= 8;
    manufacturerCode |= reply->additionalMessage().data().at(5);

    quint16 maximalRxSize = reply->additionalMessage().data().at(6);
    maximalRxSize <<= 8;
    maximalRxSize |= reply->additionalMessage().data().at(7);

    quint16 maximalTxSize = reply->additionalMessage().data().at(8);
    maximalTxSize <<= 8;
    maximalTxSize |= reply->additionalMessage().data().at(9);

    quint16 serverMask = reply->additionalMessage().data().at(10);
    serverMask <<= 8;
    serverMask |= reply->additionalMessage().data().at(11);

    quint8 descriptorFlag = static_cast<quint8>(reply->additionalMessage().data().at(12));
    quint8 macFlags = static_cast<quint8>(reply->additionalMessage().data().at(13));
    quint8 maxBufferSize = static_cast<quint8>(reply->additionalMessage().data().at(14));

    quint16 bitField = reply->additionalMessage().data().at(15);
    bitField <<= 8;
    bitField |= reply->additionalMessage().data().at(16);

    // TODO: find note for short address and set data

    qCDebug(dcZigbee()) << "Node descriptor:";
    qCDebug(dcZigbee()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbee()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbee()) << "    Short address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbee()) << "    Manufacturer code:" << ZigbeeUtils::convertUint16ToHexString(manufacturerCode);
    qCDebug(dcZigbee()) << "    Maximum Rx size:" << ZigbeeUtils::convertUint16ToHexString(maximalRxSize);
    qCDebug(dcZigbee()) << "    Maximum Tx size:" << ZigbeeUtils::convertUint16ToHexString(maximalTxSize);
    qCDebug(dcZigbee()) << "    Server makk:" << ZigbeeUtils::convertUint16ToHexString(serverMask);
    qCDebug(dcZigbee()) << "    Descriptor flag:" << ZigbeeUtils::convertByteToHexString(descriptorFlag);
    qCDebug(dcZigbee()) << "    MAC flags:" << ZigbeeUtils::convertByteToHexString(macFlags);
    qCDebug(dcZigbee()) << "    Maximum buffer size:" << ZigbeeUtils::convertByteToHexString(maxBufferSize);
    qCDebug(dcZigbee()) << "    Bit field:" << ZigbeeUtils::convertUint16ToHexString(bitField);

}

void ZigbeeNetworkManager::onRequestSimpleNodeDescriptionFinished()
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

    quint16 nwkAddress = reply->additionalMessage().data().at(2);
    nwkAddress <<= 8;
    nwkAddress |= reply->additionalMessage().data().at(3);

    quint8 length = static_cast<quint8>(reply->additionalMessage().data().at(4));
    quint8 endPoint = static_cast<quint8>(reply->additionalMessage().data().at(5));

    quint16 profileId = reply->additionalMessage().data().at(6);
    profileId <<= 8;
    profileId |= reply->additionalMessage().data().at(7);

    quint16 deviceId = reply->additionalMessage().data().at(8);
    deviceId <<= 8;
    deviceId |= reply->additionalMessage().data().at(9);

    quint8 bitField = static_cast<quint8>(reply->additionalMessage().data().at(10));

    qCDebug(dcZigbee()) << "Node somple descriptor:";
    qCDebug(dcZigbee()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbee()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbee()) << "    Nwk address:" << ZigbeeUtils::convertUint16ToHexString(nwkAddress);
    qCDebug(dcZigbee()) << "    Lenght:" << ZigbeeUtils::convertByteToHexString(length);
    qCDebug(dcZigbee()) << "    End Point:" << ZigbeeUtils::convertByteToHexString(endPoint);
    qCDebug(dcZigbee()) << "    Profile:" << ZigbeeUtils::profileIdToString((Zigbee::ZigbeeProfile)profileId);
    qCDebug(dcZigbee()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId);
    qCDebug(dcZigbee()) << "    Bit field:" << ZigbeeUtils::convertByteToHexString(bitField);

    quint8 inputClusterCount = static_cast<quint8>(reply->additionalMessage().data().at(10));

    qCDebug(dcZigbee()) << "    Input clusters:";
    QByteArray inputClusterListData = reply->additionalMessage().data().mid(11, inputClusterCount * 2);
    for (int i = 0; i < inputClusterListData.count(); i+=2) {
        quint16 clusterId = inputClusterListData.at(i);
        clusterId <<= 8;
        clusterId |= inputClusterListData .at(i+1);

        qCDebug(dcZigbee()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << static_cast<Zigbee::ClusterId>(clusterId);
    }

    quint8 outputClusterCount = static_cast<quint8>(reply->additionalMessage().data().at(12 + inputClusterCount * 2));

    qCDebug(dcZigbee()) << "    Output clusters:";
    QByteArray outputClusterListData = reply->additionalMessage().data().mid(12 + inputClusterCount * 2, outputClusterCount * 2);
    for (int i = 0; i < outputClusterListData.count(); i+=2) {
        quint16 clusterId = outputClusterListData.at(i);
        clusterId <<= 8;
        clusterId |= outputClusterListData .at(i+1);

        qCDebug(dcZigbee()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << static_cast<Zigbee::ClusterId>(clusterId);
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

    qCDebug(dcZigbee()) << "Restart finished. Current controller state:" << controllerStatusString;
}

void ZigbeeNetworkManager::processNodeClusterList(const ZigbeeInterfaceMessage &message)
{
    quint8 sourceEndpoint = static_cast<quint8>(message.data().at(0));

    quint16 profileId = static_cast<quint8>(message.data().at(1));
    profileId <<= 8;
    profileId |= static_cast<quint8>(message.data().at(2));

    qCDebug(dcZigbee()) << "Node cluster list received:";
    qCDebug(dcZigbee()) << "    Souce endpoint:" << sourceEndpoint;
    qCDebug(dcZigbee()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));


    QByteArray clusterListData = message.data().right(message.data().count() - 3);

    for (int i = 0; i < clusterListData.count(); i+=2) {
        quint16 clusterId = clusterListData.at(i);
        clusterId <<= 8;
        clusterId |= clusterListData .at(i+1);

        qCDebug(dcZigbee()) << "        Cluster ID:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId)) << static_cast<Zigbee::ClusterId>(clusterId);
    }
}

void ZigbeeNetworkManager::processNodeAttributeList(const ZigbeeInterfaceMessage &message)
{
    quint8 sourceEndpoint = static_cast<quint8>(message.data().at(0));

    quint16 profileId = static_cast<quint8>(message.data().at(1));
    profileId <<= 8;
    profileId |= static_cast<quint8>(message.data().at(2));

    quint16 clusterId = static_cast<quint8>(message.data().at(3));
    clusterId <<= 8;
    clusterId |= static_cast<quint8>(message.data().at(4));


    qCDebug(dcZigbee()) << "Node attribute list received:";
    qCDebug(dcZigbee()) << "    Souce endpoint:" << sourceEndpoint;
    qCDebug(dcZigbee()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));
    qCDebug(dcZigbee()) << "    Cluster ID:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));

    QByteArray attributeListData = message.data().right(message.data().count() - 5);

    for (int i = 0; i < attributeListData.count(); i+=2) {
        quint16 attribute = attributeListData.at(i);
        attribute <<= 8;
        attribute |= attributeListData .at(i+1);

        qCDebug(dcZigbee()) << "        Attribute:" << ZigbeeUtils::convertUint16ToHexString(attribute);
    }
}

void ZigbeeNetworkManager::processNodeCommandIdList(const ZigbeeInterfaceMessage &message)
{
    quint8 sourceEndpoint = static_cast<quint8>(message.data().at(0));

    quint16 profileId = static_cast<quint8>(message.data().at(1));
    profileId <<= 8;
    profileId |= static_cast<quint8>(message.data().at(2));

    quint16 clusterId = static_cast<quint8>(message.data().at(3));
    clusterId <<= 8;
    clusterId |= static_cast<quint8>(message.data().at(4));

    qCDebug(dcZigbee()) << "Node command list received:";
    qCDebug(dcZigbee()) << "    Souce endpoint:" << sourceEndpoint;
    qCDebug(dcZigbee()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));
    qCDebug(dcZigbee()) << "    Cluster ID:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));

    QByteArray commandListData = message.data().right(message.data().count() - 5);

    for (int i = 0; i < commandListData.count(); i++) {
        quint8 attribute = commandListData.at(i);
        qCDebug(dcZigbee()) << "        Command:" << ZigbeeUtils::convertByteToHexString(attribute);
    }
}

void ZigbeeNetworkManager::processDeviceAnnounce(const ZigbeeInterfaceMessage &message)
{
    quint16 shortAddress = message.data().at(0);
    shortAddress <<= 8;
    shortAddress |= message.data().at(1);

    quint64 ieeeAddress = message.data().at(2);
    ieeeAddress <<= 8;
    ieeeAddress |= message.data().at(3);
    ieeeAddress <<= 8;
    ieeeAddress |= message.data().at(4);
    ieeeAddress <<= 8;
    ieeeAddress |= message.data().at(5);
    ieeeAddress <<= 8;
    ieeeAddress |= message.data().at(6);
    ieeeAddress <<= 8;
    ieeeAddress |= message.data().at(7);
    ieeeAddress <<= 8;
    ieeeAddress |= message.data().at(8);
    ieeeAddress <<= 8;
    ieeeAddress |= message.data().at(9);

    quint8 macCapability = message.data().at(10);

    qCDebug(dcZigbee()) << "Device announced:";
    qCDebug(dcZigbee()) << "    Short address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbee()) << "    Extended address:" << ZigbeeUtils::convertUint64ToHexString(ieeeAddress);
    qCDebug(dcZigbee()) << "    Mac capabilities:" << ZigbeeUtils::convertByteToHexString(macCapability);

    // TODO: parse mac capabilities

    requestNodeDescription(shortAddress);
    requestSimpleNodeDescription(shortAddress);
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
    default:
        qCDebug(dcZigbeeController()) << "Message received:" << message;
        break;
    }
}


