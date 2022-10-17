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
    quint8 typeAndDescriptorFlags = 0, frequencyAndApsFlags = 0, macCapabilitiesFlags = 0, descriptorCapabilities = 0;
    quint16 serverMask = 0;

    stream >> typeAndDescriptorFlags >> frequencyAndApsFlags >> macCapabilitiesFlags >> nodeDescriptor.manufacturerCode >> nodeDescriptor.maximumBufferSize;
    stream >> nodeDescriptor.maximumRxSize >> serverMask >> nodeDescriptor.maximumTxSize >> descriptorCapabilities;

    nodeDescriptor.nodeType = static_cast<NodeType>(typeAndDescriptorFlags & 0x07);
    nodeDescriptor.complexDescriptorAvailable = typeAndDescriptorFlags & 0x08;
    nodeDescriptor.userDescriptorAvailable = typeAndDescriptorFlags & 0x10;

    // Frequency band, 5 bits
    if (ZigbeeUtils::checkBitUint8(frequencyAndApsFlags, 3)) {
        nodeDescriptor.frequencyBand = FrequencyBand868Mhz;
    } else if (ZigbeeUtils::checkBitUint8(frequencyAndApsFlags, 5)) {
        nodeDescriptor.frequencyBand = FrequencyBand902Mhz;
    } else if (ZigbeeUtils::checkBitUint8(frequencyAndApsFlags, 6)) {
        nodeDescriptor.frequencyBand = FrequencyBand2400Mhz;
    }

    nodeDescriptor.macCapabilities = parseMacCapabilities(macCapabilitiesFlags);
    nodeDescriptor.serverMask = parseServerMask(serverMask);
    nodeDescriptor.descriptorCapabilities = parseDescriptorCapabilities(descriptorCapabilities);

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

ZigbeeDeviceProfile::BindingTable ZigbeeDeviceProfile::parseBindingTable(const QByteArray &payload)
{
    ZigbeeDeviceProfile::BindingTable table;

    QDataStream stream(payload);
    stream.setByteOrder(QDataStream::LittleEndian);

    quint8 sqn, status, listRecordCount;
    stream >> sqn >> status >> table.tableSize >> table.startIndex >> listRecordCount;

    table.status = static_cast<Status>(status);

    for (int i = 0; i < listRecordCount; i++) {
        ZigbeeDeviceProfile::BindingTableListRecord record;

        quint64 sourceAddress;
        stream >> sourceAddress;
        record.sourceAddress = ZigbeeAddress(sourceAddress);

        stream >> record.sourceEndpoint;
        stream >> record.clusterId;
        quint8 addressMode;
        stream >> addressMode;
        record.destinationAddressMode = static_cast<Zigbee::DestinationAddressMode>(addressMode);

        if (addressMode == Zigbee::DestinationAddressModeGroup) {
            stream >> record.destinationShortAddress;
        } else if (addressMode == Zigbee::DestinationAddressModeIeeeAddress) {
            quint64 destinationAddressIeee;
            stream >> destinationAddressIeee >> record.destinationEndpoint;
            record.destinationIeeeAddress = ZigbeeAddress(destinationAddressIeee);
        }
        table.records.append(record);
    }

    return table;
}

ZigbeeDeviceProfile::NeighborTable ZigbeeDeviceProfile::parseNeighborTable(const QByteArray &payload)
{
    ZigbeeDeviceProfile::NeighborTable table;
    QDataStream stream(payload);
    stream.setByteOrder(QDataStream::LittleEndian);

    quint8 messageId, status, listRecordCount;
    stream >> messageId >> status >> table.tableSize >> table.startIndex >> listRecordCount;
    table.status = static_cast<Status>(status);
    for (int i = 0; i < listRecordCount; i++) {
        ZigbeeDeviceProfile::NeighborTableListRecord record;
        stream >> record.extendedPanId;
        quint64 ieeeAddress;
        stream >> ieeeAddress;
        record.ieeeAddress = ZigbeeAddress(ieeeAddress);
        stream >> record.shortAddress;

        quint8 nodeFlags;
        stream >> nodeFlags;
        record.nodeType = static_cast<NodeType>(nodeFlags & 0x03);
        record.receiverOnWhenIdle = static_cast<bool>((nodeFlags & 0x0c) >> 2);
        record.relationship = static_cast<Relationship>((nodeFlags & 0x70) >> 4);

        stream >> nodeFlags;
        record.permitJoining = (nodeFlags & 0x03) == 1;
        stream >> record.depth;
        stream >> record.lqi;

        table.records.append(record);
    }

    return table;
}

ZigbeeDeviceProfile::RoutingTable ZigbeeDeviceProfile::parseRoutingTable(const QByteArray &payload)
{
    ZigbeeDeviceProfile::RoutingTable table;
    QDataStream stream(payload);
    stream.setByteOrder(QDataStream::LittleEndian);

    quint8 messageId, status, listRecordCount;
    stream >> messageId >> status >> table.tableSize >> table.startIndex >> listRecordCount;
    table.status = static_cast<Status>(status);
    for (int i = 0; i < listRecordCount; i++) {
        ZigbeeDeviceProfile::RoutingTableListRecord record;
        stream >> record.destinationAddress;

        qint8 flags;
        stream >> flags;
        record.status = static_cast<RouteStatus>(flags & 0x07);
        record.memoryConstrained = (flags & 0x08) > 0;
        record.manyToOne = (flags & 0x10) > 0;
        record.routeRecordRequired = (flags & 0x20) > 0;

        stream >> record.nextHopAddress;

        table.records.append(record);
    }

    return table;

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

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::Adpu &adpu)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "DeviceProfileAdpu(SQN: " << adpu.transactionSequenceNumber << ", ";
    debug.nospace() << adpu.status << ", ";
    debug.nospace() << ZigbeeUtils::convertUint16ToHexString(adpu.addressOfInterest) << ", ";
    debug.nospace() << "Payload: " << ZigbeeUtils::convertByteArrayToHexString(adpu.payload) << ")";
    return debug;
}

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::NodeDescriptor &nodeDescriptor)
{
    QDebugStateSaver saver(debug);
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
    QDebugStateSaver saver(debug);
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
    QDebugStateSaver saver(debug);
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
    QDebugStateSaver saver(debug);
    debug.nospace() << "DescriptorCapabilities(" << ZigbeeUtils::convertByteToHexString(descriptorCapabilities.descriptorCapabilitiesFlag) << ")" << "\n";
    debug.nospace() << "    Extended active endpoint list available: " << descriptorCapabilities.extendedActiveEndpointListAvailable << "\n";
    debug.nospace() << "    Extended simple descriptor list available: " << descriptorCapabilities.extendedSimpleDescriptorListAvailable;
    return debug;
}

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::PowerDescriptor &powerDescriptor)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "PowerDescriptor(" << ZigbeeUtils::convertByteToHexString(powerDescriptor.powerDescriptoFlag) << ")" << "\n";
    debug.nospace() << "    Power mode: " << powerDescriptor.powerMode << "\n";
    debug.nospace() << "    Available power sources: " << powerDescriptor.availablePowerSources << "\n";
    debug.nospace() << "    Power source: " << powerDescriptor.powerSource << "\n";
    debug.nospace() << "    Power level: " << powerDescriptor.powerLevel;
    return debug;
}

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::BindingTableListRecord &bindingTableListRecord)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "BindingTableListRecord(" << bindingTableListRecord.sourceAddress.toString() << ", ";
    debug.nospace() << "source endpoint: " << bindingTableListRecord.sourceEndpoint << ", ";
    debug.nospace() << "cluster: " << static_cast<ZigbeeClusterLibrary::ClusterId>(bindingTableListRecord.clusterId) << " --> ";
    switch (bindingTableListRecord.destinationAddressMode) {
    case Zigbee::DestinationAddressModeGroup:
        debug.nospace() << "destination address (group): " << ZigbeeUtils::convertUint16ToHexString(bindingTableListRecord.destinationShortAddress) << ") ";
        break;
    case Zigbee::DestinationAddressModeIeeeAddress:
        debug.nospace() << "destination address (unicast): " << bindingTableListRecord.destinationIeeeAddress.toString() << ", ";
        debug.nospace() << "destination endpoint: " << bindingTableListRecord.destinationEndpoint << ") ";
        break;
    default:
        break;
    }
    return debug;
}

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::NeighborTableListRecord &neighborTableListRecord)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "NeighborTableListRecord(" << neighborTableListRecord.ieeeAddress.toString() << ", ";
    debug.nospace() << "NWK address: " << ZigbeeUtils::convertUint16ToHexString(neighborTableListRecord.shortAddress) << ", ";
    debug.nospace() << "Extended PAN ID: " << neighborTableListRecord.extendedPanId << ", ";
    debug.nospace() << "Node type: " << neighborTableListRecord.nodeType << ", ";
    debug.nospace() << "RxOn: " << neighborTableListRecord.receiverOnWhenIdle << ", ";
    debug.nospace() << "Relationship: " << neighborTableListRecord.relationship << ", ";
    debug.nospace() << "Permit join: " << neighborTableListRecord.permitJoining << ", ";
    debug.nospace() << "Depth: " << neighborTableListRecord.depth << ", ";
    debug.nospace() << "LQI: " << neighborTableListRecord.lqi << ")";
    return debug;
}

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::RoutingTableListRecord &routingTableListRecord)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "RoutingTableListRecord(";
    debug.nospace() << "Destination address: " << ZigbeeUtils::convertUint16ToHexString(routingTableListRecord.destinationAddress) << ", ";
    debug.nospace() << "Next hop: " << routingTableListRecord.nextHopAddress << ", ";
    debug.nospace() << "Status: " << routingTableListRecord.status << ", ";
    debug.nospace() << "Memory constrained: " << routingTableListRecord.memoryConstrained << ", ";
    debug.nospace() << "Many-to-one: " << routingTableListRecord.manyToOne << ", ";
    debug.nospace() << "RRR: " << routingTableListRecord.routeRecordRequired << ")";
    return debug;
}
