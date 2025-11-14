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

#include "zigbeeclusterilluminancemeasurement.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"

ZigbeeClusterIlluminanceMeasurement::ZigbeeClusterIlluminanceMeasurement(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdIlluminanceMeasurement, direction, parent)
{

}

quint16 ZigbeeClusterIlluminanceMeasurement::illuminance() const
{
    return m_illuminance;
}

void ZigbeeClusterIlluminanceMeasurement::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    // Parse the information for convenience
    if (attribute.id() == AttributeMeasuredValue) {
        bool valueOk = false;
        quint16 value = attribute.dataType().toUInt16(&valueOk);
        if (valueOk) {
            if (value == 0xffff) {
                qCDebug(dcZigbeeCluster()) << m_node << m_endpoint << this << "received invalid measurement value. Not updating the attribute.";
                return;
            }

            m_illuminance = value;
            qCDebug(dcZigbeeCluster()) << "Illuminance changed on" << m_node << m_endpoint << this << m_illuminance << "lux";
            emit illuminanceChanged(m_illuminance);
        }
    }
}
