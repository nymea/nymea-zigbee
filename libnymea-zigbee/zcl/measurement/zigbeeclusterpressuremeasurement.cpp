// SPDX-License-Identifier: LGPL-3.0-or-later

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* nymea-zigbee
* Zigbee integration module for nymea
*
* Copyright (C) 2013 - 2024, nymea GmbH
* Copyright (C) 2024 - 2025, chargebyte austria GmbH
*
* This file is part of nymea-zigbee.
*
* nymea-zigbee is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3
* of the License, or (at your option) any later version.
*
* nymea-zigbee is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with nymea-zigbee. If not, see <https://www.gnu.org/licenses/>.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "zigbeeclusterpressuremeasurement.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"

ZigbeeClusterPressureMeasurement::ZigbeeClusterPressureMeasurement(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, ZigbeeCluster::Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdPressureMeasurement, direction, parent)
{

}

double ZigbeeClusterPressureMeasurement::pressure() const
{
    return m_pressure;
}

double ZigbeeClusterPressureMeasurement::pressureScaled() const
{
    return m_pressureScaled;
}

void ZigbeeClusterPressureMeasurement::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    // Parse the information for convinience
    if (attribute.id() == AttributeMeasuredValue) {
        bool valueOk = false;
        qint16 value = attribute.dataType().toInt16(&valueOk);
        if (valueOk) {
            if (value == static_cast<qint16>(0x8000)) {
                qCDebug(dcZigbeeCluster()) << m_node << m_endpoint << this << "received invalid measurement value. Not updating the attribute.";
                return;
            }

            m_pressure = value / 10.0;
            qCDebug(dcZigbeeCluster()) << "Pressure changed on" << m_node << m_endpoint << this << m_pressure << "kPa";
            emit pressureChanged(m_pressure);
        }
    } else if (attribute.id() == AttributeScaledValue) {
        bool valueOk = false;
        qint16 value = attribute.dataType().toInt16(&valueOk);
        if (valueOk) {
            if (value == static_cast<qint16>(0x8000)) {
                qCDebug(dcZigbeeCluster()) << m_node << m_endpoint << this << "received invalid measurement value. Not updating the attribute.";
                return;
            }

            m_pressureScaled = value / 10.0;
            qCDebug(dcZigbeeCluster()) << "Pressure scaled changed on" << m_node << m_endpoint << this << m_pressureScaled << "Pa";
            emit pressureScaledChanged(m_pressureScaled);
        }
    }
}
