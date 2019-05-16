#include "zigbeeutils.h"
#include "zigbeecluster.h"

ZigbeeCluster::ZigbeeCluster(Zigbee::ClusterId clusterId, QObject *parent) :
    QObject(parent),
    m_clusterId(clusterId)
{

}

Zigbee::ClusterId ZigbeeCluster::clusterId() const
{
    return m_clusterId;
}

QString ZigbeeCluster::clusterName() const
{
    return ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(m_clusterId));
}

QList<ZigbeeClusterAttribute> ZigbeeCluster::attributes() const
{
    return m_attributes.values();
}

bool ZigbeeCluster::hasAttribute(quint16 attributeId) const
{
    if (m_attributes.keys().isEmpty())
        return false;

    return m_attributes.keys().contains(attributeId);
}

ZigbeeClusterAttribute ZigbeeCluster::attribute(quint16 id)
{
    return m_attributes.value(id);
}

void ZigbeeCluster::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    if (hasAttribute(attribute.id())) {
        if (m_attributes.value(attribute.id()) != attribute) {
            m_attributes[attribute.id()] = attribute;
            emit attributeChanged(attribute);
        }
    } else {
        m_attributes.insert(attribute.id(), attribute);
        emit attributeChanged(attribute);
    }
}

QDebug operator<<(QDebug debug, ZigbeeCluster *cluster)
{
    debug.nospace().noquote() << "ZigbeeCluster("
                              << ZigbeeUtils::convertUint16ToHexString(static_cast<quint16>(cluster->clusterId())) << ", "
                              << cluster->clusterName() << ")";

    return debug.space();
}
