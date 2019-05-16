#ifndef ZIGBEECLUSTER_H
#define ZIGBEECLUSTER_H

#include <QObject>

#include "zigbee.h"
#include "zigbeeclusterattribute.h"

class ZigbeeCluster : public QObject
{
    Q_OBJECT

    friend class ZigbeeNode;

public:
    explicit ZigbeeCluster(Zigbee::ClusterId clusterId, QObject *parent = nullptr);

    Zigbee::ClusterId clusterId() const;
    QString clusterName() const;

    QList<ZigbeeClusterAttribute> attributes() const;
    bool hasAttribute(quint16 attributeId) const;

    ZigbeeClusterAttribute attribute(quint16 id);

private:
    Zigbee::ClusterId m_clusterId = Zigbee::ClusterIdUnknown;
    QHash<quint16, ZigbeeClusterAttribute> m_attributes;

protected:
    void setAttribute(const ZigbeeClusterAttribute &attribute);

signals:
    void attributeChanged(const ZigbeeClusterAttribute &attribute);

public slots:

};

QDebug operator<<(QDebug debug, ZigbeeCluster *cluster);

#endif // ZIGBEECLUSTER_H
