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

    quint8 lqi() const;
    QDateTime lastSeen() const;

    // Information from descriptors
    ZigbeeDeviceProfile::NodeDescriptor nodeDescriptor() const;
    ZigbeeDeviceProfile::MacCapabilities macCapabilities() const;
    ZigbeeDeviceProfile::PowerDescriptor powerDescriptor() const;

    // Information from node power descriptor
    ZigbeeDeviceProfile::PowerMode powerMode() const;
    ZigbeeDeviceProfile::PowerSource powerSource() const;
    QList<ZigbeeDeviceProfile::PowerSource> availablePowerSources() const;
    ZigbeeDeviceProfile::PowerLevel powerLevel() const;

    // Only available if fetched
    QList<ZigbeeDeviceProfile::BindingTableListRecord> bindingTableRecords() const;

    // This method starts the node initialization phase (read descriptors and endpoints)
    void startInitialization();

    ZigbeeReply *removeAllBindings();
    ZigbeeReply *readBindingTableEntries();

private:
    ZigbeeNode(ZigbeeNetwork *network, quint16 shortAddress, const ZigbeeAddress &extendedAddress, QObject *parent = nullptr);

    ZigbeeNetwork *m_network;
    quint16 m_shortAddress = 0;
    ZigbeeAddress m_extendedAddress;

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

    QList<ZigbeeDeviceProfile::BindingTableListRecord> m_bindingTableRecords;

    void setState(State state);
    void setReachable(bool reachable);

    // Init methods
    int m_requestRetry = 0;
    QList<quint8> m_uninitializedEndpoints;
    void initNodeDescriptor();
    void initPowerDescriptor();
    void initEndpoints();
    void initEndpoint(quint8 endpointId);

    void removeNextBinding(ZigbeeReply *reply);

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
    void lqiChanged(quint8 lqi);
    void lastSeenChanged(const QDateTime &lastSeen);
    void reachableChanged(bool reachable);
    void bindingTableRecordsChanged();
    void clusterAdded(ZigbeeCluster *cluster);
    void endpointClusterAttributeChanged(ZigbeeNodeEndpoint *endpoint, ZigbeeCluster *cluster, const ZigbeeClusterAttribute &attribute);

public slots:
    void handleZigbeeClusterLibraryIndication(const Zigbee::ApsdeDataIndication &indication);

};

QDebug operator<<(QDebug debug, ZigbeeNode *node);


#endif // ZIGBEENODE_H
