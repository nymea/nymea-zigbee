#include "zigbeeclusterelectricalmeasurement.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeCluster)

ZigbeeClusterElectricalMeasurement::ZigbeeClusterElectricalMeasurement(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent):
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdElectricalMeasurement, direction, parent)
{

}

quint16 ZigbeeClusterElectricalMeasurement::activePowerPhaseA() const
{
    return m_activePowerPhaseA;
}

void ZigbeeClusterElectricalMeasurement::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    switch (attribute.id()) {
    case AttributeACPhaseAMeasurementActivePower:
        qCDebug(dcZigbeeCluster) << "Active power changed" << attribute.dataType() << attribute.dataType().toInt16();
        m_activePowerPhaseA = attribute.dataType().toInt16();
        emit activePowerPhaseAChanged(m_activePowerPhaseA);
        break;
    }
}
