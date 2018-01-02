#include "zigbeenetworkmanager.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QDateTime>
#include <QDataStream>
#include <QSettings>

ZigbeeNetworkManager::ZigbeeNetworkManager(const int &channel, const QString &serialPort, QObject *parent) :
    ZigbeeNode(new ZigbeeBridgeController(serialPort, parent), parent)
{
    connect(controller(), &ZigbeeBridgeController::messageReceived, this, &ZigbeeNetworkManager::onMessageReceived);

    if (controller()->available()) {
        qCDebug(dcZigbee()) << "Bridge controller started successfully on" << serialPort;
    } else {
        qCCritical(dcZigbee()) << "The zigbee controller is not available on" << serialPort;
        return;
    }

    QSettings settings;
    settings.beginGroup("Network");
    m_extendedPanId = static_cast<quint64>(settings.value("panId", 0).toUInt());
    if (m_extendedPanId == 0) {
        m_extendedPanId = generateRandomPanId();
        settings.setValue("panId", m_extendedPanId);
    }
    settings.endGroup();

    qCDebug(dcZigbee()) << "PAN Id:" << m_extendedPanId << ZigbeeUtils::convertUint64ToHexString(m_extendedPanId);

    // Create channel mask
    // Note: normal number passed, that specific channel will be used || Bitfield: all channels would be 0x07FFF800
    quint32 channelMask = 0;
    if (channel == 0) {
        qCDebug(dcZigbee()) << "Using quitest channel for the zigbee network.";
    } else {
        channelMask |= 1 << (channel);
        qCDebug(dcZigbee()) << "Using channel" << channel << "for the zigbee network.";
    }

//    // Call init methods
//    erasePersistentData();
//    //resetController();
//    getVersion();
//    setExtendedPanId(m_extendedPanId);
//    setChannelMask(channelMask);
//    setDeviceType(NodeTypeCoordinator);
//    startNetwork();


//    //startScan();
//    //getPermitJoiningStatus();
//    enableWhitelist();
//    permitJoining();
//    //getPermitJoiningStatus();

//    //startScan();
//    initiateTouchLink();
}

QString ZigbeeNetworkManager::controllerVersion() const
{
    return m_controllerVersion;
}

QList<ZigbeeNode *> ZigbeeNetworkManager::nodeList() const
{
    return m_nodeList;
}

quint64 ZigbeeNetworkManager::extendedPanId() const
{
    return m_extendedPanId;
}

bool ZigbeeNetworkManager::networkRunning() const
{
    return m_networkRunning;
}

quint64 ZigbeeNetworkManager::generateRandomPanId()
{
    srand(static_cast<int>(QDateTime::currentMSecsSinceEpoch() / 1000));
    srand(qrand());
    return (ULLONG_MAX - 0) * (qrand()/(double)RAND_MAX);
}

void ZigbeeNetworkManager::parseNetworkFormed(const QByteArray &data)
{
    // Parse network status
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


    // Parse short network address
    quint16 shortAddress = data.at(1);
    shortAddress <<= 8;
    shortAddress |= data.at(2);

    // Parse extended network address
    quint64 extendedAddress = data.at(3);
    extendedAddress <<= 8;
    extendedAddress |= data.at(4);
    extendedAddress <<= 8;
    extendedAddress |= data.at(5);
    extendedAddress <<= 8;
    extendedAddress |= data.at(6);
    extendedAddress <<= 8;
    extendedAddress |= data.at(7);
    extendedAddress <<= 8;
    extendedAddress |= data.at(8);
    extendedAddress <<= 8;
    extendedAddress |= data.at(9);
    extendedAddress <<= 8;
    extendedAddress |= data.at(10);

    // Parse network channel
    quint8 channel = static_cast<quint8>(data.at(11));

    qCDebug(dcZigbee()).noquote() << "Network" << networkStatusString;
    qCDebug(dcZigbee()) << "    Address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbee()) << "    Extended address:" << ZigbeeAddress(extendedAddress);
    qCDebug(dcZigbee()) << "    Channel:" << channel;

    // Set the node information
    setShortAddress(shortAddress);
    setExtendedAddress(ZigbeeAddress(extendedAddress));

}

void ZigbeeNetworkManager::loadNetwork()
{
    qCDebug(dcZigbee()) << "Loading network nodes";
    QSettings settings;
    settings.beginGroup("Nodes");
    foreach (const QString nodeAddress, settings.childGroups()) {
        settings.beginGroup(nodeAddress);
        quint16 shortAddress = static_cast<quint16>(settings.value("shortAddress", 0).toUInt());
        settings.endGroup();

        ZigbeeNode *node = new ZigbeeNode(controller(), this);
        node->setExtendedAddress(ZigbeeAddress(nodeAddress));
        node->setShortAddress(shortAddress);
        m_nodeList.append(node);
        //node->init();
    }

}

void ZigbeeNetworkManager::saveNetwork()
{
    qCDebug(dcZigbee()) << "Save network";
    QSettings settings;
    settings.beginGroup("Nodes");
    foreach (ZigbeeNode *node, m_nodeList) {
        settings.beginGroup(node->extendedAddress().toString());
        settings.setValue("shortAddress", node->shortAddress());
        settings.endGroup();
    }
}

void ZigbeeNetworkManager::resetController()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeReset));
    request.setDescription("Reset controller");
    request.setTimoutIntervall(3000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onResetControllerFinished);
}

void ZigbeeNetworkManager::erasePersistentData()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeErasePersistentData));
    request.setDescription("Erase persistent data");

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onErasePersistentDataFinished);
}

void ZigbeeNetworkManager::sendDataManagerAvailableResponse()
{
    ZigbeeInterfaceMessage message;
    message.setMessageType(Zigbee::MessageTypeDataManagerAvailableResponse);
    message.setData(QByteArray::fromRawData("\x00\x00", 2));

    ZigbeeInterfaceRequest request(message);
    request.setDescription("Data manager available response");

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, reply, &ZigbeeInterfaceReply::deleteLater);
}

void ZigbeeNetworkManager::getVersion()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeGetVersion));
    request.setDescription("Get version");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeVersionList);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onGetVersionFinished);
}

void ZigbeeNetworkManager::setExtendedPanId(const quint64 &panId)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << panId;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSetExtendetPanId, data));
    request.setDescription("Set extended PAN ID " + QString::number(panId) + " " + ZigbeeUtils::convertUint64ToHexString(panId));

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onSetExtendedPanIdFinished);
}

void ZigbeeNetworkManager::setChannelMask(const quint32 &channelMask)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << channelMask;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSetChannelMask, data));
    request.setDescription("Set channel mask " + ZigbeeUtils::convertByteArrayToHexString(data));

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onSetChannelMaskFinished);
}

void ZigbeeNetworkManager::setDeviceType(const NodeType &deviceType)
{
    quint8 deviceTypeValue = static_cast<quint8>(deviceType);

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << deviceTypeValue;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSetDeviceType, data));

    switch (deviceType) {
    case NodeTypeCoordinator:
        request.setDescription("Set device type coordinator");
        break;
    case NodeTypeRouter:
        request.setDescription("Set device type router");
        break;
    default:
        break;
    }

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onSetDeviceTypeFinished);
}

void ZigbeeNetworkManager::startNetwork()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeStartNetwork));
    request.setDescription("Start network");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNetworkJoinedFormed);
    request.setTimoutIntervall(12000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onStartNetworkFinished);
}

void ZigbeeNetworkManager::startScan()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeStartScan));
    request.setDescription("Start scan");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNetworkJoinedFormed);
    request.setTimoutIntervall(12000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onStartScanFinished);
}

void ZigbeeNetworkManager::permitJoining(quint16 targetAddress, const quint8 advertisingIntervall)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << targetAddress;
    stream << advertisingIntervall;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypePermitJoiningRequest, data));
    request.setDescription("Permit joining request on " + ZigbeeUtils::convertUint16ToHexString(targetAddress) + " for " + QString::number(advertisingIntervall) + "[s]");

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onPermitJoiningFinished);
}

void ZigbeeNetworkManager::getPermitJoiningStatus()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeGetPermitJoining));
    request.setDescription("Get permit joining status");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeGetPermitJoiningResponse);
    request.setTimoutIntervall(1000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onGetPermitJoiningStatusFinished);
}

void ZigbeeNetworkManager::enableWhitelist()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeNetworkWhitelistEnable));
    request.setDescription("Enable whitelist");

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onEnableWhitelistFinished);
}

void ZigbeeNetworkManager::initiateTouchLink()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeInitiateTouchlink));
    request.setDescription("Initiate touch link");

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onInitiateTouchLinkFinished);
}

void ZigbeeNetworkManager::touchLinkFactoryReset()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeTouchlinkFactoryReset));
    request.setDescription("Touch link factory reset");

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onTouchLinkFactoryResetFinished);
}


void ZigbeeNetworkManager::requestMatchDescriptor(const quint16 &shortAddress, const Zigbee::ZigbeeProfile &profile)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;
    stream << static_cast<quint16>(profile);

    // TODO: check what this does

//    // Input clusters
//    stream << static_cast<quint8>(7);
//    stream << static_cast<quint16>(Zigbee::ClusterIdBasic);
//    stream << static_cast<quint16>(Zigbee::ClusterIdIdentify);
//    stream << static_cast<quint16>(Zigbee::ClusterIdGroups);
//    stream << static_cast<quint16>(Zigbee::ClusterIdScenes);
//    stream << static_cast<quint16>(Zigbee::ClusterIdOnOff);
//    stream << static_cast<quint16>(Zigbee::ClusterIdLevelControl);
//    stream << static_cast<quint16>(Zigbee::ClusterIdColorControl);

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeMatchDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeMatchDescriptorResponse);
    request.setDescription("Request match descriptors " + ZigbeeUtils::convertUint16ToHexString(shortAddress));
    request.setTimoutIntervall(5000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onRequestMatchDescriptorFinished);
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

    parseNetworkFormed(reply->additionalMessage().data());

    init();

    loadNetwork();
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

    parseNetworkFormed(reply->additionalMessage().data());

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

void ZigbeeNetworkManager::onRequestMatchDescriptorFinished()
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

void ZigbeeNetworkManager::onEnableWhitelistFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkManager::onInitiateTouchLinkFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkManager::onTouchLinkFactoryResetFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
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

    qCDebug(dcZigbeeController()) << "Node cluster list received:";
    qCDebug(dcZigbeeController()) << "    Souce endpoint:" << sourceEndpoint;
    qCDebug(dcZigbeeController()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));


    QByteArray clusterListData = message.data().right(message.data().count() - 3);

    for (int i = 0; i < clusterListData.count(); i+=2) {
        quint16 clusterId = clusterListData.at(i);
        clusterId <<= 8;
        clusterId |= clusterListData .at(i+1);

        qCDebug(dcZigbeeController()) << "        Cluster ID:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
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


    qCDebug(dcZigbeeController()) << "Node attribute list received:";
    qCDebug(dcZigbeeController()) << "    Souce endpoint:" << sourceEndpoint;
    qCDebug(dcZigbeeController()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));
    qCDebug(dcZigbeeController()) << "    Cluster ID:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));

    QByteArray attributeListData = message.data().right(message.data().count() - 5);

    for (int i = 0; i < attributeListData.count(); i+=2) {
        quint16 attribute = attributeListData.at(i);
        attribute <<= 8;
        attribute |= attributeListData .at(i+1);

        qCDebug(dcZigbeeController()) << "        Attribute:" << ZigbeeUtils::convertUint16ToHexString(attribute);
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

    qCDebug(dcZigbeeController()) << "Node command list received:";
    qCDebug(dcZigbeeController()) << "    Souce endpoint:" << sourceEndpoint;
    qCDebug(dcZigbeeController()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));
    qCDebug(dcZigbeeController()) << "    Cluster ID:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));

    QByteArray commandListData = message.data().right(message.data().count() - 5);

    for (int i = 0; i < commandListData.count(); i++) {
        quint8 attribute = commandListData.at(i);
        qCDebug(dcZigbeeController()) << "        Command:" << ZigbeeUtils::convertByteToHexString(attribute);
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
    qCDebug(dcZigbee()) << "    Address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbee()) << "    Extended address:" << ZigbeeAddress(ieeeAddress);
    qCDebug(dcZigbee()) << "    Mac capabilities:" << ZigbeeUtils::convertByteToHexString(macCapability);

    ZigbeeNode *node = new ZigbeeNode(controller(), this);
    node->setShortAddress(shortAddress);
    node->setExtendedAddress(ZigbeeAddress(ieeeAddress));

    m_nodeList.append(node);

    node->init();

    saveNetwork();
}

void ZigbeeNetworkManager::processAttributeReport(const ZigbeeInterfaceMessage &message)
{
    quint8 sequenceNumber = static_cast<quint8>(message.data().at(0));

    quint16 sourceAddress = message.data().at(1);
    sourceAddress <<= 8;
    sourceAddress |= message.data().at(2);

    quint8 endPoint = static_cast<quint8>(message.data().at(3));

    quint16 clusterId = message.data().at(4);
    clusterId <<= 8;
    clusterId |= message.data().at(5);

    quint16 attributeId = message.data().at(6);
    attributeId <<= 8;
    attributeId |= message.data().at(7);

    quint8 attributStatus = static_cast<quint8>(message.data().at(8));
    quint8 attributDataType = static_cast<quint8>(message.data().at(9));

    quint16 attributeSize = message.data().at(10);
    attributeSize <<= 8;
    attributeSize |= message.data().at(11);

    QByteArray data = message.data().mid(12);
    Zigbee::DataType dataType = static_cast<Zigbee::DataType>(attributDataType);

    qCDebug(dcZigbee()) << "Attribute report:";
    qCDebug(dcZigbee()) << "    SQN:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbee()) << "    Source address:" << ZigbeeUtils::convertUint16ToHexString(sourceAddress);
    qCDebug(dcZigbee()) << "    End point:" << ZigbeeUtils::convertByteToHexString(endPoint);
    qCDebug(dcZigbee()) << "    Cluster:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
    qCDebug(dcZigbee()) << "    Attribut id:" << ZigbeeUtils::convertUint16ToHexString(attributeId);
    qCDebug(dcZigbee()) << "    Attribut status:" << ZigbeeUtils::convertByteToHexString(attributStatus);
    qCDebug(dcZigbee()) << "    Attribut data type:" << dataType;
    qCDebug(dcZigbee()) << "    Attribut size:" << attributeSize;
    qCDebug(dcZigbee()) << "    Data:" << ZigbeeUtils::convertByteArrayToHexString(data);

    switch (dataType) {
    case Zigbee::CharString:
        qCDebug(dcZigbee()) << "    Data(converted)" << QString::fromUtf8(data);
        break;
    case Zigbee::Bool:
        qCDebug(dcZigbee()) << "    Data(converted)" << QVariant(data.toInt()).toBool();
        break;
    default:
        break;
    }

    // TODO: find node and set attribute value

}

void ZigbeeNetworkManager::processLeaveIndication(const ZigbeeInterfaceMessage &message)
{
    quint16 shortAddress = message.data().at(0);
    shortAddress <<= 8;
    shortAddress |= message.data().at(1);

    quint8 rejoining = message.data().at(2);

    qCDebug(dcZigbee()) << "Node leaving:" << ZigbeeUtils::convertUint16ToHexString(shortAddress) << rejoining;

    // TODO: remove node
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
    default:
        qCDebug(dcZigbeeController()) << "Message received:" << message;
        break;
    }
}


