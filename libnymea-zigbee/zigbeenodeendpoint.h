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

#ifndef ZIGBEENODEENDPOINT_H
#define ZIGBEENODEENDPOINT_H

#include <QObject>

#include "zigbee.h"
#include "zigbeeaddress.h"
#include "zigbeenetworkreply.h"

// Import all implemented cluster types
#include "zcl/zigbeecluster.h"

class ZigbeeNode;
class ZigbeeNetwork;

class ZigbeeNodeEndpoint : public QObject
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;
    friend class ZigbeeNetworkDatabase;

public:
    quint8 endpointId() const;

    ZigbeeNode *node() const;

    Zigbee::ZigbeeProfile profile() const;
    void setProfile(Zigbee::ZigbeeProfile profile);

    quint16 deviceId() const;
    void setDeviceId(quint16 deviceId);

    quint8 deviceVersion() const;
    void setDeviceVersion(quint8 deviceVersion);

    bool initialized() const;

    // Basic cluster information
    QString manufacturerName() const;
    QString modelIdentifier() const;
    QString softwareBuildId() const;

    // Server clusters
    QList<ZigbeeCluster *> inputClusters() const;
    ZigbeeCluster *getInputCluster(ZigbeeClusterLibrary::ClusterId clusterId) const;
    bool hasInputCluster(ZigbeeClusterLibrary::ClusterId clusterId) const;

    // Client clusters
    QList<ZigbeeCluster *> outputClusters() const;
    ZigbeeCluster *getOutputCluster(ZigbeeClusterLibrary::ClusterId clusterId) const;
    bool hasOutputCluster(ZigbeeClusterLibrary::ClusterId clusterId) const;

    // Convenience cast methods for getting a specific cluster object
    template<typename T>
    inline T* inputCluster(ZigbeeClusterLibrary::ClusterId clusterId)
    {
        if (!hasInputCluster(clusterId))
            return nullptr;

        return qobject_cast<T *>(getInputCluster(clusterId));
    }

    template<typename T>
    inline T* outputCluster(ZigbeeClusterLibrary::ClusterId clusterId)
    {
        if (!hasOutputCluster(clusterId))
            return nullptr;

        return qobject_cast<T *>(getOutputCluster(clusterId));
    }

private:
    explicit ZigbeeNodeEndpoint(ZigbeeNetwork *network, ZigbeeNode *node, quint8 endpointId, QObject *parent = nullptr);
    ~ZigbeeNodeEndpoint();

    ZigbeeNetwork *m_network = nullptr;
    ZigbeeNode *m_node = nullptr;
    quint8 m_endpointId = 0;
    Zigbee::ZigbeeProfile m_profile = Zigbee::ZigbeeProfileLightLink;
    quint16 m_deviceId = 0;
    quint8 m_deviceVersion = 0;
    bool m_initialized = false;

    QHash<ZigbeeClusterLibrary::ClusterId, ZigbeeCluster *> m_inputClusters;
    QHash<ZigbeeClusterLibrary::ClusterId, ZigbeeCluster *> m_outputClusters;

    QString m_manufacturerName;
    QString m_modelIdentifier;
    QString m_softwareBuildId;

    void setManufacturerName(const QString &manufacturerName);
    void setModelIdentifier(const QString &modelIdentifier);
    void setSoftwareBuildId(const QString &softwareBuildId);

    ZigbeeCluster *createCluster(ZigbeeClusterLibrary::ClusterId clusterId, ZigbeeCluster::Direction direction);

    void addInputCluster(ZigbeeCluster *cluster);
    void addOutputCluster(ZigbeeCluster *cluster);

    void handleZigbeeClusterLibraryIndication(const Zigbee::ApsdeDataIndication &indication);

signals:
    void inputClusterAdded(ZigbeeCluster *cluster);
    void outputClusterAdded(ZigbeeCluster *cluster);

    void clusterAttributeChanged(ZigbeeCluster *cluster, const ZigbeeClusterAttribute &attribute);

    void manufacturerNameChanged(const QString &manufacturerName);
    void modelIdentifierChanged(const QString &modelIdentifier);
    void softwareBuildIdChanged(const QString &softwareBuildId);
};

QDebug operator<<(QDebug debug, ZigbeeNodeEndpoint *endpoint);

#endif // ZIGBEENODEENDPOINT_H
