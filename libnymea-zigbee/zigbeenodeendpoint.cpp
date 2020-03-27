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

ZigbeeCluster *ZigbeeNodeEndpoint::getInputCluster(Zigbee::ClusterId clusterId) const
{
    return m_inputClusters.value(clusterId);
}

bool ZigbeeNodeEndpoint::hasInputCluster(Zigbee::ClusterId clusterId) const
{
    return m_inputClusters.keys().contains(clusterId);
}

QList<ZigbeeCluster *> ZigbeeNodeEndpoint::outputClusters() const
{
    return m_outputClusters.values();
}

bool ZigbeeNodeEndpoint::hasOutputCluster(Zigbee::ClusterId clusterId) const
{
    return m_outputClusters.keys().contains(clusterId);
}

ZigbeeNodeEndpoint::ZigbeeNodeEndpoint(ZigbeeNode *node, quint8 endpointId, QObject *parent) :
    QObject(parent),
    m_node(node),
    m_endpointId(endpointId)
{

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

void ZigbeeNodeEndpoint::addInputCluster(ZigbeeCluster *cluster)
{
    m_inputClusters.insert(cluster->clusterId(), cluster);
}

void ZigbeeNodeEndpoint::addOutputCluster(ZigbeeCluster *cluster)
{
    m_outputClusters.insert(cluster->clusterId(), cluster);
}

ZigbeeNetworkReply *ZigbeeNodeEndpoint::createNetworkReply()
{
    return new ZigbeeNetworkReply(this);
}

void ZigbeeNodeEndpoint::finishNetworkReply(ZigbeeNetworkReply *reply, ZigbeeNetworkReply::Error error, Zigbee::ZigbeeStatus zigbeeStatus)
{
    reply->m_error = error;
    reply->m_zigbeeStatus = zigbeeStatus;
    reply->finished();
}

ZigbeeCluster *ZigbeeNodeEndpoint::getOutputCluster(Zigbee::ClusterId clusterId) const
{
    return m_outputClusters.value(clusterId);
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

    debug.nospace().noquote() << ")";
    return debug.space();
}
