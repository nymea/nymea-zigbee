#include "zigbeenodeendpoint.h"

ZigbeeNodeEndpoint::ZigbeeNodeEndpoint(quint8 endpoint, QObject *parent) :
    QObject(parent),
    m_endpoint(endpoint)
{

}

quint8 ZigbeeNodeEndpoint::endpoint() const
{
    return m_endpoint;
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

ZigbeeCluster *ZigbeeNodeEndpoint::getOutputCluster(Zigbee::ClusterId clusterId) const
{
    return m_outputClusters.value(clusterId);
}
