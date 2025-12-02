// SPDX-License-Identifier: LGPL-3.0-or-later

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* nymea-zigbee
* Zigbee integration module for nymea
*
* Copyright (C) 2013 - 2024, nymea GmbH
* Copyright (C) 2024 - 2025, chargebyte austria GmbH
*
* This file is part of nymea-zigbee.
*
* nymea-zigbee is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3
* of the License, or (at your option) any later version.
*
* nymea-zigbee is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with nymea-zigbee. If not, see <https://www.gnu.org/licenses/>.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef ZIGBEEDEVICEPROFILE_H
#define ZIGBEEDEVICEPROFILE_H

#include <QDebug>
#include <QObject>

#include "zigbee.h"
#include "zigbeeaddress.h"

class ZigbeeDeviceProfile
{
    Q_GADGET

public:
    enum Status {
        StatusSuccess = 0x00,
        StatusInvalidRequestType = 0x80,
        StatusDeviceNotFound = 0x81,
        StatusInvalidEndpoint = 0x82,
        StatusNotActive = 0x83,
        StatusNotSupported = 0x84,
        StatusTimeout = 0x85,
        StatusNoMatch = 0x86,
        StatusNoEntry = 0x88,
        StatusNoDescriptor = 0x89,
        StatusInsufficientSpace = 0x8a,
        StatusNotPermitted = 0x8b,
        StatusTableFull = 0x8c,
        StatusNotAuthorized = 0x8d,
        StatusDeviceBindingTableFull = 0x8e
    };
    Q_ENUM(Status)

    enum ZdoCommand {
        /* Requests */
        /*Device and service discovery */
        NetworkAddressRequest = 0x0000,
        IeeeAddressRequest = 0x0001,
        NodeDescriptorRequest = 0x0002,
        PowerDescriptorRequest = 0x0003,
        SimpleDescriptorRequest = 0x0004,
        ActiveEndpointsRequest = 0x0005,
        MatchDescriptorsRequest = 0x0006,
        ComplexDescriptorRequest = 0x0010,
        UserDescriptorRequest = 0x0011,
        DiscoveryCacheRequest = 0x0012,
        DeviceAnnounce = 0x0013,
        UserDescriptorSet = 0x0014,
        SystemServerDiscoveryRequest = 0x0015,
        DiscoveryStoreRequest = 0x0016,
        NodeDescriptorStoreRequest = 0x0017,
        PowerDescriptorStoreRequest = 0x0018,
        ActiveEndpointStoreRequest = 0x0019,
        SimpleDescriptorStoreRequest = 0x001A,
        RemoveNodeCacheRequest = 0x001B,
        FindNodeCacheRequest = 0x001C,
        ExtendedSimpleDescriptorRequest = 0x001D,
        ExtendedActiveEndpointRequest = 0x001E,
        ParentAnnounceRequest = 0x001F,

        /* Binding */
        EndDeviceBindRequest = 0x0020,
        BindRequest = 0x0021,
        UnbindRequest = 0x0022,
        BindRegisterRequest = 0x0023,
        ReplaceDeviceRequest = 0x0024,
        StoreBackupBindEntryRequest = 0x0025,
        RemoveBackupBindEntryRequest = 0x0026,
        BackupBindTableRequest = 0x0027,
        RecoverBindTableRequest = 0x0028,
        BackupSourceBindRequest = 0x0029,
        RecoverSourceBindRequest = 0x002A,

        /* Network management */
        MgmtNetworkDiscoveryRequest = 0x0030,
        MgmtLqiRequest = 0x0031,
        MgmtRoutingTableRequest = 0x0032,
        MgmtBindRequest = 0x0033,
        MgmtLeaveRequest = 0x0034,
        MgmtDirectJoinRequest = 0x0035,
        MgmtPermitJoinRequest = 0x0036,
        MgmtCacheRequest = 0x0037,
        MgmtNetworkUpdateRequest = 0x0038,

        /* Responses */
        /*Device and service discovery */
        NetworkAddressResponse = 0x8000,
        IeeeAddressResponse = 0x8001,
        NodeDescriptorResponse = 0x8002,
        PowerDescriptorResponse = 0x8003,
        SimpleDescriptorResponse = 0x8004,
        ActiveEndpointsResponse = 0x8005,
        MatchDescriptorsResponse = 0x8006,
        ComplexDescriptorResponse = 0x8010,
        UserDescriptorResponse = 0x8011,
        DiscoveryCacheResponse = 0x8012,
        UserDescriptorSetResponse = 0x8014,
        SystemServerDiscoveryResponse = 0x8015,
        DiscoveryStoreResponse = 0x8016,
        NodeDescriptorStoreResponse = 0x8017,
        PowerDescriptorStoreResponse = 0x8018,
        ActiveEndpointStoreResponse = 0x8019,
        SimpleDescriptorStoreResponse = 0x801A,
        RemoveNodeCacheResponse = 0x801B,
        FindNodeCacheResponse = 0x801C,
        ExtendedSimpleDescriptorResponse = 0x801D,
        ExtendedActiveEndpointResponse = 0x801E,
        ParentAnnounceRespone = 0x801F,

        /* Binding */
        EndDeviceBindResponse = 0x8020,
        BindResponse = 0x8021,
        UnbindResponse = 0x8022,
        BindRegisterResponse = 0x8023,
        ReplaceDeviceResponse = 0x8024,
        StoreBackupBindEntryResponse = 0x8025,
        RemoveBackupBindEntryResponse = 0x8026,
        BackupBindTableResponse = 0x8027,
        RecoverBindTableResponse = 0x8028,
        BackupSourceBindResponse = 0x8029,
        RecoverSourceBindResponse = 0x802A,

        /* Network management */
        MgmtNetworkDiscoveryResponse = 0x8030,
        MgmtLqiResponse = 0x8031,
        MgmtRoutingTableResponse = 0x8032,
        MgmtBindResponse = 0x8033,
        MgmtLeaveResponse = 0x8034,
        MgmtDirectJoinResponse = 0x8035,
        MgmtPermitJoinResponse = 0x8036,
        MgmtCacheResponse = 0x8037,
        MgmtNetworkUpdateResponse = 0x8038
    };
    Q_ENUM(ZdoCommand)

    // For sending
    typedef struct Frame {
        quint8 transactionSequenceNumber = 0;
        QByteArray payload;
    } Frame;

    // Receiving
    typedef struct Adpu {
        quint8 transactionSequenceNumber = 0;
        ZigbeeDeviceProfile::Status status = ZigbeeDeviceProfile::StatusSuccess;
        quint16 addressOfInterest = 0;
        QByteArray payload;
    } Adpu;

    static ZigbeeDeviceProfile::Adpu parseAdpu(const QByteArray &adpu);

    // Node information
    enum NodeType {
        NodeTypeCoordinator = 0,
        NodeTypeRouter = 1,
        NodeTypeEndDevice = 2
    };
    Q_ENUM(NodeType)

    enum FrequencyBand {
        FrequencyBand868Mhz,
        FrequencyBand902Mhz,
        FrequencyBand2400Mhz
    };
    Q_ENUM(FrequencyBand)

    enum DeviceType {
        DeviceTypeFullFunction,
        DeviceTypeReducedFunction
    };
    Q_ENUM(DeviceType)

    enum Relationship {
        RelationshipParent,
        RelationshipChild,
        RelationshipSibling,
        RelationshipNone,
        RelationshipPreviousChild
    };
    Q_ENUM(Relationship)

    enum RouteStatus {
        RouteStatusActive,
        RouteStatusDiscoveryUnderway,
        RouteStatusDiscoveryFailed,
        RouteStatusInactive,
        RouteStatusValidationUnderway
    };
    Q_ENUM(RouteStatus)

    enum PowerMode {
        PowerModeAlwaysOn,
        PowerModeOnPeriodically,
        PowerModeOnWhenStimulated
    };
    Q_ENUM(PowerMode)

    enum PowerSource {
        PowerSourcePermanentMainSupply,
        PowerSourceRecharchableBattery,
        PowerSourceDisposableBattery
    };
    Q_ENUM(PowerSource)

    enum PowerLevel {
        PowerLevelCriticalLow,
        PowerLevelLow,
        PowerLevelOk,
        PowerLevelFull
    };
    Q_ENUM(PowerLevel)

    typedef struct MacCapabilities {
        quint8 flag = 0x00; // For saving
        bool alternatePanCoordinator = false;
        DeviceType deviceType = DeviceTypeReducedFunction;
        bool powerSourceFlagMainPower = false;
        bool receiverOnWhenIdle = false;
        bool securityCapability = false;
        bool allocateAddress = false;
    } MacCapabilities;

    typedef struct DescriptorCapabilities {
        quint8 descriptorCapabilitiesFlag = 0x00; // For saving
        bool extendedActiveEndpointListAvailable = false;
        bool extendedSimpleDescriptorListAvailable = false;
    } DescriptorCapabilities;

    typedef struct ServerMask {
        quint16 serverMaskFlag = 0x0000; // For saving
        bool primaryTrustCenter = false;
        bool backupTrustCenter = false;
        bool primaryBindingCache = false;
        bool backupBindingCache = false;
        bool primaryDiscoveryCache = false;
        bool backupDiscoveryCache = false;
        bool networkManager = false;
        quint8 stackComplianceVersion = 0;
    } ServerMask;

    typedef struct NodeDescriptor {
        QByteArray descriptorRawData; // For saving
        NodeType nodeType = NodeTypeEndDevice;
        bool complexDescriptorAvailable = false;
        bool userDescriptorAvailable = false;
        FrequencyBand frequencyBand = FrequencyBand2400Mhz;
        MacCapabilities macCapabilities;
        quint16 manufacturerCode = 0;
        quint8 maximumBufferSize = 0;
        quint16 maximumRxSize = 0;
        ServerMask serverMask;
        quint16 maximumTxSize = 0;
        DescriptorCapabilities descriptorCapabilities;
    } NodeDescriptor;

    typedef struct PowerDescriptor {
        quint16 powerDescriptoFlag = 0x0000;
        PowerMode powerMode = PowerModeAlwaysOn;
        QList<PowerSource> availablePowerSources;
        PowerSource powerSource = PowerSourcePermanentMainSupply;
        PowerLevel powerLevel = PowerLevelFull;
    } PowerDescriptor;

    typedef struct BindingTableListRecord {
        ZigbeeAddress sourceAddress;
        quint8 sourceEndpoint;
        quint16 clusterId;
        Zigbee::DestinationAddressMode destinationAddressMode;
        quint16 destinationShortAddress;
        ZigbeeAddress destinationIeeeAddress;
        quint8 destinationEndpoint;
    } BindingTableListRecord;

    typedef struct BindingTable {
        Status status;
        quint8 tableSize;
        quint8 startIndex;
        QList<BindingTableListRecord> records;
    } BindingTable;

    typedef struct NeighborTableListRecord {
        quint64 extendedPanId;
        ZigbeeAddress ieeeAddress;
        quint16 shortAddress;
        NodeType nodeType;
        bool receiverOnWhenIdle;
        Relationship relationship;
        bool permitJoining;
        quint8 depth;
        quint8 lqi;
    } NeighborTableListRecord;

    typedef struct NeighborTable {
        Status status;
        quint8 tableSize;
        quint8 startIndex;
        QList<NeighborTableListRecord> records;
    } NeighborTable;

    typedef struct RoutingTableListRecord {
        quint16 destinationAddress;
        RouteStatus status;
        bool memoryConstrained;
        bool manyToOne;
        bool routeRecordRequired;
        quint16 nextHopAddress;
    } RoutingTableListRecord;

    typedef struct RoutingTable{
        Status status;
        quint8 tableSize;
        quint8 startIndex;
        QList<RoutingTableListRecord> records;
    } RoutingTable;

    static NodeDescriptor parseNodeDescriptor(const QByteArray &payload);
    static MacCapabilities parseMacCapabilities(quint8 macCapabilitiesFlag);
    static ServerMask parseServerMask(quint16 serverMaskFlag);
    static DescriptorCapabilities parseDescriptorCapabilities(quint8 descriptorCapabilitiesFlag);
    static PowerDescriptor parsePowerDescriptor(quint16 powerDescriptorFlag);
    static BindingTable parseBindingTable(const QByteArray &payload);
    static NeighborTable parseNeighborTable(const QByteArray &payload);
    static RoutingTable parseRoutingTable(const QByteArray &payload);
};

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::Adpu &deviceAdpu);
QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::NodeDescriptor &nodeDescriptor);
QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::MacCapabilities &macCapabilities);
QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::ServerMask &serverMask);
QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::DescriptorCapabilities &descriptorCapabilities);
QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::PowerDescriptor &powerDescriptor);
QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::BindingTableListRecord &bindingTableListRecord);
QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::NeighborTableListRecord &neighborTableListRecord);
QDebug operator<<(QDebug debug, const ZigbeeDeviceProfile::RoutingTableListRecord &routingTableListRecord);

#endif // ZIGBEEDEVICEPROFILE_H
