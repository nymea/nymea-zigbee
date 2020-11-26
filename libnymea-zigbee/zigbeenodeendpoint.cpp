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

#include "zigbeenodeendpoint.h"
#include "zigbeeutils.h"
#include "zigbeenode.h"
#include "loggingcategory.h"

quint8 ZigbeeNodeEndpoint::endpointId() const
{
    return m_endpointId;
}

ZigbeeNode *ZigbeeNodeEndpoint::node() const
{
    return m_node;
}

Zigbee::ZigbeeProfile ZigbeeNodeEndpoint::profile() const
{
    return m_profile;
}

void ZigbeeNodeEndpoint::setProfile(Zigbee::ZigbeeProfile profile)
{
    m_profile = profile;
}

quint16 ZigbeeNodeEndpoint::deviceId() const
{
    return m_deviceId;
}

void ZigbeeNodeEndpoint::setDeviceId(quint16 deviceId)
{
    m_deviceId = deviceId;
}

quint8 ZigbeeNodeEndpoint::deviceVersion() const
{
    return m_deviceVersion;
}

void ZigbeeNodeEndpoint::setDeviceVersion(quint8 deviceVersion)
{
    m_deviceVersion = deviceVersion;
}

bool ZigbeeNodeEndpoint::initialized() const
{
    return m_initialized;
}

QString ZigbeeNodeEndpoint::manufacturerName() const
{
    return m_manufacturerName;
}

QString ZigbeeNodeEndpoint::modelIdentifier() const
{
    return m_modelIdentifier;
}

QString ZigbeeNodeEndpoint::softwareBuildId() const
{
    return m_softwareBuildId;
}

QList<ZigbeeCluster *> ZigbeeNodeEndpoint::inputClusters() const
{
    return m_inputClusters.values();
}

ZigbeeCluster *ZigbeeNodeEndpoint::getInputCluster(ZigbeeClusterLibrary::ClusterId clusterId) const
{
    return m_inputClusters.value(clusterId);
}

bool ZigbeeNodeEndpoint::hasInputCluster(ZigbeeClusterLibrary::ClusterId clusterId) const
{
    return m_inputClusters.contains(clusterId);
}

QList<ZigbeeCluster *> ZigbeeNodeEndpoint::outputClusters() const
{
    return m_outputClusters.values();
}

ZigbeeCluster *ZigbeeNodeEndpoint::getOutputCluster(ZigbeeClusterLibrary::ClusterId clusterId) const
{
    return m_outputClusters.value(clusterId);
}

bool ZigbeeNodeEndpoint::hasOutputCluster(ZigbeeClusterLibrary::ClusterId clusterId) const
{
    return m_outputClusters.contains(clusterId);
}

ZigbeeNodeEndpoint::ZigbeeNodeEndpoint(ZigbeeNetwork *network, ZigbeeNode *node, quint8 endpointId, QObject *parent) :
    QObject(parent),
    m_network(network),
    m_node(node),
    m_endpointId(endpointId)
{
    qCDebug(dcZigbeeEndpoint()) << "Creating endpoint" << m_endpointId << "on" << m_node;
}

ZigbeeNodeEndpoint::~ZigbeeNodeEndpoint()
{
    qCDebug(dcZigbeeEndpoint()) << "Destroy endpoint" << m_endpointId << "on" << m_node;
}

void ZigbeeNodeEndpoint::setManufacturerName(const QString &manufacturerName)
{
    if (m_manufacturerName == manufacturerName)
        return;

    m_manufacturerName = manufacturerName;
    emit manufacturerNameChanged(m_manufacturerName);
}

void ZigbeeNodeEndpoint::setModelIdentifier(const QString &modelIdentifier)
{
    if (m_modelIdentifier == modelIdentifier)
        return;

    m_modelIdentifier = modelIdentifier;
    emit modelIdentifierChanged(m_modelIdentifier);
}

void ZigbeeNodeEndpoint::setSoftwareBuildId(const QString &softwareBuildId)
{
    if (m_softwareBuildId == softwareBuildId)
        return;

    m_softwareBuildId = softwareBuildId;
    emit softwareBuildIdChanged(m_softwareBuildId);
}

ZigbeeCluster *ZigbeeNodeEndpoint::createCluster(ZigbeeClusterLibrary::ClusterId clusterId, ZigbeeCluster::Direction direction)
{
    switch (clusterId) {
    // General
    case ZigbeeClusterLibrary::ClusterIdBasic:
        return new ZigbeeClusterBasic(m_network, m_node, this, direction, this);
    case ZigbeeClusterLibrary::ClusterIdPowerConfiguration:
        return new ZigbeeClusterPowerConfiguration(m_network, m_node, this, direction, this);
    case ZigbeeClusterLibrary::ClusterIdIdentify:
        return new ZigbeeClusterIdentify(m_network, m_node, this, direction, this);
    case ZigbeeClusterLibrary::ClusterIdOnOff:
        return new ZigbeeClusterOnOff(m_network, m_node, this, direction, this);
    case ZigbeeClusterLibrary::ClusterIdLevelControl:
        return new ZigbeeClusterLevelControl(m_network, m_node, this, direction, this);
    case ZigbeeClusterLibrary::ClusterIdGroups:
        return new ZigbeeClusterGroups(m_network, m_node, this, direction, this);
    case ZigbeeClusterLibrary::ClusterIdAnalogInput:
        return new ZigbeeClusterAnalogInput(m_network, m_node, this, direction, this);
    case ZigbeeClusterLibrary::ClusterIdAnalogOutput:
        return new ZigbeeClusterAnalogOutput(m_network, m_node, this, direction, this);
    case ZigbeeClusterLibrary::ClusterIdAnalogValue:
        return new ZigbeeClusterAnalogValue(m_network, m_node, this, direction, this);
    case ZigbeeClusterLibrary::ClusterIdBinaryInput:
        return new ZigbeeClusterBinaryInput(m_network, m_node, this, direction, this);
    case ZigbeeClusterLibrary::ClusterIdMultistateInput:
        return new ZigbeeClusterMultistateInput(m_network, m_node, this, direction, this);

        // Measurement
    case ZigbeeClusterLibrary::ClusterIdIlluminanceMeasurement:
        return new ZigbeeClusterIlluminanceMeasurment(m_network, m_node, this, direction, this);
    case ZigbeeClusterLibrary::ClusterIdTemperatureMeasurement:
        return new ZigbeeClusterTemperatureMeasurement(m_network, m_node, this, direction, this);
    case ZigbeeClusterLibrary::ClusterIdRelativeHumidityMeasurement:
        return new ZigbeeClusterRelativeHumidityMeasurement(m_network, m_node, this, direction, this);
    case ZigbeeClusterLibrary::ClusterIdOccupancySensing:
        return new ZigbeeClusterOccupancySensing(m_network, m_node, this, direction, this);
    case ZigbeeClusterLibrary::ClusterIdPressureMeasurement:
        return new ZigbeeClusterPressureMeasurement(m_network, m_node, this, direction, this);

        // Colsures
    case ZigbeeClusterLibrary::ClusterIdDoorLock:
        return new ZigbeeClusterDoorLock(m_network, m_node, this, direction, this);

        // Lighting
    case ZigbeeClusterLibrary::ClusterIdColorControl:
        return new ZigbeeClusterColorControl(m_network, m_node, this, direction, this);

        // Security
    case ZigbeeClusterLibrary::ClusterIdIasZone:
        return new ZigbeeClusterIasZone(m_network, m_node, this, direction, this);

        // OTA
    case ZigbeeClusterLibrary::ClusterIdOtaUpgrade:
        return new ZigbeeClusterOta(m_network, m_node, this, direction, this);

        // HVAC
    case ZigbeeClusterLibrary::ClusterIdThermostat:
        return new ZigbeeClusterThermostat(m_network, m_node, this, direction, this);

    default:
        // Return a default cluster since we have no special implementation for this cluster, allowing to use generic clusters operations
        return new ZigbeeCluster(m_network, m_node, this, clusterId, direction, this);
    }
}

void ZigbeeNodeEndpoint::addInputCluster(ZigbeeCluster *cluster)
{
    m_inputClusters.insert(cluster->clusterId(), cluster);
    connect(cluster, &ZigbeeCluster::attributeChanged, this, [this, cluster](const ZigbeeClusterAttribute &attribute){
        emit clusterAttributeChanged(cluster, attribute);
    });
    emit inputClusterAdded(cluster);
}

void ZigbeeNodeEndpoint::addOutputCluster(ZigbeeCluster *cluster)
{
    m_outputClusters.insert(cluster->clusterId(), cluster);
    connect(cluster, &ZigbeeCluster::attributeChanged, this, [this, cluster](const ZigbeeClusterAttribute &attribute){
        emit clusterAttributeChanged(cluster, attribute);
    });
    emit outputClusterAdded(cluster);
}

void ZigbeeNodeEndpoint::handleZigbeeClusterLibraryIndication(const Zigbee::ApsdeDataIndication &indication)
{
    ZigbeeClusterLibrary::Frame frame = ZigbeeClusterLibrary::parseFrameData(indication.asdu);
    qCDebug(dcZigbeeEndpoint()) << "Processing ZCL indication" << this << indication << frame;

    // Check which kind of cluster sent this inidication, server or client
    ZigbeeCluster *cluster = nullptr;
    switch (frame.header.frameControl.direction) {
    case ZigbeeClusterLibrary::DirectionClientToServer:
        // Get the output/client cluster this indication is coming from
        cluster = getOutputCluster(static_cast<ZigbeeClusterLibrary::ClusterId>(indication.clusterId));
        if (!cluster) {
            cluster = createCluster(static_cast<ZigbeeClusterLibrary::ClusterId>(indication.clusterId), ZigbeeCluster::Client);
            qCDebug(dcZigbeeEndpoint()) << "Received a ZCL indication for a client cluster which does not exist yet on" << m_node << this << "Creating" << cluster;

            addOutputCluster(cluster);
            if (m_initialized) {
                // Note: if the node has already been initialized and the cluster did not exist until now,
                // we need to store the new cluster in the database before updating the attribute. This is required
                // only for devices which are out of spec and do not list the new cluster in the simple descriptor.


            }
        }
        break;
    case ZigbeeClusterLibrary::DirectionServerToClient:
        // Get the input/server cluster this indication is coming from
        cluster = getInputCluster(static_cast<ZigbeeClusterLibrary::ClusterId>(indication.clusterId));
        if (!cluster) {
            cluster = createCluster(static_cast<ZigbeeClusterLibrary::ClusterId>(indication.clusterId), ZigbeeCluster::Server);
            qCDebug(dcZigbeeEndpoint()) << "Received a ZCL indication for a server cluster which does not exist yet on" << m_node << this << "Creating" << cluster;
            addInputCluster(cluster);
        }
        break;
    }

    cluster->processApsDataIndication(indication.asdu, frame);
}

QDebug operator<<(QDebug debug, ZigbeeNodeEndpoint *endpoint)
{
    debug.nospace().noquote() << "ZigbeeNodeEndpoint(" << ZigbeeUtils::convertByteToHexString(endpoint->endpointId());
    debug.nospace().noquote() << ", " << endpoint->profile();
    if (endpoint->profile() == Zigbee::ZigbeeProfileLightLink) {
        debug.nospace().noquote() << ", " << static_cast<Zigbee::LightLinkDevice>(endpoint->deviceId());
    } else if (endpoint->profile() == Zigbee::ZigbeeProfileHomeAutomation) {
        debug.nospace().noquote() << ", " << static_cast<Zigbee::HomeAutomationDevice>(endpoint->deviceId());
    } else if (endpoint->profile() == Zigbee::ZigbeeProfileGreenPower) {
        debug.nospace().noquote() << ", " << static_cast<Zigbee::GreenPowerDevice>(endpoint->deviceId());
    }

    debug.nospace().noquote() << ") ";
    return debug;
}
