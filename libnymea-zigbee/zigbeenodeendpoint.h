#ifndef ZIGBEENODEENDPOINT_H
#define ZIGBEENODEENDPOINT_H

#include <QObject>

#include "zigbee.h"
#include "zigbeecluster.h"
class ZigbeeNodeEndpoint : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeNodeEndpoint(quint8 endpoint, QObject *parent = nullptr);

    quint8 endpoint() const;

    Zigbee::ZigbeeProfile profile() const;
    void setProfile(Zigbee::ZigbeeProfile profile);

    quint16 deviceId() const;
    void setDeviceId(quint16 deviceId);

    quint8 deviceVersion() const;
    void setDeviceVersion(quint8 deviceVersion);

    QList<ZigbeeCluster *> inputClusters() const;
    ZigbeeCluster *getInputCluster(Zigbee::ClusterId clusterId) const;
    bool hasInputCluster(Zigbee::ClusterId clusterId) const;

    QList<ZigbeeCluster *> outputClusters() const;
    ZigbeeCluster *getOutputCluster(Zigbee::ClusterId clusterId) const;
    bool hasOutputCluster(Zigbee::ClusterId clusterId) const;

private:
    quint8 m_endpoint = 0;
    Zigbee::ZigbeeProfile m_profile = Zigbee::ZigbeeProfileLightLink;
    quint16 m_deviceId = 0;
    quint8 m_deviceVersion = 0;

    QHash<Zigbee::ClusterId, ZigbeeCluster *> m_inputClusters;
    QHash<Zigbee::ClusterId, ZigbeeCluster *> m_outputClusters;

signals:

};

#endif // ZIGBEENODEENDPOINT_H
