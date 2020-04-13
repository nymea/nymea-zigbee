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

#include "zigbeenode.h"
#include "zigbeeutils.h"
#include "loggingcategory.h"

#include <QDataStream>

ZigbeeNode::ZigbeeNode(QObject *parent) :
    QObject(parent)
{

}

ZigbeeNode::State ZigbeeNode::state() const
{
    return m_state;
}

bool ZigbeeNode::connected() const
{
    return m_connected;
}

quint16 ZigbeeNode::shortAddress() const
{
    return m_shortAddress;
}

ZigbeeAddress ZigbeeNode::extendedAddress() const
{
    return m_extendedAddress;
}

QList<ZigbeeNodeEndpoint *> ZigbeeNode::endpoints() const
{
    return m_endpoints;
}

bool ZigbeeNode::hasEndpoint(quint8 endpointId) const
{
    return getEndpoint(endpointId) != nullptr;
}

ZigbeeNodeEndpoint *ZigbeeNode::getEndpoint(quint8 endpointId) const
{
    foreach (ZigbeeNodeEndpoint *endpoint, m_endpoints) {
        if (endpoint->endpointId() == endpointId) {
            return endpoint;
        }
    }

    return nullptr;
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

void ZigbeeNode::setState(ZigbeeNode::State state)
{
    if (m_state == state)
        return;

    qCDebug(dcZigbeeNode()) << "State changed" << state;
    m_state = state;
    emit stateChanged(m_state);
}

void ZigbeeNode::setConnected(bool connected)
{
    if (m_connected == connected)
        return;

    qCDebug(dcZigbeeNode()) << "Connected changed" << connected;
    m_connected = connected;
    emit connectedChanged(m_connected);
}

void ZigbeeNode::setShortAddress(const quint16 &shortAddress)
{
    m_shortAddress = shortAddress;
}

void ZigbeeNode::setExtendedAddress(const ZigbeeAddress &extendedAddress)
{
    m_extendedAddress = extendedAddress;
}

QByteArray ZigbeeNode::nodeDescriptorRawData() const
{
    return m_nodeDescriptorRawData;
}

void ZigbeeNode::setNodeDescriptorRawData(const QByteArray nodeDescriptorRawData)
{
    m_nodeDescriptorRawData = nodeDescriptorRawData;

    // Parse the raw data
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

    QDataStream stream(&m_nodeDescriptorRawData, QIODevice::ReadOnly);
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

    // Set node data
    m_manufacturerCode = manufacturerCode;
    m_maximumRxSize = maximalRxSize;
    m_maximumTxSize = maximalTxSize;
    m_maximumBufferSize = maxBufferSize;

    setServerMask(serverMask);
    setMacCapabilitiesFlag(macFlags);
    setDescriptorFlag(descriptorFlag);

    // Parse bit field
    // 0-2 Bit = logical type, 0 = coordinator, 1 = router, 2 = end device
    if (!ZigbeeUtils::checkBitUint16(bitField, 0) && !ZigbeeUtils::checkBitUint16(bitField, 1)) {
        m_nodeType = NodeTypeCoordinator;
    } else if (!ZigbeeUtils::checkBitUint16(bitField, 0) && ZigbeeUtils::checkBitUint16(bitField, 1)) {
        m_nodeType = NodeTypeRouter;
    } else if (ZigbeeUtils::checkBitUint16(bitField, 0) && !ZigbeeUtils::checkBitUint16(bitField, 1)) {
        m_nodeType = NodeTypeEndDevice;
    }

    m_complexDescriptorAvailable = (bitField >> 3) & 0x0001;
    m_userDescriptorAvailable = (bitField >> 4) & 0x0001;

    qCDebug(dcZigbeeNode()) << "Node descriptor:";
    qCDebug(dcZigbeeNode()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbeeNode()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbeeNode()) << "    Node type:" << nodeType();
    qCDebug(dcZigbeeNode()) << "    Short address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbeeNode()) << "    Manufacturer code:" << ZigbeeUtils::convertUint16ToHexString(manufacturerCode);
    qCDebug(dcZigbeeNode()) << "    Maximum Rx size:" << ZigbeeUtils::convertUint16ToHexString(maximumRxSize());
    qCDebug(dcZigbeeNode()) << "    Maximum Tx size:" << ZigbeeUtils::convertUint16ToHexString(maximumTxSize());
    qCDebug(dcZigbeeNode()) << "    Maximum buffer size:" << ZigbeeUtils::convertByteToHexString(maximumBufferSize());
    qCDebug(dcZigbeeNode()) << "    Server mask:" << ZigbeeUtils::convertUint16ToHexString(serverMask);
    qCDebug(dcZigbeeNode()) << "        Primary Trust center:" << isPrimaryTrustCenter();
    qCDebug(dcZigbeeNode()) << "        Backup Trust center:" << isBackupTrustCenter();
    qCDebug(dcZigbeeNode()) << "        Primary Binding cache:" << isPrimaryBindingCache();
    qCDebug(dcZigbeeNode()) << "        Backup Binding cache:" << isBackupBindingCache();
    qCDebug(dcZigbeeNode()) << "        Primary Discovery cache:" << isPrimaryDiscoveryCache();
    qCDebug(dcZigbeeNode()) << "        Backup Discovery cache:" << isBackupDiscoveryCache();
    qCDebug(dcZigbeeNode()) << "        Network Manager:" << isNetworkManager();
    qCDebug(dcZigbeeNode()) << "    Descriptor flag:" << ZigbeeUtils::convertByteToHexString(descriptorFlag);
    qCDebug(dcZigbeeNode()) << "        Extended active endpoint list available:" << extendedActiveEndpointListAvailable();
    qCDebug(dcZigbeeNode()) << "        Extended simple descriptor list available:" << extendedSimpleDescriptorListAvailable();
    qCDebug(dcZigbeeNode()) << "    MAC flags:" << ZigbeeUtils::convertByteToHexString(macFlags);
    qCDebug(dcZigbeeNode()) << "        Alternate PAN coordinator:" << alternatePanCoordinator();
    qCDebug(dcZigbeeNode()) << "        Device type:" << deviceType();
    qCDebug(dcZigbeeNode()) << "        Power source flag main power:" << powerSourceFlagMainPower();
    qCDebug(dcZigbeeNode()) << "        Receiver on when idle:" << receiverOnWhenIdle();
    qCDebug(dcZigbeeNode()) << "        Security capability:" << securityCapability();
    qCDebug(dcZigbeeNode()) << "        Allocate address:" << allocateAddress();
    qCDebug(dcZigbeeNode()) << "    Bit field:" << ZigbeeUtils::convertUint16ToHexString(bitField);
    qCDebug(dcZigbeeNode()) << "        Complex desciptor available:" << complexDescriptorAvailable();
    qCDebug(dcZigbeeNode()) << "        User desciptor available:" << userDescriptorAvailable();
}

quint16 ZigbeeNode::serverMask() const
{
    return m_serverMask;
}

void ZigbeeNode::setServerMask(quint16 serverMask)
{
    m_serverMask = serverMask;
    m_isPrimaryTrustCenter = ((m_serverMask >> 0) & 0x0001);
    m_isBackupTrustCenter = ((m_serverMask >> 1) & 0x0001);
    m_isPrimaryBindingCache = ((m_serverMask >> 2) & 0x0001);
    m_isBackupBindingCache = ((m_serverMask >> 3) & 0x0001);
    m_isPrimaryDiscoveryCache = ((m_serverMask >> 4) & 0x0001);
    m_isBackupDiscoveryCache = ((m_serverMask >> 5) & 0x0001);
    m_isNetworkManager = ((m_serverMask >> 6) & 0x0001);
}

quint8 ZigbeeNode::macCapabilitiesFlag() const
{
    return m_macCapabilitiesFlag;
}

void ZigbeeNode::setMacCapabilitiesFlag(quint8 macFlag)
{
    m_macCapabilitiesFlag = macFlag;
    m_alternatePanCoordinator = ((m_macCapabilitiesFlag >> 0) & 0x01);
    if (((m_macCapabilitiesFlag >> 1) & 0x01)) {
        m_deviceType = DeviceTypeFullFunction;
    } else {
        m_deviceType = DeviceTypeReducedFunction;
    }
    m_powerSourceFlagMainPower = ((m_macCapabilitiesFlag >> 2) & 0x01);
    m_receiverOnWhenIdle = ((m_macCapabilitiesFlag >> 3) & 0x01);
    m_securityCapability = ((m_macCapabilitiesFlag >> 6) & 0x01);
    m_allocateAddress = ((m_macCapabilitiesFlag >> 7) & 0x01);
}

void ZigbeeNode::setDescriptorFlag(quint8 descriptorFlag)
{
    m_descriptorFlag = descriptorFlag;
    m_extendedActiveEndpointListAvailable = ((m_descriptorFlag >> 0) & 0x01);
    m_extendedSimpleDescriptorListAvailable = ((m_descriptorFlag >> 1) & 0x01);
}

quint16 ZigbeeNode::powerDescriptorFlag() const
{
    return m_powerDescriptorFlag;
}

void ZigbeeNode::setPowerDescriptorFlag(quint16 powerDescriptorFlag)
{
    m_powerDescriptorFlag = powerDescriptorFlag;

    qCDebug(dcZigbeeNode()) << "Parse power descriptor flag" << ZigbeeUtils::convertUint16ToHexString(m_powerDescriptorFlag);

    // Bit 0 - 3 Power mode
    // 0000: Receiver configured according to “Receiver on when idle” MAC flag in the Node Descriptor
    // 0001: Receiver switched on periodically
    // 0010: Receiver switched on when stimulated, e.g. by pressing a button

    m_powerMode = PowerModeAlwaysOn;
    if (!ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 0) && !ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 1)) {
        m_powerMode = PowerModeAlwaysOn;
    } else if (ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 0) && !ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 1)) {
        m_powerMode = PowerModeOnPeriodically;
    } else if (!ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 0) && ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 1)) {
        m_powerMode = PowerModeOnWhenStimulated;
    }

    // Bit 4 - 7 Available power sources
    // Bit 0: Permanent mains supply
    // Bit 1: Rechargeable battery
    // Bit 2: Disposable battery
    // Bit 4: Reserved

    m_availablePowerSources.clear();
    if (ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 4)) {
        m_availablePowerSources.append(PowerSourcePermanentMainSupply);
    } else if (ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 5)) {
        m_availablePowerSources.append(PowerSourceRecharchableBattery);
    } else if (ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 6)) {
        m_availablePowerSources.append(PowerSourceDisposableBattery);
    }

    // Bit 8 - 11 Active source: according to the same schema as available power sources
    m_powerSource = PowerSourcePermanentMainSupply;
    if (ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 8)) {
        m_powerSource = PowerSourcePermanentMainSupply;
    } else if (ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 9)) {
        m_powerSource = PowerSourceRecharchableBattery;
    } else if (ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 10)) {
        m_powerSource = PowerSourceDisposableBattery;
    }

    // Bit 12 - 15: Battery level if available
    // 0000: Critically low
    // 0100: Approximately 33%
    // 1000: Approximately 66%
    // 1100: Approximately 100% (near fully charged)
    m_powerLevel = PowerLevelCriticalLow;
    if (!ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 14) && !ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 15)) {
        m_powerLevel = PowerLevelCriticalLow;
    } else if (ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 14) && !ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 15)) {
        m_powerLevel = PowerLevelLow;
    } else if (!ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 14) && ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 15)) {
        m_powerLevel = PowerLevelOk;
    } else if (ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 14) && ZigbeeUtils::checkBitUint16(m_powerDescriptorFlag, 15)) {
        m_powerLevel = PowerLevelFull;
    }

    qCDebug(dcZigbeeNode()) << "Node power descriptor (" << ZigbeeUtils::convertUint16ToHexString(m_powerDescriptorFlag) << "):";
    qCDebug(dcZigbeeNode()) << "    Power mode:" << m_powerMode;
    qCDebug(dcZigbeeNode()) << "    Available power sources:";
    foreach (const PowerSource &source, m_availablePowerSources) {
        qCDebug(dcZigbeeNode()) << "        " << source;
    }
    qCDebug(dcZigbeeNode()) << "    Power source:" << m_powerSource;
    qCDebug(dcZigbeeNode()) << "    Power level:" << m_powerLevel;
}

void ZigbeeNode::startInitialization()
{
    qCWarning(dcZigbeeNode()) << "Start initialization is not implemented for this backend.";
}

void ZigbeeNode::onClusterAttributeChanged(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster *cluster = static_cast<ZigbeeCluster *>(sender());
    qCDebug(dcZigbeeNode()) << "Cluster" << cluster << "attribute changed" << attribute;
    emit clusterAttributeChanged(cluster, attribute);
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

//    qCDebug(dcZigbeeNode()) << "User descriptor:";
//    qCDebug(dcZigbeeNode()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
//    qCDebug(dcZigbeeNode()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
//    qCDebug(dcZigbeeNode()) << "    Attribute address:" << ZigbeeUtils::convertUint16ToHexString(nwkAddress);
//    qCDebug(dcZigbeeNode()) << "    Lenght:" << ZigbeeUtils::convertByteToHexString(length);
//    qCDebug(dcZigbeeNode()) << "    Data:" << data;
//}

QDebug operator<<(QDebug debug, ZigbeeNode *node)
{
    debug.nospace().noquote() << "ZigbeeNode(" << ZigbeeUtils::convertUint16ToHexString(node->shortAddress());
    debug.nospace().noquote() << ", " << node->extendedAddress().toString();
    debug.nospace().noquote() << ", " << node->nodeType();
    debug.nospace().noquote() << ")";
    return debug.space();
}
