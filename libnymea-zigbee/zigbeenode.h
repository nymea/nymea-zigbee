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

#ifndef ZIGBEENODE_H
#define ZIGBEENODE_H

#include <QObject>
#include <QDateTime>

#include "zigbee.h"
#include "zigbeereply.h"
#include "zigbeeaddress.h"
#include "zigbeenodeendpoint.h"
#include "zdo/zigbeedeviceobject.h"
#include "zdo/zigbeedeviceprofile.h"
#include "zcl/general/zigbeeclusterbasic.h"

class ZigbeeNetwork;

class ZigbeeNode : public QObject
{
    Q_OBJECT

    friend class ZigbeeNetwork;
    friend class ZigbeeNetworkDatabase;

public:
    enum State {
        StateUninitialized,
        StateInitializing,
        StateInitialized
    };
    Q_ENUM(State)

    State state() const;

    // Note: For sleepy devices this indicates best effort.
    // If a device does not send any data within 6h, it will be assumed to no reachable
    bool reachable() const;

    QUuid networkUuid() const;

    ZigbeeDeviceObject *deviceObject() const;

    quint16 shortAddress() const;
    ZigbeeAddress extendedAddress() const;

    QList<ZigbeeNodeEndpoint *> endpoints() const;
    bool hasEndpoint(quint8 endpointId) const;
    ZigbeeNodeEndpoint *getEndpoint(quint8 endpointId) const;

    // Basic cluster infomation
    QString manufacturerName() const;
    QString modelName() const;
    QString version() const;

    quint8 lqi() const;
    QDateTime lastSeen() const;

    // Information from descriptors
    ZigbeeDeviceProfile::NodeDescriptor nodeDescriptor() const;
    bool nodeDescriptorAvailable() const;

    ZigbeeDeviceProfile::MacCapabilities macCapabilities() const;

    ZigbeeDeviceProfile::PowerDescriptor powerDescriptor() const;
    bool powerDescriptorAvailable() const;

    // Only available if fetched
    QList<ZigbeeDeviceProfile::BindingTableListRecord> bindingTableRecords() const;
    QList<ZigbeeDeviceProfile::NeighborTableListRecord> neighborTableRecords() const;
    QList<ZigbeeDeviceProfile::RoutingTableListRecord> routingTableRecords() const;

    // This method starts the node initialization phase (read descriptors and endpoints)
    void startInitialization();

    ZigbeeReply *readBindingTableEntries();
    ZigbeeReply *readLqiTableEntries();
    ZigbeeReply *readRoutingTableEntries();

    ZigbeeReply *addBinding(quint8 sourceEndpointId, quint16 clusterId, const ZigbeeAddress &destinationAddress, quint8 destinationEndpoint);
    ZigbeeReply *addBinding(quint8 sourceEndpointId, quint16 clusterId, quint16 destinationGroupAddress);
    ZigbeeReply *removeBinding(const ZigbeeDeviceProfile::BindingTableListRecord &binding);
    ZigbeeReply *removeAllBindings();

private:
    ZigbeeNode(ZigbeeNetwork *network, quint16 shortAddress, const ZigbeeAddress &extendedAddress, QObject *parent = nullptr);

    ZigbeeNetwork *m_network;
    quint16 m_shortAddress = 0;
    ZigbeeAddress m_extendedAddress;

    // Basic cluster infomation
    QString m_manufacturerName;
    QString m_modelName;
    QString m_version;

    ZigbeeDeviceObject *m_deviceObject = nullptr;
    QList<ZigbeeNodeEndpoint *> m_endpoints;
    bool m_reachable = false;
    State m_state = StateUninitialized;
    quint8 m_lqi = 0;
    QDateTime m_lastSeen;

    // Node information
    ZigbeeDeviceProfile::NodeDescriptor m_nodeDescriptor;
    ZigbeeDeviceProfile::MacCapabilities m_macCapabilities;
    ZigbeeDeviceProfile::PowerDescriptor m_powerDescriptor;
    bool m_nodeDescriptorAvailable = false;
    bool m_powerDescriptorAvailable = false;

    QList<ZigbeeDeviceProfile::BindingTableListRecord> m_bindingTableRecords;
    QHash<quint16, ZigbeeDeviceProfile::NeighborTableListRecord> m_neighborTableRecords;
    QHash<quint16, ZigbeeDeviceProfile::RoutingTableListRecord> m_routingTableRecords;
    ZigbeeDeviceProfile::BindingTable m_bindingTable;
    ZigbeeDeviceProfile::NeighborTable m_neighborTable; // Used internally to sync the table from the device in chunks
    ZigbeeDeviceProfile::RoutingTable m_routingTable; // Used internally to sync the table from the device in chunks

    void setState(State state);
    void setReachable(bool reachable);

    // Init methods
    int m_requestRetry = 0;
    int m_requestRetriesMax = 2;
    QList<quint8> m_uninitializedEndpoints;
    void initNodeDescriptor();
    void initPowerDescriptor();
    void initEndpoints();
    void initEndpoint(quint8 endpointId);

    void removeNextBinding(ZigbeeReply *reply);
    void readBindingTableChunk(ZigbeeReply *reply, quint8 startIndex);
    void readNeighborTableChunk(ZigbeeReply *reply, quint8 startIndex);
    void readRoutingTableChunk(ZigbeeReply *reply, quint8 startIndex);

    void setupEndpointInternal(ZigbeeNodeEndpoint *endpoint);

    // For convenience and having base information about the first endpoint
    void initBasicCluster();
    void readManufacturerName(ZigbeeClusterBasic *basicCluster);
    void readModelIdentifier(ZigbeeClusterBasic *basicCluster);
    void readSoftwareBuildId(ZigbeeClusterBasic *basicCluster);

    void handleDataIndication(const Zigbee::ApsdeDataIndication &indication);


signals:
    void nodeInitializationFailed();
    void stateChanged(State state);
    void shortAddressChanged(quint16 shortAddress);
    void lqiChanged(quint8 lqi);
    void lastSeenChanged(const QDateTime &lastSeen);
    void manufacturerNameChanged(const QString &manufacturerName);
    void modelNameChanged(const QString &modelName);
    void versionChanged(const QString &version);
    void reachableChanged(bool reachable);
    void bindingTableRecordsChanged();
    void neighborTableRecordsChanged();
    void routingTableRecordsChanged();
    void clusterAdded(ZigbeeCluster *cluster);
    void endpointClusterAttributeChanged(ZigbeeNodeEndpoint *endpoint, ZigbeeCluster *cluster, const ZigbeeClusterAttribute &attribute);

public slots:
    void handleZigbeeClusterLibraryIndication(const Zigbee::ApsdeDataIndication &indication);

};

QDebug operator<<(QDebug debug, ZigbeeNode *node);


#endif // ZIGBEENODE_H
