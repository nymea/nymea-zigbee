#include "zigbeeclusterbasic.h"
#include "loggingcategory.h"

ZigbeeClusterBasic::ZigbeeClusterBasic(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, Zigbee::ClusterIdBasic, direction, parent)
{

}

void ZigbeeClusterBasic::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    if (hasAttribute(attribute.id())) {
        qCDebug(dcZigbeeCluster()) << this << "update attribute" << static_cast<Attribute>(attribute.id()) << attribute.dataType() << attribute.data();
        m_attributes[attribute.id()] = attribute;
        emit attributeChanged(attribute);
    } else {
        qCDebug(dcZigbeeCluster()) << this << "add attribute" << static_cast<Attribute>(attribute.id()) << attribute.dataType() << attribute.data();
        m_attributes.insert(attribute.id(), attribute);
        emit attributeChanged(attribute);
    }
}
