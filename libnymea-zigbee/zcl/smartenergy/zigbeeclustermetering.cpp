#include "zigbeeclustermetering.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeCluster)

ZigbeeClusterMetering::ZigbeeClusterMetering(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent):
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdMetering, direction, parent)
{

}

quint64 ZigbeeClusterMetering::currentSummationDelivered() const
{
    return m_currentSummationDelivered;
}

qint32 ZigbeeClusterMetering::instantaneousDemand() const
{
    return m_instantaneousDemand;
}

quint32 ZigbeeClusterMetering::multiplier() const
{
    return m_multiplier;
}

quint32 ZigbeeClusterMetering::divisor() const
{
    return m_divisor;
}

ZigbeeClusterReply *ZigbeeClusterMetering::readFormatting()
{
    ZigbeeClusterReply *readDivisorReply = readAttributes({ZigbeeClusterMetering::AttributeDivisor, ZigbeeClusterMetering::AttributeMultiplier});
    connect(readDivisorReply, &ZigbeeClusterReply::finished, this, [=](){
        if (readDivisorReply->error() != ZigbeeClusterReply::ErrorNoError) {
            qCWarning(dcZigbeeCluster()) << "Failed to read formatting." << readDivisorReply->error();
            return;
        }
    });
    return readDivisorReply;
}

void ZigbeeClusterMetering::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    switch (attribute.id()) {
    case AttributeCurrentSummationDelivered:
        m_currentSummationDelivered = attribute.dataType().toUInt64();
        emit currentSummationDeliveredChanged(m_currentSummationDelivered);
        break;
    case AttributeInstantaneousDemand:
        m_instantaneousDemand = attribute.dataType().toInt32();
        emit instantaneousDemandChanged(m_instantaneousDemand);
        break;
    case AttributeMultiplier:
        m_multiplier = attribute.dataType().toUInt32();
        break;
    case AttributeDivisor:
        m_divisor = attribute.dataType().toUInt32();
        break;
    default:
        qCWarning(dcZigbeeCluster()) << "Unhandled attribute change:" << attribute;
    }
}
