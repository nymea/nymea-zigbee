#include "zigbeenode.h"
#include "zigbeeutils.h"
#include "loggingcategory.h"

#include <QDataStream>

quint16 ZigbeeNode::shortAddress() const
{
    return m_shortAddress;
}

ZigbeeAddress ZigbeeNode::extendedAddress() const
{
    return m_extendedAddress;
}

int ZigbeeNode::endPoint() const
{
    return m_endPoint;
}

ZigbeeNode::NodeType ZigbeeNode::nodeType() const
{
    return m_nodeType;
}

ZigbeeNode::FrequencyBand ZigbeeNode::frequencyBand() const
{
    return m_frequencyBand;
}

ZigbeeNode::Relationship ZigbeeNode::relationShip() const
{
    return m_relationShip;
}

Zigbee::ZigbeeProfile ZigbeeNode::profile() const
{
    return m_profile;
}

quint16 ZigbeeNode::manufacturerCode() const
{
    return m_manufacturerCode;
}

ZigbeeNode::PowerMode ZigbeeNode::powerMode() const
{
    return m_powerMode;
}

ZigbeeNode::PowerSource ZigbeeNode::powerSource() const
{
    return m_powerSource;
}

QList<ZigbeeNode::PowerSource> ZigbeeNode::availablePowerSources() const
{
    return m_availablePowerSources;
}

ZigbeeNode::PowerLevel ZigbeeNode::powerLevel() const
{
    return m_powerLevel;
}

void ZigbeeNode::init()
{
    requestNodeDescription();
    requestSimpleNodeDescription();
    requestPowerDescriptor();
}


void ZigbeeNode::identify()
{
//    QByteArray data;
//    QDataStream stream(&data, QIODevice::WriteOnly);
//    stream << m_shortAddress;
//    stream << static_cast<quint8>(0);

//    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeManagementLqiRequest, data));
//    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeManagementLqiResponse);
//    request.setDescription("Node link quality request for " + ZigbeeUtils::convertUint16ToHexString(m_shortAddress));
//    request.setTimoutIntervall(10000);

//    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    //    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNode::onRequestLinkQuality);
}

void ZigbeeNode::toggle(int addressMode)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << static_cast<quint8>(addressMode); // adress mode
    stream << m_shortAddress;
    stream << static_cast<quint8>(1); // source endpoint
    stream << static_cast<quint8>(1); // destination endpoint
    stream << static_cast<quint8>(2); // command toggle

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeCluserOnOff, data));
    request.setDescription("Toggle request for " + ZigbeeUtils::convertUint16ToHexString(m_shortAddress));

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNode::onToggleFinished);
}

void ZigbeeNode::requestNodeDescription()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_shortAddress;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeNodeDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNodeDescriptorRsponse);
    request.setDescription("Node descriptor request for " + ZigbeeUtils::convertUint16ToHexString(m_shortAddress));
    request.setTimoutIntervall(5000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNode::onRequestNodeDescriptionFinished);
}

void ZigbeeNode::requestSimpleNodeDescription()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_shortAddress;
    stream << quint8(1);

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSimpleDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeSimpleDescriptorResponse);
    request.setDescription("Simple node descriptor request for " + ZigbeeUtils::convertUint16ToHexString(m_shortAddress));
    request.setTimoutIntervall(5000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNode::onRequestSimpleNodeDescriptionFinished);
}

void ZigbeeNode::requestPowerDescriptor()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_shortAddress;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypePowerDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypePowerDescriptorResponse);
    request.setDescription("Node power descriptor request for " + ZigbeeUtils::convertUint16ToHexString(m_shortAddress));
    request.setTimoutIntervall(5000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNode::onRequestPowerDescriptorFinished);
}

void ZigbeeNode::requestUserDescriptor()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_shortAddress;
    stream << static_cast<quint16>(0);

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeUserDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeUserDescriptorResponse);
    request.setDescription("Node user descriptor request for " + ZigbeeUtils::convertUint16ToHexString(m_shortAddress));
    request.setTimoutIntervall(5000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNode::onRequestUserDescriptorFinished);
}

ZigbeeNode::ZigbeeNode(ZigbeeBridgeController *controller, QObject *parent) :
    QObject(parent),
    m_controller(controller)
{

}

ZigbeeBridgeController *ZigbeeNode::controller()
{
    return m_controller;
}

void ZigbeeNode::setShortAddress(const quint16 &shortAddress)
{
    m_shortAddress = shortAddress;
}

void ZigbeeNode::setExtendedAddress(const ZigbeeAddress &extendedAddress)
{
    m_extendedAddress = extendedAddress;
}

void ZigbeeNode::onRequestNodeDescriptionFinished()
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

    // Set node data
    m_manufacturerCode = manufacturerCode;
    m_maximalRxSize = maximalRxSize;
    m_maximalTxSize = maximalTxSize;

    // Parse server mask
    m_isPrimaryTrustCenter = ((serverMask >> 0) & 0x0001);
    m_isBackupTrustCenter = ((serverMask >> 1) & 0x0001);
    m_isPrimaryBindingCache = ((serverMask >> 2) & 0x0001);
    m_isBackupBindingCache = ((serverMask >> 3) & 0x0001);
    m_isPrimaryDiscoveryCache = ((serverMask >> 4) & 0x0001);
    m_isBackupDiscoveryCache = ((serverMask >> 5) & 0x0001);
    m_isNetworkManager = ((serverMask >> 6) & 0x0001);

    // Parse desciptor flag
    bool extendedActiveEndpointListAvailable = ((descriptorFlag >> 0) & 0x01);
    bool extendedSimpleDescriptorListAvailable = ((descriptorFlag >> 1) & 0x01);

    // Parse MAC capabilities
    m_receiverOnWhenIdle = ((macFlags >> 3) & 0x01);
    m_securityCapability = ((macFlags >> 6) & 0x01);

    // Parse bit field
    bool isCoordinator = ((bitField >> 0) & 0x0001);
    bool isRouter = ((bitField >> 1) & 0x0001);
    bool isEndDevice = ((bitField >> 2) & 0x0001);
    bool complexDescriptorAvailable = ((bitField >> 3) & 0x0001);
    bool userDescriptorAvailable = ((bitField >> 4) & 0x0001);

    if (isCoordinator && !isRouter && !isEndDevice) {
        m_nodeType = NodeTypeCoordinator;
    } else if (!isCoordinator && isRouter && !isEndDevice) {
        m_nodeType = NodeTypeRouter;
    } else if (!isCoordinator && !isRouter && isEndDevice) {
        m_nodeType = NodeTypeEndDevice;
    } else {
        if (m_isNetworkManager) {
            m_nodeType = NodeTypeCoordinator;
        } else {
            m_nodeType = NodeTypeEndDevice;
        }
    }

    // Note: Frequency always 2,4 GHz

    qCDebug(dcZigbee()) << "Node descriptor:";
    qCDebug(dcZigbee()) << "    Node type:" << m_nodeType;
    qCDebug(dcZigbee()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbee()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbee()) << "    Short address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbee()) << "    Manufacturer code:" << ZigbeeUtils::convertUint16ToHexString(manufacturerCode);
    qCDebug(dcZigbee()) << "    Maximum Rx size:" << ZigbeeUtils::convertUint16ToHexString(m_maximalRxSize);
    qCDebug(dcZigbee()) << "    Maximum Tx size:" << ZigbeeUtils::convertUint16ToHexString(m_maximalTxSize);
    qCDebug(dcZigbee()) << "    Server mask:" << ZigbeeUtils::convertUint16ToHexString(serverMask);
    qCDebug(dcZigbee()) << "        Primary Trust center:" << m_isPrimaryTrustCenter;
    qCDebug(dcZigbee()) << "        Backup Trust center:" << m_isBackupTrustCenter;
    qCDebug(dcZigbee()) << "        Primary Binding cache:" << m_isPrimaryBindingCache;
    qCDebug(dcZigbee()) << "        Backup Binding cache:" << m_isBackupBindingCache;
    qCDebug(dcZigbee()) << "        Primary Discovery cache:" << m_isPrimaryDiscoveryCache;
    qCDebug(dcZigbee()) << "        Backup Discovery cache:" << m_isBackupDiscoveryCache;
    qCDebug(dcZigbee()) << "        Network Manager:" << m_isNetworkManager;
    qCDebug(dcZigbee()) << "    Descriptor flag:" << ZigbeeUtils::convertByteToHexString(descriptorFlag);
    qCDebug(dcZigbee()) << "        Extended active endpoint list available:" << extendedActiveEndpointListAvailable;
    qCDebug(dcZigbee()) << "        Extended simple descriptor list available:" << extendedSimpleDescriptorListAvailable;
    qCDebug(dcZigbee()) << "    MAC flags:" << ZigbeeUtils::convertByteToHexString(macFlags);
    qCDebug(dcZigbee()) << "        Receiver on when idle:" << m_receiverOnWhenIdle;
    qCDebug(dcZigbee()) << "        Security capability:" << m_securityCapability;
    qCDebug(dcZigbee()) << "    Maximum buffer size:" << ZigbeeUtils::convertByteToHexString(maxBufferSize);
    qCDebug(dcZigbee()) << "    Bit field:" << ZigbeeUtils::convertUint16ToHexString(bitField);
    qCDebug(dcZigbee()) << "        Is coordinator:" << isCoordinator;
    qCDebug(dcZigbee()) << "        Is router:" << isRouter;
    qCDebug(dcZigbee()) << "        Is end device:" << isEndDevice;
    qCDebug(dcZigbee()) << "        Complex desciptor available:" << complexDescriptorAvailable;
    qCDebug(dcZigbee()) << "        User desciptor available:" << userDescriptorAvailable;


    if (userDescriptorAvailable) {
        requestUserDescriptor();
    }

}

void ZigbeeNode::onRequestSimpleNodeDescriptionFinished()
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

    if (length == 0) {
        qCWarning(dcZigbee()) << "Length 0";
        return;
    }

    quint8 endPoint = static_cast<quint8>(reply->additionalMessage().data().at(5));

    quint16 profileId = reply->additionalMessage().data().at(6);
    profileId <<= 8;
    profileId |= reply->additionalMessage().data().at(7);

    quint16 deviceId = reply->additionalMessage().data().at(8);
    deviceId <<= 8;
    deviceId |= reply->additionalMessage().data().at(9);

    quint8 bitField = static_cast<quint8>(reply->additionalMessage().data().at(10));

    qCDebug(dcZigbee()) << "Node simple descriptor:";
    qCDebug(dcZigbee()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbee()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbee()) << "    Nwk address:" << ZigbeeUtils::convertUint16ToHexString(nwkAddress);
    qCDebug(dcZigbee()) << "    Lenght:" << ZigbeeUtils::convertByteToHexString(length);
    qCDebug(dcZigbee()) << "    End Point:" << ZigbeeUtils::convertByteToHexString(endPoint);
    qCDebug(dcZigbee()) << "    Profile:" << ZigbeeUtils::profileIdToString((Zigbee::ZigbeeProfile)profileId);

    if (profileId == Zigbee::ZigbeeProfileLightLink) {
        qCDebug(dcZigbee()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::LightLinkDevice>(deviceId);
    } else {
        qCDebug(dcZigbee()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::HomeAutomationDevice>(deviceId);
    }

    qCDebug(dcZigbee()) << "    Bit field:" << ZigbeeUtils::convertByteToHexString(bitField);

    quint8 inputClusterCount = static_cast<quint8>(reply->additionalMessage().data().at(10));

    qCDebug(dcZigbee()) << "    Input clusters:";
    QByteArray inputClusterListData = reply->additionalMessage().data().mid(11, inputClusterCount * 2);
    for (int i = 0; i < inputClusterListData.count(); i+=2) {
        quint16 clusterId = inputClusterListData.at(i);
        clusterId <<= 8;
        clusterId |= inputClusterListData .at(i+1);

        qCDebug(dcZigbee()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
    }

    quint8 outputClusterCount = static_cast<quint8>(reply->additionalMessage().data().at(12 + inputClusterCount * 2));

    qCDebug(dcZigbee()) << "    Output clusters:";
    QByteArray outputClusterListData = reply->additionalMessage().data().mid(12 + inputClusterCount * 2, outputClusterCount * 2);
    for (int i = 0; i < outputClusterListData.count(); i+=2) {
        quint16 clusterId = outputClusterListData.at(i);
        clusterId <<= 8;
        clusterId |= outputClusterListData .at(i+1);

        qCDebug(dcZigbee()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
    }

}

void ZigbeeNode::onRequestPowerDescriptorFinished()
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

    quint16 bitField = reply->additionalMessage().data().at(2);
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

    qCDebug(dcZigbee()) << "Node power descriptor:";
    qCDebug(dcZigbee()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbee()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbee()) << "    Bitfiled:" << ZigbeeUtils::convertUint16ToHexString(bitField);
    qCDebug(dcZigbee()) << "    Power mode:" << m_powerMode;
    qCDebug(dcZigbee()) << "    Available power sources:";
    foreach (const PowerSource &source, m_availablePowerSources) {
        qCDebug(dcZigbee()) << "        " << source;
    }
    qCDebug(dcZigbee()) << "    Power source:" << m_powerSource;
    qCDebug(dcZigbee()) << "    Power level:" << m_powerLevel;

}

void ZigbeeNode::onRequestUserDescriptorFinished()
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

    QByteArray data;
    if (length > 0) {
        data = reply->additionalMessage().data().mid(5, length);
    }

    qCDebug(dcZigbee()) << "User descriptor:";
    qCDebug(dcZigbee()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbee()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbee()) << "    Attribute address:" << ZigbeeUtils::convertUint16ToHexString(nwkAddress);
    qCDebug(dcZigbee()) << "    Lenght:" << ZigbeeUtils::convertByteToHexString(length);
    qCDebug(dcZigbee()) << "    Data:" << data;
}

void ZigbeeNode::onToggleFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
}

void ZigbeeNode::onIdentifyFinished()
{
    
}

QDebug operator<<(QDebug debug, ZigbeeNode *node)
{
    debug.nospace().noquote() << "Node(" << ZigbeeUtils::convertUint16ToHexString(node->shortAddress()) << " | " << node->extendedAddress().toString() << ") ";
    debug.nospace().noquote() << "    " << ZigbeeUtils::convertUint16ToHexString(node->shortAddress()) << " | " << node->extendedAddress().toString() << ") ";
    return debug;
}
