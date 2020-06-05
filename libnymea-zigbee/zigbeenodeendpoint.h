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

#ifndef ZIGBEENODEENDPOINT_H
#define ZIGBEENODEENDPOINT_H

#include <QObject>

#include "zigbee.h"
#include "zigbeeaddress.h"
#include "zigbeenetworkreply.h"

// Import all implemented cluster types
#include "zcl/zigbeecluster.h"

#include "zcl/general/zigbeeclusterbasic.h"
#include "zcl/general/zigbeeclusteronoff.h"
#include "zcl/general/zigbeeclusteridentify.h"
#include "zcl/general/zigbeeclusterlevelcontrol.h"

#include "zcl/measurement/zigbeeclusteroccupancysensing.h"
#include "zcl/measurement/zigbeeclusterilluminancemeasurment.h"
#include "zcl/measurement/zigbeeclustertemperaturemeasurement.h"
#include "zcl/measurement/zigbeeclusterrelativehumiditymeasurement.h"

#include "zcl/lighting/zigbeeclustercolorcontrol.h"

#include "zcl/security/zigbeeclusteriaszone.h"

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
    ZigbeeCluster *getInputCluster(Zigbee::ClusterId clusterId) const;
    bool hasInputCluster(Zigbee::ClusterId clusterId) const;

    // Client clusters
    QList<ZigbeeCluster *> outputClusters() const;
    ZigbeeCluster *getOutputCluster(Zigbee::ClusterId clusterId) const;
    bool hasOutputCluster(Zigbee::ClusterId clusterId) const;

    // Convinience cast methods for getting a specific cluster object
    template<typename T>
    inline T* inputCluster(Zigbee::ClusterId clusterId)
    {
        if (!hasInputCluster(clusterId))
            return nullptr;

        return qobject_cast<T *>(getInputCluster(clusterId));
    }

    template<typename T>
    inline T* outputCluster(Zigbee::ClusterId clusterId)
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

    QHash<Zigbee::ClusterId, ZigbeeCluster *> m_inputClusters;
    QHash<Zigbee::ClusterId, ZigbeeCluster *> m_outputClusters;

    QString m_manufacturerName;
    QString m_modelIdentifier;
    QString m_softwareBuildId;

    void setManufacturerName(const QString &manufacturerName);
    void setModelIdentifier(const QString &modelIdentifier);
    void setSoftwareBuildId(const QString &softwareBuildId);

    ZigbeeCluster *createCluster(Zigbee::ClusterId clusterId, ZigbeeCluster::Direction direction);

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
