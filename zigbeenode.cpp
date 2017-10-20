#include "zigbeenode.h"

ZigbeeNode::ZigbeeNode(QObject *parent) : QObject(parent)
{

}

quint16 ZigbeeNode::shortAddress() const
{
    return m_shortAddress;
}

quint64 ZigbeeNode::extendedAddress() const
{
    return m_extendedAddress;
}

ZigbeeNode::NodeType ZigbeeNode::nodeType() const
{
    return m_nodeType;
}

void ZigbeeNode::setShortAddress(const quint16 &shortAddress)
{
    m_shortAddress = shortAddress;
}

void ZigbeeNode::setExtendedAddress(const quint64 &extendedAddress)
{
    m_extendedAddress = extendedAddress;
}
