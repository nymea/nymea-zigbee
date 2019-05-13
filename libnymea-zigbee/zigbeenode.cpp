#include "zigbeenode.h"
#include "zigbeeutils.h"
#include "loggingcategory.h"

#include <QDataStream>

ZigbeeNode::ZigbeeNode(QObject *parent) :
    QObject(parent)
{

}

quint16 ZigbeeNode::shortAddress() const
{
    return m_shortAddress;
}

ZigbeeAddress ZigbeeNode::extendedAddress() const
{
    return m_extendedAddress;
}

quint8 ZigbeeNode::endPoint() const
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

ZigbeeNode::Relationship ZigbeeNode::relationship() const
{
    return m_relationship;
}

Zigbee::ZigbeeProfile ZigbeeNode::profile() const
{
    return m_profile;
}

quint16 ZigbeeNode::manufacturerCode() const
{
    return m_manufacturerCode;
}

bool ZigbeeNode::complexDescriptorAvailable() const
{
    return m_complexDescriptorAvailable;
}

bool ZigbeeNode::userDescriptorAvailable() const
{
    return m_userDescriptorAvailable;
}

quint16 ZigbeeNode::maximumRxSize() const
{
    return m_maximumRxSize;
}

quint16 ZigbeeNode::maximumTxSize() const
{
    return m_maximumTxSize;
}

quint8 ZigbeeNode::maximumBufferSize() const
{
    return m_maximumBufferSize;
}

bool ZigbeeNode::isPrimaryTrustCenter() const
{
    return m_isPrimaryTrustCenter;
}

bool ZigbeeNode::isBackupTrustCenter() const
{
    return m_isBackupTrustCenter;
}

bool ZigbeeNode::isPrimaryBindingCache() const
{
    return m_isPrimaryBindingCache;
}

bool ZigbeeNode::isBackupBindingCache() const
{
    return m_isBackupBindingCache;
}

bool ZigbeeNode::isPrimaryDiscoveryCache() const
{
    return m_isPrimaryDiscoveryCache;
}

bool ZigbeeNode::isBackupDiscoveryCache() const
{
    return m_isBackupDiscoveryCache;
}

bool ZigbeeNode::isNetworkManager() const
{
    return m_isNetworkManager;
}

bool ZigbeeNode::extendedActiveEndpointListAvailable() const
{
    return m_extendedActiveEndpointListAvailable;
}

bool ZigbeeNode::extendedSimpleDescriptorListAvailable() const
{
    return m_extendedSimpleDescriptorListAvailable;
}

bool ZigbeeNode::alternatePanCoordinator() const
{
    return m_alternatePanCoordinator;
}

ZigbeeNode::DeviceType ZigbeeNode::deviceType() const
{
    return m_deviceType;
}

bool ZigbeeNode::powerSourceFlagMainPower() const
{
    return m_powerSourceFlagMainPower;
}

bool ZigbeeNode::receiverOnWhenIdle() const
{
    return m_receiverOnWhenIdle;
}

bool ZigbeeNode::securityCapability() const
{
    return m_securityCapability;
}

bool ZigbeeNode::allocateAddress() const
{
    return m_allocateAddress;
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


//void ZigbeeNode::identify()
//{
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
//}

//void ZigbeeNode::toggle(int addressMode)
//{
//    QByteArray data;
//    QDataStream stream(&data, QIODevice::WriteOnly);
//    stream << static_cast<quint8>(addressMode); // adress mode
//    stream << m_shortAddress;
//    stream << static_cast<quint8>(1); // source endpoint
//    stream << static_cast<quint8>(1); // destination endpoint
//    stream << static_cast<quint8>(2); // command toggle

//    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeCluserOnOff, data));
//    request.setDescription("Toggle request for " + ZigbeeUtils::convertUint16ToHexString(m_shortAddress));

//    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
//    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNode::onToggleFinished);
//}



void ZigbeeNode::setShortAddress(const quint16 &shortAddress)
{
    m_shortAddress = shortAddress;
}

void ZigbeeNode::setExtendedAddress(const ZigbeeAddress &extendedAddress)
{
    m_extendedAddress = extendedAddress;
}

void ZigbeeNode::setEndPoint(quint8 endPoint)
{
    m_endPoint = endPoint;
}

void ZigbeeNode::setNodeType(ZigbeeNode::NodeType nodeType)
{
    m_nodeType = nodeType;
}

void ZigbeeNode::setFrequencyBand(ZigbeeNode::FrequencyBand frequencyBand)
{
    m_frequencyBand = frequencyBand;
}

void ZigbeeNode::setRelationship(ZigbeeNode::Relationship relationship)
{
    m_relationship = relationship;
}

void ZigbeeNode::setZigbeeProfile(Zigbee::ZigbeeProfile profile)
{
    m_profile = profile;
}

void ZigbeeNode::setManufacturerCode(quint16 manufacturerCode)
{
    m_manufacturerCode = manufacturerCode;
}

void ZigbeeNode::setMaximumRxSize(quint16 size)
{
    m_maximumRxSize = size;
}

void ZigbeeNode::setMaximumTxSize(quint16 size)
{
    m_maximumTxSize = size;
}

void ZigbeeNode::setMaximumBufferSize(quint8 size)
{
    m_maximumBufferSize = size;
}

void ZigbeeNode::setServerMask(quint16 serverMask)
{
    m_isPrimaryTrustCenter = ((serverMask >> 0) & 0x0001);
    m_isBackupTrustCenter = ((serverMask >> 1) & 0x0001);
    m_isPrimaryBindingCache = ((serverMask >> 2) & 0x0001);
    m_isBackupBindingCache = ((serverMask >> 3) & 0x0001);
    m_isPrimaryDiscoveryCache = ((serverMask >> 4) & 0x0001);
    m_isBackupDiscoveryCache = ((serverMask >> 5) & 0x0001);
    m_isNetworkManager = ((serverMask >> 6) & 0x0001);
}

void ZigbeeNode::setComplexDescriptorAvailable(bool complexDescriptorAvailable)
{
    m_complexDescriptorAvailable = complexDescriptorAvailable;
}

void ZigbeeNode::setUserDescriptorAvailable(bool userDescriptorAvailable)
{
    m_userDescriptorAvailable = userDescriptorAvailable;
}

void ZigbeeNode::setMacCapabilitiesFlag(quint16 macFlag)
{
    // Parse MAC capabilities
    m_alternatePanCoordinator = ((macFlag >> 0) & 0x01);
    if (((macFlag >> 1) & 0x01)) {
        m_deviceType = DeviceTypeFullFunction;
    } else {
        m_deviceType = DeviceTypeReducedFunction;
    }
    m_powerSourceFlagMainPower = ((macFlag >> 2) & 0x01);
    m_receiverOnWhenIdle = ((macFlag >> 3) & 0x01);
    m_securityCapability = ((macFlag >> 6) & 0x01);
    m_allocateAddress = ((macFlag >> 7) & 0x01);
}

void ZigbeeNode::setDescriptorFlag(quint8 descriptorFlag)
{
    m_extendedActiveEndpointListAvailable = ((descriptorFlag >> 0) & 0x01);
    m_extendedSimpleDescriptorListAvailable = ((descriptorFlag >> 1) & 0x01);
}

//void ZigbeeNode::onRequestUserDescriptorFinished()
//{
//    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
//    reply->deleteLater();

//    if (reply->status() != ZigbeeInterfaceReply::Success) {
//        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
//        return;
//    }

//    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
//    qCDebug(dcZigbeeController()) << reply->additionalMessage();

//    quint8 sequenceNumber = static_cast<quint8>(reply->additionalMessage().data().at(0));
//    quint8 status = static_cast<quint8>(reply->additionalMessage().data().at(1));

//    quint16 nwkAddress = static_cast<quint16>(reply->additionalMessage().data().at(2));
//    nwkAddress <<= 8;
//    nwkAddress |= reply->additionalMessage().data().at(3);

//    quint8 length = static_cast<quint8>(reply->additionalMessage().data().at(4));

//    QByteArray data;
//    if (length > 0) {
//        data = reply->additionalMessage().data().mid(5, length);
//    }

//    qCDebug(dcZigbeeNetwork()) << "User descriptor:";
//    qCDebug(dcZigbeeNetwork()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
//    qCDebug(dcZigbeeNetwork()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
//    qCDebug(dcZigbeeNetwork()) << "    Attribute address:" << ZigbeeUtils::convertUint16ToHexString(nwkAddress);
//    qCDebug(dcZigbeeNetwork()) << "    Lenght:" << ZigbeeUtils::convertByteToHexString(length);
//    qCDebug(dcZigbeeNetwork()) << "    Data:" << data;
//}

//void ZigbeeNode::onToggleFinished()
//{
//    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
//    reply->deleteLater();

//    if (reply->status() != ZigbeeInterfaceReply::Success) {
//        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
//        return;
//    }

//    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
//}

//void ZigbeeNode::onIdentifyFinished()
//{
    
//}

QDebug operator<<(QDebug debug, ZigbeeNode *node)
{
    debug.nospace().noquote() << "ZigbeeNode(" << ZigbeeUtils::convertUint16ToHexString(node->shortAddress()) << ", " << node->extendedAddress().toString() << ") ";
    return debug;
}
