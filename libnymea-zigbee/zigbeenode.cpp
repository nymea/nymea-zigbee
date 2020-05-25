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
#include "zigbeenetwork.h"
#include "loggingcategory.h"

#include <QDataStream>

ZigbeeNode::ZigbeeNode(ZigbeeNetwork *network, quint16 shortAddress, const ZigbeeAddress &extendedAddress, QObject *parent) :
    QObject(parent),
    m_network(network),
    m_shortAddress(shortAddress),
    m_extendedAddress(extendedAddress)
{
    m_deviceObject = new ZigbeeDeviceObject(m_network, this, this);
}

ZigbeeNode::State ZigbeeNode::state() const
{
    return m_state;
}

bool ZigbeeNode::connected() const
{
    return m_connected;
}

ZigbeeDeviceObject *ZigbeeNode::deviceObject() const
{
    return m_deviceObject;
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

    qCDebug(dcZigbeeNode()) << "State changed" << this << state;
    m_state = state;
    emit stateChanged(m_state);
}

void ZigbeeNode::setConnected(bool connected)
{
    if (m_connected == connected)
        return;

    qCDebug(dcZigbeeNode()) << "Connected changed"  << this << connected;
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
    //      Bit 0: Permanent mains supply
    //      Bit 1: Rechargeable battery
    //      Bit 2: Disposable battery
    //      Bit 4: Reserved

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

    //    qCDebug(dcZigbeeNode()) << "Node power descriptor (" << ZigbeeUtils::convertUint16ToHexString(m_powerDescriptorFlag) << "):";
    //    qCDebug(dcZigbeeNode()) << "    Power mode:" << m_powerMode;
    //    qCDebug(dcZigbeeNode()) << "    Available power sources:";
    //    foreach (const PowerSource &source, m_availablePowerSources) {
    //        qCDebug(dcZigbeeNode()) << "        " << source;
    //    }
    //    qCDebug(dcZigbeeNode()) << "    Power source:" << m_powerSource;
    //    qCDebug(dcZigbeeNode()) << "    Power level:" << m_powerLevel;
}

void ZigbeeNode::startInitialization()
{
    setState(StateInitializing);

    /* Node initialisation steps (sequentially)
      * - Node descriptor
      * - Power descriptor
      * - Active endpoints
      * - for each endpoint do:
      *    - Simple descriptor request
      *    - for each endpoint
      *      - read basic cluster
      */

    initNodeDescriptor();
}

void ZigbeeNode::initNodeDescriptor()
{
    ZigbeeDeviceObjectReply *reply = deviceObject()->requestNodeDescriptor();
    connect(reply, &ZigbeeDeviceObjectReply::finished, this, [this, reply](){
        if (reply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read node descriptor" << reply->error();
            // FIXME: decide what to do, retry or stop initialization
            return;
        }

        if (reply->responseAdpu().status != ZigbeeDeviceProfile::StatusSuccess) {
            qCWarning(dcZigbeeNode()) << this << "failed to read node descriptor" << reply->responseAdpu().status;
            // FIXME: decide what to do, retry or stop initialization
            return;
        }

        qCDebug(dcZigbeeNode()) << this << "reading node descriptor finished successfully.";

        // Parse and set the node descriptor FIXME: make it nicer using the data types

        QDataStream stream(reply->responseAdpu().payload);
        stream.setByteOrder(QDataStream::LittleEndian);
        quint8 typeDescriptorFlag = 0; quint8 frequencyFlag = 0; quint8 macCapabilities = 0;
        quint16 serverMask = 0;
        quint8 descriptorCapabilities = 0;

        stream >> typeDescriptorFlag >> frequencyFlag >> macCapabilities >> m_manufacturerCode >> m_maximumBufferSize;
        stream >> m_maximumRxSize >> serverMask >> m_maximumTxSize >> descriptorCapabilities;

        // 0-2 Bit = logical type, 0 = coordinator, 1 = router, 2 = end device
        if (!ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 0) && !ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 1)) {
            m_nodeType = NodeTypeCoordinator;
        } else if (!ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 0) && ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 1)) {
            m_nodeType = NodeTypeRouter;
        } else if (ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 0) && !ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 1)) {
            m_nodeType = NodeTypeEndDevice;
        }

        m_complexDescriptorAvailable = (typeDescriptorFlag >> 3) & 0x0001;
        m_userDescriptorAvailable = (typeDescriptorFlag >> 4) & 0x0001;

        // Frequency band, 5 bits
        if (ZigbeeUtils::checkBitUint8(frequencyFlag, 3)) {
            m_frequencyBand = FrequencyBand868Mhz;
        } else if (ZigbeeUtils::checkBitUint8(frequencyFlag, 5)) {
            m_frequencyBand = FrequencyBand902Mhz;
        } else if (ZigbeeUtils::checkBitUint8(frequencyFlag, 6)) {
            m_frequencyBand = FrequencyBand2400Mhz;
        }

        setMacCapabilitiesFlag(macCapabilities);
        setServerMask(serverMask);
        setDescriptorFlag(descriptorCapabilities);

        qCDebug(dcZigbeeNode()) << "Node descriptor:" << ZigbeeUtils::convertUint16ToHexString(shortAddress()) << extendedAddress().toString();
        qCDebug(dcZigbeeNode()) << "    Node type:" << nodeType();
        qCDebug(dcZigbeeNode()) << "    Complex desciptor available:" << complexDescriptorAvailable();
        qCDebug(dcZigbeeNode()) << "    User desciptor available:" << userDescriptorAvailable();
        qCDebug(dcZigbeeNode()) << "    Frequency band:" << frequencyBand();
        qCDebug(dcZigbeeNode()) << "    Manufacturer code:" << ZigbeeUtils::convertUint16ToHexString(m_manufacturerCode);
        qCDebug(dcZigbeeNode()) << "    Maximum Rx size:" << ZigbeeUtils::convertUint16ToHexString(m_maximumRxSize) << "(" <<  m_maximumRxSize << ")";
        qCDebug(dcZigbeeNode()) << "    Maximum Tx size:" << ZigbeeUtils::convertUint16ToHexString(m_maximumTxSize) << "(" << m_maximumTxSize << ")";
        qCDebug(dcZigbeeNode()) << "    Maximum buffer size:" << ZigbeeUtils::convertByteToHexString(m_maximumBufferSize) << "(" << m_maximumBufferSize << ")";
        qCDebug(dcZigbeeNode()) << "    Server mask:" << ZigbeeUtils::convertUint16ToHexString(serverMask);
        qCDebug(dcZigbeeNode()) << "        Primary Trust center:" << isPrimaryTrustCenter();
        qCDebug(dcZigbeeNode()) << "        Backup Trust center:" << isBackupTrustCenter();
        qCDebug(dcZigbeeNode()) << "        Primary Binding cache:" << isPrimaryBindingCache();
        qCDebug(dcZigbeeNode()) << "        Backup Binding cache:" << isBackupBindingCache();
        qCDebug(dcZigbeeNode()) << "        Primary Discovery cache:" << isPrimaryDiscoveryCache();
        qCDebug(dcZigbeeNode()) << "        Backup Discovery cache:" << isBackupDiscoveryCache();
        qCDebug(dcZigbeeNode()) << "        Network Manager:" << isNetworkManager();
        qCDebug(dcZigbeeNode()) << "    Descriptor flag:" << ZigbeeUtils::convertByteToHexString(descriptorCapabilities);
        qCDebug(dcZigbeeNode()) << "        Extended active endpoint list available:" << extendedActiveEndpointListAvailable();
        qCDebug(dcZigbeeNode()) << "        Extended simple descriptor list available:" << extendedSimpleDescriptorListAvailable();
        qCDebug(dcZigbeeNode()) << "    MAC flags:" << ZigbeeUtils::convertByteToHexString(macCapabilities);
        qCDebug(dcZigbeeNode()) << "        Alternate PAN coordinator:" << alternatePanCoordinator();
        qCDebug(dcZigbeeNode()) << "        Device type:" << deviceType();
        qCDebug(dcZigbeeNode()) << "        Power source flag main power:" << powerSourceFlagMainPower();
        qCDebug(dcZigbeeNode()) << "        Receiver on when idle:" << receiverOnWhenIdle();
        qCDebug(dcZigbeeNode()) << "        Security capability:" << securityCapability();
        qCDebug(dcZigbeeNode()) << "        Allocate address:" << allocateAddress();


        // Continue with the power descriptor
        initPowerDescriptor();
    });
}

void ZigbeeNode::initPowerDescriptor()
{
    ZigbeeDeviceObjectReply *reply = deviceObject()->requestPowerDescriptor();
    connect(reply, &ZigbeeDeviceObjectReply::finished, this, [this, reply](){
        if (reply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read power descriptor" << reply->error();
            // FIXME: decide what to do, retry or stop initialization
            return;
        }

        ZigbeeDeviceProfile::Adpu adpu = reply->responseAdpu();
        if (adpu.status != ZigbeeDeviceProfile::StatusSuccess) {
            qCWarning(dcZigbeeNode()) << this << "failed to read node descriptor" << adpu.status;
            // FIXME: decide what to do, retry or stop initialization
            return;
        }

        qCDebug(dcZigbeeNode()) << this << "reading power descriptor finished successfully.";

        QDataStream stream(adpu.payload);
        stream.setByteOrder(QDataStream::LittleEndian);
        quint16 powerDescriptorFlag = 0;
        stream >> powerDescriptorFlag;
        setPowerDescriptorFlag(powerDescriptorFlag);

        // Continue with endpoint fetching
        initEndpoints();
    });
}

void ZigbeeNode::initEndpoints()
{
    ZigbeeDeviceObjectReply *reply = deviceObject()->requestActiveEndpoints();
    connect(reply, &ZigbeeDeviceObjectReply::finished, this, [this, reply](){
        if (reply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read active endpoints" << reply->error();
            // FIXME: decide what to do, retry or stop initialization
            return;
        }

        if (reply->responseAdpu().status != ZigbeeDeviceProfile::StatusSuccess) {
            qCWarning(dcZigbeeNode()) << this << "failed to read active endpoints" << reply->responseAdpu().status;
            // FIXME: decide what to do, retry or stop initialization
            return;
        }

        qCDebug(dcZigbeeNode()) << this << "reading active endpoints finished successfully.";

        QDataStream stream(reply->responseAdpu().payload);
        stream.setByteOrder(QDataStream::LittleEndian);
        quint8 endpointCount = 0;
        m_uninitializedEndpoints.clear();
        stream >> endpointCount;
        for (int i = 0; i < endpointCount; i++) {
            quint8 endpoint = 0;
            stream >> endpoint;
            m_uninitializedEndpoints.append(endpoint);
        }

        qCDebug(dcZigbeeNode()) << "Endpoints (" << endpointCount << ")";
        for (int i = 0; i < m_uninitializedEndpoints.count(); i++) {
            qCDebug(dcZigbeeNode()) << " -" << ZigbeeUtils::convertByteToHexString(m_uninitializedEndpoints.at(i));
        }

        // If there a no endpoints or all endpoints have already be initialized, continue with reading the basic cluster information
        if (m_uninitializedEndpoints.isEmpty()) {
            initBasicCluster();
        }

        // Read simple descriptor for each uninitialized endpoint
        for (int i = 0; i < m_uninitializedEndpoints.count(); i++) {
            quint8 endpointId = m_uninitializedEndpoints.at(i);
            qCDebug(dcZigbeeNode()) << "Read simple descriptor of endpoint" << ZigbeeUtils::convertByteToHexString(endpointId);
            initEndpoint(endpointId);
        }
    });
}


void ZigbeeNode::initEndpoint(quint8 endpointId)
{
    ZigbeeDeviceObjectReply *reply = deviceObject()->requestSimpleDescriptor(endpointId);
    connect(reply, &ZigbeeDeviceObjectReply::finished, this, [this, reply, endpointId](){
        if (reply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read simple descriptor for endpoint" << endpointId << reply->error();
            // FIXME: decide what to do, retry or stop initialization
            return;
        }

        if (reply->responseAdpu().status != ZigbeeDeviceProfile::StatusSuccess) {
            qCWarning(dcZigbeeNode()) << this << "failed to read simple descriptor from endpoint" << endpointId << reply->responseAdpu().status;
            // FIXME: decide what to do, retry or stop initialization
            return;
        }

        qCDebug(dcZigbeeNode()) << this << "reading simple descriptor for endpoint" << endpointId << "finished successfully.";

        quint8 length = 0; quint8 endpointId = 0; quint16 profileId = 0; quint16 deviceId = 0; quint8 deviceVersion = 0;
        quint8 inputClusterCount = 0; quint8 outputClusterCount = 0;
        QList<quint16> inputClusters;
        QList<quint16> outputClusters;

        QDataStream stream(reply->responseAdpu().payload);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> length >> endpointId >> profileId >> deviceId >> deviceVersion >> inputClusterCount;

        qCDebug(dcZigbeeNode()) << "Node endpoint simple descriptor:";
        qCDebug(dcZigbeeNode()) << "    Lenght:" << ZigbeeUtils::convertByteToHexString(length);
        qCDebug(dcZigbeeNode()) << "    End Point:" << ZigbeeUtils::convertByteToHexString(endpointId);
        qCDebug(dcZigbeeNode()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));
        if (profileId == Zigbee::ZigbeeProfileLightLink) {
            qCDebug(dcZigbeeNode()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::LightLinkDevice>(deviceId);
        } else if (profileId == Zigbee::ZigbeeProfileHomeAutomation) {
            qCDebug(dcZigbeeNode()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::HomeAutomationDevice>(deviceId);
        } else if (profileId == Zigbee::ZigbeeProfileGreenPower) {
            qCDebug(dcZigbeeNode()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::GreenPowerDevice>(deviceId);
        }

        qCDebug(dcZigbeeNode()) << "    Device version:" << ZigbeeUtils::convertByteToHexString(deviceVersion);

        // Create endpoint
        ZigbeeNodeEndpoint *endpoint = nullptr;
        if (!hasEndpoint(endpointId)) {
            endpoint = new ZigbeeNodeEndpoint(m_network, this, endpointId, this);
            m_endpoints.append(endpoint);
        } else {
            endpoint = getEndpoint(endpointId);
        }
        endpoint->setProfile(static_cast<Zigbee::ZigbeeProfile>(profileId));
        endpoint->setDeviceId(deviceId);
        endpoint->setDeviceVersion(deviceVersion);

        qCDebug(dcZigbeeNode()) << "    Input clusters: (" << inputClusterCount << ")";
        for (int i = 0; i < inputClusterCount; i++) {
            quint16 clusterId = 0;
            stream >> clusterId;
            if (!endpoint->hasInputCluster(static_cast<Zigbee::ClusterId>(clusterId))) {
                endpoint->addInputCluster(endpoint->createCluster(static_cast<Zigbee::ClusterId>(clusterId), ZigbeeCluster::Input));
            }
            qCDebug(dcZigbeeNode()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
        }

        stream >> outputClusterCount;

        qCDebug(dcZigbeeNode()) << "    Output clusters: (" << outputClusterCount << ")";
        for (int i = 0; i < outputClusterCount; i++) {
            quint16 clusterId = 0;
            stream >> clusterId;
            if (!endpoint->hasOutputCluster(static_cast<Zigbee::ClusterId>(clusterId))) {
                endpoint->addOutputCluster(endpoint->createCluster(static_cast<Zigbee::ClusterId>(clusterId), ZigbeeCluster::Output));
            }
            qCDebug(dcZigbeeNode()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
        }

        m_uninitializedEndpoints.removeAll(endpointId);

        if (m_uninitializedEndpoints.isEmpty()) {

            if (m_shortAddress == 0) {
                setState(StateInitialized);
                return;
            }

            // Continue with the basic cluster attributes
            initBasicCluster();
        }
    });
}

void ZigbeeNode::initBasicCluster()
{
    ZigbeeClusterBasic *basicCluster = m_endpoints.first()->inputCluster<ZigbeeClusterBasic>(Zigbee::ClusterIdBasic);

    if (!basicCluster) {
        qCWarning(dcZigbeeNode()) << this << "could not find basic server cluster";
        setState(StateInitialized);
        return;
    }

    ZigbeeClusterBasic::Attribute attributeId = ZigbeeClusterBasic::AttributeManufacturerName;
    qCDebug(dcZigbeeNode()) << "Reading attribute" << attributeId;
    ZigbeeClusterReply *reply = basicCluster->readAttributes({static_cast<quint16>(attributeId)});
    connect(reply, &ZigbeeClusterReply::finished, this, [this, basicCluster, reply, attributeId](){
        if (reply->error() != ZigbeeClusterReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read basic cluster attribute" << attributeId << reply->error();
        } else {
            qCDebug(dcZigbeeNode()) << "Reading basic cluster attributes finished successfully";
            QList<ZigbeeClusterLibrary::ReadAttributeStatusRecord> attributeStatusRecords = ZigbeeClusterLibrary::parseAttributeStatusRecords(reply->responseFrame().payload);
            if (!attributeStatusRecords.isEmpty()) {
                qCDebug(dcZigbeeNode()) << attributeStatusRecords.first();
                basicCluster->setAttribute(ZigbeeClusterAttribute(static_cast<quint16>(attributeId), attributeStatusRecords.first().dataType, attributeStatusRecords.first().data));
                m_endpoints.first()->m_manufacturerName = QString::fromUtf8(attributeStatusRecords.first().data);
            }
        }

        ZigbeeClusterBasic::Attribute attributeId = ZigbeeClusterBasic::AttributeModelIdentifier;
        qCDebug(dcZigbeeNode()) << "Reading attribute" << attributeId;
        ZigbeeClusterReply *reply = basicCluster->readAttributes({static_cast<quint16>(attributeId)});
        connect(reply, &ZigbeeClusterReply::finished, this, [this, basicCluster, reply, attributeId](){
            if (reply->error() != ZigbeeClusterReply::ErrorNoError) {
                qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read basic cluster attribute" << attributeId << reply->error();
            } else {
                qCDebug(dcZigbeeNode()) << "Reading basic cluster attributes finished successfully";
                QList<ZigbeeClusterLibrary::ReadAttributeStatusRecord> attributeStatusRecords = ZigbeeClusterLibrary::parseAttributeStatusRecords(reply->responseFrame().payload);
                if (!attributeStatusRecords.isEmpty()) {
                    qCDebug(dcZigbeeNode()) << attributeStatusRecords.first();
                    basicCluster->setAttribute(ZigbeeClusterAttribute(static_cast<quint16>(attributeId), attributeStatusRecords.first().dataType, attributeStatusRecords.first().data));
                    m_endpoints.first()->m_modelIdentifier = QString::fromUtf8(attributeStatusRecords.first().data);
                }
            }


            ZigbeeClusterBasic::Attribute attributeId = ZigbeeClusterBasic::AttributeSwBuildId;
            qCDebug(dcZigbeeNode()) << "Reading attribute" << attributeId;
            ZigbeeClusterReply *reply = basicCluster->readAttributes({static_cast<quint16>(attributeId)});
            connect(reply, &ZigbeeClusterReply::finished, this, [this, basicCluster, reply, attributeId](){
                if (reply->error() != ZigbeeClusterReply::ErrorNoError) {
                    qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read basic cluster attribute" << attributeId << reply->error();
                } else {
                    qCDebug(dcZigbeeNode()) << "Reading basic cluster attributes finished successfully";
                    QList<ZigbeeClusterLibrary::ReadAttributeStatusRecord> attributeStatusRecords = ZigbeeClusterLibrary::parseAttributeStatusRecords(reply->responseFrame().payload);
                    if (!attributeStatusRecords.isEmpty()) {
                        qCDebug(dcZigbeeNode()) << attributeStatusRecords.first();
                        basicCluster->setAttribute(ZigbeeClusterAttribute(static_cast<quint16>(attributeId), attributeStatusRecords.first().dataType, attributeStatusRecords.first().data));
                        m_endpoints.first()->m_softwareBuildId = QString::fromUtf8(attributeStatusRecords.first().data);
                    }
                }

                // Finished with reading basic cluster, the node is initialized. TODO: read other cluster information
                setState(StateInitialized);
            });
        });
    });

}

void ZigbeeNode::onClusterAttributeChanged(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster *cluster = static_cast<ZigbeeCluster *>(sender());
    qCDebug(dcZigbeeNode()) << "Cluster" << cluster << "attribute changed" << attribute;
    emit clusterAttributeChanged(cluster, attribute);
}

QDebug operator<<(QDebug debug, ZigbeeNode *node)
{
    debug.nospace().noquote() << "ZigbeeNode(" << ZigbeeUtils::convertUint16ToHexString(node->shortAddress());
    debug.nospace().noquote() << ", " << node->extendedAddress().toString();
    debug.nospace().noquote() << ")";
    return debug.space();
}
