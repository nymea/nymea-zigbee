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

#ifndef ZIGBEEDEVICEPROFILE_H
#define ZIGBEEDEVICEPROFILE_H

#include <QDebug>
#include <QObject>

#include "zigbee.h"

typedef struct ZigbeeDeviceProfileAdpu {
    quint8 sequenceNumber = 0;
    Zigbee::ZigbeeStatus status = Zigbee::ZigbeeStatusSuccess;
    quint16 addressOfInterest = 0;
    QByteArray payload;
} ZigbeeDeviceProfileAdpu;

class ZigbeeDeviceProfile
{
    Q_GADGET

public:
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

    static ZigbeeDeviceProfileAdpu parseAdpu(const QByteArray &adpu);
};

QDebug operator<<(QDebug debug, const ZigbeeDeviceProfileAdpu &deviceAdpu);

#endif // ZIGBEEDEVICEPROFILE_H
