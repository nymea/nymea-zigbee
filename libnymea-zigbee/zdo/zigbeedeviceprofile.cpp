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

#include "zigbeedeviceprofile.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeDeviceProfile::NodeDescriptor ZigbeeDeviceProfile::parseNodeDescriptor(const QByteArray &payload)
{
    NodeDescriptor nodeDescriptor;
    nodeDescriptor.descriptorRawData = payload;

    // Parse and set the node descriptor
    QDataStream stream(payload);
    stream.setByteOrder(QDataStream::LittleEndian);
    quint8 typeDescriptorFlag = 0; quint8 frequencyFlag = 0; quint8 macCapabilitiesFlag = 0;
    quint16 serverMaskFlag = 0; quint8 descriptorCapabilitiesFlag = 0;

    stream >> typeDescriptorFlag >> frequencyFlag >> macCapabilitiesFlag >> nodeDescriptor.manufacturerCode >> nodeDescriptor.maximumBufferSize;
    stream >> nodeDescriptor.maximumRxSize >> serverMaskFlag >> nodeDescriptor.maximumTxSize >> descriptorCapabilitiesFlag;

    // 0-2 Bit = logical type, 0 = coordinator, 1 = router, 2 = end device
    if (!ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 0) && !ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 1)) {
        nodeDescriptor.nodeType = NodeTypeCoordinator;
    } else if (!ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 0) && ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 1)) {
        nodeDescriptor.nodeType = NodeTypeRouter;
    } else if (ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 0) && !ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 1)) {
        nodeDescriptor.nodeType = NodeTypeEndDevice;
    }

    nodeDescriptor.complexDescriptorAvailable = (typeDescriptorFlag >> 3) & 0x0001;
    nodeDescriptor.userDescriptorAvailable = (typeDescriptorFlag >> 4) & 0x0001;

    // Frequency band, 5 bits
    if (ZigbeeUtils::checkBitUint8(frequencyFlag, 3)) {
        nodeDescriptor.frequencyBand = FrequencyBand868Mhz;
    } else if (ZigbeeUtils::checkBitUint8(frequencyFlag, 5)) {
        nodeDescriptor.frequencyBand = FrequencyBand902Mhz;
    } else if (ZigbeeUtils::checkBitUint8(frequencyFlag, 6)) {
        nodeDescriptor.frequencyBand = FrequencyBand2400Mhz;
    }

    nodeDescriptor.macCapabilities = parseMacCapabilities(macCapabilitiesFlag);
    nodeDescriptor.serverMask = parseServerMask(serverMaskFlag);
    nodeDescriptor.descriptorCapabilities = parseDescriptorCapabilities(descriptorCapabilitiesFlag);

    return nodeDescriptor;
}

ZigbeeDeviceProfile::MacCapabilities ZigbeeDeviceProfile::parseMacCapabilities(quint8 macCapabilitiesFlag)
{
    MacCapabilities capabilities;
    capabilities.flag = macCapabilitiesFlag;
    capabilities.alternatePanCoordinator = ((macCapabilitiesFlag >> 0) & 0x01);
    if (((macCapabilitiesFlag >> 1) & 0x01)) {
        capabilities.deviceType = DeviceTypeFullFunction;
    } else {
        capabilities.deviceType = DeviceTypeReducedFunction;
    }
    capabilities.powerSourceFlagMainPower = ((macCapabilitiesFlag >> 2) & 0x01);
    capabilities.receiverOnWhenIdle = ((macCapabilitiesFlag >> 3) & 0x01);
    capabilities.securityCapability = ((macCapabilitiesFlag >> 6) & 0x01);
    capabilities.allocateAddress = ((macCapabilitiesFlag >> 7) & 0x01);
    return capabilities;
}

ZigbeeDeviceProfile::ServerMask ZigbeeDeviceProfile::parseServerMask(quint16 serverMaskFlag)
{
    ServerMask serverMask;
    serverMask.serverMaskFlag = serverMaskFlag;
    serverMask.primaryTrustCenter = ((serverMaskFlag >> 0) & 0x0001);
    serverMask.backupTrustCenter = ((serverMaskFlag >> 1) & 0x0001);
    serverMask.primaryBindingCache = ((serverMaskFlag >> 2) & 0x0001);
    serverMask.backupBindingCache = ((serverMaskFlag >> 3) & 0x0001);
    serverMask.primaryDiscoveryCache = ((serverMaskFlag >> 4) & 0x0001);
    serverMask.backupDiscoveryCache = ((serverMaskFlag >> 5) & 0x0001);
    serverMask.networkManager = ((serverMaskFlag >> 6) & 0x0001);
    return serverMask;
}

ZigbeeDeviceProfile::DescriptorCapabilities ZigbeeDeviceProfile::parseDescriptorCapabilities(quint8 descriptorCapabilitiesFlag)
{
    DescriptorCapabilities capabilities;
    capabilities.descriptorCapabilitiesFlag = descriptorCapabilitiesFlag;
    capabilities.extendedActiveEndpointListAvailable = ((descriptorCapabilitiesFlag >> 0) & 0x01);
    capabilities.extendedSimpleDescriptorListAvailable = ((descriptorCapabilitiesFlag >> 1) & 0x01);
    return capabilities;
}

ZigbeeDeviceProfile::PowerDescriptor ZigbeeDeviceProfile::parsePowerDescriptor(quint16 powerDescriptorFlag)
{
    PowerDescriptor powerDescriptor;

    powerDescriptor.powerDescriptoFlag = powerDescriptorFlag;


    // Bit 0 - 3 Power mode
    // 0000: Receiver configured according to “Receiver on when idle” MAC flag in the Node Descriptor
    // 0001: Receiver switched on periodically
    // 0010: Receiver switched on when stimulated, e.g. by pressing a button

    powerDescriptor.powerMode = ZigbeeDeviceProfile::PowerModeAlwaysOn;
    if (!ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 0) && !ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 1)) {
        powerDescriptor.powerMode = ZigbeeDeviceProfile::PowerModeAlwaysOn;
    } else if (ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 0) && !ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 1)) {
        powerDescriptor.powerMode = ZigbeeDeviceProfile::PowerModeOnPeriodically;
    } else if (!ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 0) && ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 1)) {
        powerDescriptor.powerMode = ZigbeeDeviceProfile::PowerModeOnWhenStimulated;
    }

    // Bit 4 - 7 Available power sources
    //      Bit 0: Permanent mains supply
    //      Bit 1: Rechargeable battery
    //      Bit 2: Disposable battery
    //      Bit 4: Reserved
    if (ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 4)) {
        powerDescriptor.availablePowerSources.append(ZigbeeDeviceProfile::PowerSourcePermanentMainSupply);
    } else if (ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 5)) {
        powerDescriptor.availablePowerSources.append(ZigbeeDeviceProfile::PowerSourceRecharchableBattery);
    } else if (ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 6)) {
        powerDescriptor.availablePowerSources.append(ZigbeeDeviceProfile::PowerSourceDisposableBattery);
    }

    // Bit 8 - 11 Active source: according to the same schema as available power sources
    powerDescriptor.powerSource = ZigbeeDeviceProfile::PowerSourcePermanentMainSupply;
    if (ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 8)) {
        powerDescriptor.powerSource = ZigbeeDeviceProfile::PowerSourcePermanentMainSupply;
    } else if (ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 9)) {
        powerDescriptor.powerSource = ZigbeeDeviceProfile::PowerSourceRecharchableBattery;
    } else if (ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 10)) {
        powerDescriptor.powerSource = ZigbeeDeviceProfile::PowerSourceDisposableBattery;
    }

    // Bit 12 - 15: Battery level if available
    // 0000: Critically low
    // 0100: Approximately 33%
    // 1000: Approximately 66%
    // 1100: Approximately 100% (near fully charged)
    if (!ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 14) && !ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 15)) {
        powerDescriptor.powerLevel = ZigbeeDeviceProfile::PowerLevelCriticalLow;
    } else if (ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 14) && !ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 15)) {
        powerDescriptor.powerLevel = ZigbeeDeviceProfile::PowerLevelLow;
    } else if (!ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 14) && ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 15)) {
        powerDescriptor.powerLevel = ZigbeeDeviceProfile::PowerLevelOk;
    } else if (ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 14) && ZigbeeUtils::checkBitUint16(powerDescriptorFlag, 15)) {
        powerDescriptor.powerLevel = ZigbeeDeviceProfile::PowerLevelFull;
    }

    //    qCDebug(dcZigbeeNode()) << "Node power descriptor (" << ZigbeeUtils::convertUint16ToHexString(m_powerDescriptorFlag) << "):";
    //    qCDebug(dcZigbeeNode()) << "    Power mode:" << m_powerMode;
    //    qCDebug(dcZigbeeNode()) << "    Available power sources:";
    //    foreach (const PowerSource &source, m_availablePowerSources) {
    //        qCDebug(dcZigbeeNode()) << "        " << source;
    //    }
    //    qCDebug(dcZigbeeNode()) << "    Power source:" << m_powerSource;
    //    qCDebug(dcZigbeeNode()) << "    Power level:" << m_powerLevel;
    return powerDescriptor;
}

ZigbeeDeviceProfile::Adpu ZigbeeDeviceProfile::parseAdpu(const QByteArray &adpu)
{
    QDataStream stream(adpu);
    stream.setByteOrder(QDataStream::LittleEndian);

    ZigbeeDeviceProfile::Adpu deviceAdpu;
    quint8 statusFlag = 0;
    stream >> deviceAdpu.transactionSequenceNumber >> statusFlag >> deviceAdpu.addressOfInterest;
    deviceAdpu.status = static_cast<ZigbeeDeviceProfile::Status>(statusFlag);
    deviceAdpu.payload = adpu.right(adpu.length() - 4);
    return deviceAdpu;
}

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::Adpu &deviceAdpu)
{
    debug.nospace() << "DeviceProfileAdpu(SQN: " << deviceAdpu.transactionSequenceNumber << ", ";
    debug.nospace() << deviceAdpu.status << ", ";
    debug.nospace() << ZigbeeUtils::convertUint16ToHexString(deviceAdpu.addressOfInterest) << ", ";
    debug.nospace() << "Payload: " << ZigbeeUtils::convertByteArrayToHexString(deviceAdpu.payload) << ")";
    return debug.space();
}

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::NodeDescriptor &nodeDescriptor)
{
    debug.nospace() << "NodeDescriptor(" << nodeDescriptor.nodeType << ")" << "\n";
    debug.nospace() << "    Complex descriptor available: " << nodeDescriptor.complexDescriptorAvailable << "\n";
    debug.nospace() << "    User descriptor available: " << nodeDescriptor.userDescriptorAvailable << "\n";
    debug.nospace() << "    " << nodeDescriptor.frequencyBand << "\n";
    debug.nospace() << "    " << nodeDescriptor.macCapabilities << "\n";
    debug.nospace() << "    Manufacturer code: " << ZigbeeUtils::convertUint16ToHexString(nodeDescriptor.manufacturerCode) << "(" << nodeDescriptor.manufacturerCode << ")" << "\n";
    debug.nospace() << "    Maximum buffer size: " << nodeDescriptor.maximumBufferSize << "\n";
    debug.nospace() << "    Maximum RX size: " << nodeDescriptor.maximumRxSize << "\n";
    debug.nospace() << "    Maximum TX size: " << nodeDescriptor.maximumTxSize << "\n";
    debug.nospace() << "    " << nodeDescriptor.serverMask << "\n";
    debug.nospace() << "    " << nodeDescriptor.descriptorCapabilities;
    return debug;
}

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::MacCapabilities &macCapabilities)
{
    debug.nospace() << "MacCapabilities(" << ZigbeeUtils::convertByteToHexString(macCapabilities.flag) << ")" << "\n";
    debug.nospace() << "    Alternate PAN Coordinator: " << macCapabilities.alternatePanCoordinator << "\n";
    debug.nospace() << "    " << macCapabilities.deviceType << "\n";
    debug.nospace() << "    Power source main power: " << macCapabilities.powerSourceFlagMainPower << "\n";
    debug.nospace() << "    Receiver on when idle: " << macCapabilities.receiverOnWhenIdle << "\n";
    debug.nospace() << "    Security capability: " << macCapabilities.securityCapability << "\n";
    debug.nospace() << "    Allocate address: " << macCapabilities.allocateAddress;
    return debug;
}

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::ServerMask &serverMask)
{
    debug.nospace() << "ServerMask(" << ZigbeeUtils::convertUint16ToHexString(serverMask.serverMaskFlag) << ")" << "\n";
    debug.nospace() << "    Primary trust center: " << serverMask.primaryTrustCenter << "\n";
    debug.nospace() << "    Backup trust center: " << serverMask.backupTrustCenter << "\n";
    debug.nospace() << "    Primary binding cache: " << serverMask.primaryBindingCache << "\n";
    debug.nospace() << "    Backup binding cache: " << serverMask.backupBindingCache << "\n";
    debug.nospace() << "    Primary discovery cache: " << serverMask.primaryDiscoveryCache << "\n";
    debug.nospace() << "    Backup discovery cache: " << serverMask.backupDiscoveryCache << "\n";
    debug.nospace() << "    Network manager: " << serverMask.networkManager;
    return debug;
}

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::DescriptorCapabilities &descriptorCapabilities)
{
    debug.nospace() << "DescriptorCapabilities(" << ZigbeeUtils::convertByteToHexString(descriptorCapabilities.descriptorCapabilitiesFlag) << ")" << "\n";
    debug.nospace() << "    Extended active endpoint list available: " << descriptorCapabilities.extendedActiveEndpointListAvailable << "\n";
    debug.nospace() << "    Extended simple descriptor list available: " << descriptorCapabilities.extendedSimpleDescriptorListAvailable;
    return debug;
}

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::PowerDescriptor &powerDescriptor)
{
    debug.nospace() << "PowerDescriptor(" << ZigbeeUtils::convertByteToHexString(powerDescriptor.powerDescriptoFlag) << ")" << "\n";
    debug.nospace() << "    Power mode: " << powerDescriptor.powerMode << "\n";
    debug.nospace() << "    Available power sources: " << powerDescriptor.availablePowerSources << "\n";
    debug.nospace() << "    Power source: " << powerDescriptor.powerSource << "\n";
    debug.nospace() << "    Power level: " << powerDescriptor.powerLevel;
    return debug;
}

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::BindingTableListRecord &bindingTableListRecord)
{
    debug.nospace() << "BindingTableListRecord(" << bindingTableListRecord.sourceAddress.toString() << ", ";
    debug.nospace() << "source endpoint: " << bindingTableListRecord.sourceEndpoint << ", ";
    debug.nospace() << "cluster: " << static_cast<ZigbeeClusterLibrary::ClusterId>(bindingTableListRecord.clusterId) << " --> ";
    switch (bindingTableListRecord.destinationAddressMode) {
    case Zigbee::DestinationAddressModeGroup:
        debug.nospace() << "destination address (group): " << ZigbeeUtils::convertUint16ToHexString(bindingTableListRecord.destinationAddressShort) << ") ";
        break;
    case Zigbee::DestinationAddressModeIeeeAddress:
        debug.nospace() << "destination address (unicast): " << bindingTableListRecord.destinationAddress.toString() << ", ";
        debug.nospace() << "destination endpoint: " << bindingTableListRecord.destinationEndpoint << ") ";
        break;
    default:
        break;
    }
    return debug;
}
