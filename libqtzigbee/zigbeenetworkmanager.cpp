#include "zigbeenetworkmanager.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QDateTime>
#include <QDataStream>
#include <QSettings>

ZigbeeNetworkManager::ZigbeeNetworkManager(const int &channel, const QString &serialPort, qint32 baudrate, QObject *parent) :
    ZigbeeNode(new ZigbeeBridgeController(serialPort, baudrate, parent), parent),
    m_serialPort(serialPort)
{
    connect(controller(), &ZigbeeBridgeController::messageReceived, this, &ZigbeeNetworkManager::onMessageReceived);

    if (controller()->available()) {
        qCDebug(dcZigbee()) << "Bridge controller started successfully on" << serialPort;
    } else {
        qCCritical(dcZigbee()) << "The zigbee controller is not available on" << serialPort;
        return;
    }

    QSettings settings;
    qCDebug(dcZigbee()) << "Loading settings from" << settings.fileName();
    settings.beginGroup("Network");
    m_extendedPanId = static_cast<quint64>(settings.value("panId", 0).toLongLong());
    qCDebug(dcZigbee()) << "Loading saved pan id" << m_extendedPanId;
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

    loadNetwork();
    resetController();
    getVersion();
    init();
}

bool ZigbeeNetworkManager::isAvailable() const
{
    if (m_controller){
        return m_controller->available();
    }

    return false;
}

QString ZigbeeNetworkManager::controllerVersion() const
{
    return m_controllerVersion;
}

QString ZigbeeNetworkManager::serialPort() const
{
    return m_serialPort;
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

    quint16 shortAddress = ZigbeeUtils::convertByteArrayToUint16(data.mid(1, 2));
    quint64 extendedAddress = ZigbeeUtils::convertByteArrayToUint64(data.mid(3, 8));

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
    request.setTimoutIntervall(5000);

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
    request.setDescription("Set extended PAN id " + QString::number(panId) + " " + ZigbeeUtils::convertUint64ToHexString(panId));

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onSetExtendedPanIdFinished);
}

void ZigbeeNetworkManager::setChannelMask(const quint32 &channelMask)
{
    // Note: 10 < value < 27 -> using sinle channel value
    //       0x07fff800 select from all channels 11 - 26
    //       0x2108800 primary zigbee light link channels 11, 15, 20, 25

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

void ZigbeeNetworkManager::permitJoining(quint16 targetAddress, const quint8 advertisingIntervall, bool tcSignificance)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << targetAddress;
    stream << advertisingIntervall;
    stream << static_cast<quint8>(tcSignificance);

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypePermitJoiningRequest, data));
    request.setDescription("Permit joining request on " + ZigbeeUtils::convertUint16ToHexString(targetAddress) + " for " + QString::number(advertisingIntervall) + "[s]");

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onPermitJoiningFinished);
}

void ZigbeeNetworkManager::requestLinkQuality()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress();
    stream << static_cast<quint8>(0);

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeManagementLqiRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeManagementLqiResponse);
    request.setDescription("Request link quality request for " + ZigbeeUtils::convertUint16ToHexString(shortAddress()));
    request.setTimoutIntervall(5000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onRequestLinkQualityFinished);

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

void ZigbeeNetworkManager::networkAddressRequest(quint16 targetAddress, quint64 extendedAddress)
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

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onNetworkAddressRequestFinished);
}


void ZigbeeNetworkManager::requestMatchDescriptor(const quint16 &shortAddress, const Zigbee::ZigbeeProfile &profile)
{

    // TargetAddress profile InputClusterCount InputClusterList OutputClusterCount OutputClusterList

    Q_UNUSED(profile)

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;
    stream << static_cast<quint16>(0xFFFF);
    stream << static_cast<quint8>(0);
    stream << static_cast<quint16>(0);
    stream << static_cast<quint8>(0);
    stream << static_cast<quint16>(0);

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeMatchDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeMatchDescriptorResponse);
    request.setDescription("Request match descriptors " + ZigbeeUtils::convertUint16ToHexString(shortAddress));
    request.setTimoutIntervall(5000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onRequestMatchDescriptorFinished);
}

void ZigbeeNetworkManager::setInitialSecurity(quint8 keyState, quint8 keySequence, quint8 keyType, const QString &key)
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

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onSetSecurityFinished);
}

void ZigbeeNetworkManager::authenticateDevice(const ZigbeeAddress &ieeeAddress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << ieeeAddress.toUInt64();
    stream << QByteArray::fromHex("5A6967426565416C6C69616E63653039");

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeAuthenticateDeviceRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeAuthenticateDeviceResponse);
    request.setDescription(QString("Authenticate device %1").arg(ieeeAddress.toString()));
    request.setTimoutIntervall(5000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNetworkManager::onAuthenticateDeviceFinished);
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

    // Parse version
    quint16 majorVersion = ZigbeeUtils::convertByteArrayToUint16(reply->additionalMessage().data().mid(0, 2));
    quint16 minorVersion = ZigbeeUtils::convertByteArrayToUint16(reply->additionalMessage().data().mid(2, 2));

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

void ZigbeeNetworkManager::onSetSecurityFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNetworkManager::onNetworkAddressRequestFinished()
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

    qCDebug(dcZigbee()) << "Network address response:";
    qCDebug(dcZigbee()) << "    SQN:" << sequenceNumber;
    qCDebug(dcZigbee()) << "    Status:" << statusCode;
    qCDebug(dcZigbee()) << "    Address:" << shortAddress << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbee()) << "    Extended address:" << ZigbeeAddress(ieeeAddress);
    qCDebug(dcZigbee()) << "    Deice count:" << deviceCount;
    qCDebug(dcZigbee()) << "    Start index:" << startIndex;

}

void ZigbeeNetworkManager::onAuthenticateDeviceFinished()
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


    qCDebug(dcZigbee()) << "Authentication response:";
    qCDebug(dcZigbee()) << "    Gateways address:" << ZigbeeAddress(gatewayIeeeAddress);
    qCDebug(dcZigbee()) << "    Key:" << encryptedKey;
    qCDebug(dcZigbee()) << "    MIC:" << mic.toHex();
    qCDebug(dcZigbee()) << "    Initiator address:" << ZigbeeAddress(initiatorIeeeAddress);
    qCDebug(dcZigbee()) << "    Active key sequence number:" << activeKeySequenceNumber;
    qCDebug(dcZigbee()) << "    Channel:" << channel;
    qCDebug(dcZigbee()) << "    Short PAN ID:" << ZigbeeUtils::convertUint16ToHexString(shortPan);
    qCDebug(dcZigbee()) << "    Extended PAN ID:" << extendedPanId << ZigbeeUtils::convertUint64ToHexString(extendedPanId);
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

void ZigbeeNetworkManager::onRequestLinkQualityFinished()
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

    qCDebug(dcZigbee()) << "LQI response:";
    qCDebug(dcZigbee()) << "    SQN:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbee()) << "    Status:" << ZigbeeUtils::convertByteToHexString(statusCode);
    qCDebug(dcZigbee()) << "    Neighbor table entries:" << neighborTableEntries;
    qCDebug(dcZigbee()) << "    Neighbor table list count:" << neighborTableListCount;
    qCDebug(dcZigbee()) << "    Start index:" << startIndex;

    int offset = 5;

    // Note: according to docs, if the table has no neigbors the list will be empty
    if (neighborTableEntries == 0) {
        qCDebug(dcZigbee()) << "    There are no neigbors";
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

        qCDebug(dcZigbee()) << "    Neighbor:" << i;
        qCDebug(dcZigbee()) << "        Address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
        qCDebug(dcZigbee()) << "        PAN id:" << panId;
        qCDebug(dcZigbee()) << "        Extended address:" << ZigbeeAddress(ieeeAddress);
        qCDebug(dcZigbee()) << "        Depth:" << depth;
        qCDebug(dcZigbee()) << "        Link quality:" << linkQuality;
        qCDebug(dcZigbee()) << "        BitMap:" << ZigbeeUtils::convertByteToHexString(bitMap);

        foreach (ZigbeeNode *node, m_nodeList) {
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

    for (int i = 0; i < clusterListData.count(); i += 2) {
        quint16 clusterId = clusterListData.at(i);
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
        quint8 attribute = commandListData.at(i);
        qCDebug(dcZigbeeController()) << "        Command:" << ZigbeeUtils::convertByteToHexString(attribute);
    }
}

void ZigbeeNetworkManager::processDeviceAnnounce(const ZigbeeInterfaceMessage &message)
{
    quint16 shortAddress = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(0, 2));
    quint64 ieeeAddress = ZigbeeUtils::convertByteArrayToUint64(message.data().mid(2, 8));

    quint8 macCapability = static_cast<quint8>(message.data().at(10));

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
    quint16 sourceAddress = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(1, 2));
    quint8 endPoint = static_cast<quint8>(message.data().at(3));
    quint16 clusterId = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(4, 2));
    quint16 attributeId = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(6, 2));
    quint8 attributStatus = static_cast<quint8>(message.data().at(8));
    quint8 attributDataType = static_cast<quint8>(message.data().at(9));

    quint16 attributeSize = ZigbeeUtils::convertByteArrayToUint16(message.data().mid(10, 2));
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
        qCDebug(dcZigbee()) << "    Data(converted)" << QVariant(data).toBool();
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

    qCDebug(dcZigbee()) << "Node leaving:" << ZigbeeUtils::convertUint16ToHexString(shortAddress) << rejoining;

    // TODO: remove node
}

void ZigbeeNetworkManager::processRestartProvisioned(const ZigbeeInterfaceMessage &message)
{
    if (message.data().isEmpty())
        return;

    quint8 status = static_cast<quint8>(message.data().at(0));
    switch (status) {
    case 0:
        qCDebug(dcZigbee()) << "Restart provisioned: start up";
        break;
    case 2:
        qCDebug(dcZigbee()) << "Restart provisioned: NFN start";
        break;
    case 6:
        qCDebug(dcZigbee()) << "Restart provisioned: running";
        break;
    default:
        qCDebug(dcZigbee()) << "Restart provisioned: unknown";
        break;
    }

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
        parseNetworkFormed(message.data());
        break;
    case Zigbee::MessageTypeRestartProvisioned:
        processRestartProvisioned(message);
        break;
    default:
        qCDebug(dcZigbeeController()) << "Message received:" << message;
        break;
    }
}


