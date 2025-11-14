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

#include "zigbeeclusteroccupancysensing.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"

ZigbeeClusterOccupancySensing::ZigbeeClusterOccupancySensing(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdOccupancySensing, direction, parent)
{

}

bool ZigbeeClusterOccupancySensing::occupied() const
{
    return m_occupied;
}

quint16 ZigbeeClusterOccupancySensing::pirOccupiedToUnoccupiedDelay() const
{
    return m_pirOccupiedToUnoccupiedDelay;
}

ZigbeeClusterReply *ZigbeeClusterOccupancySensing::setPirOccupiedToUnoccupiedDelay(quint16 pirOccupiedToUnoccupiedDelay)
{
    ZigbeeClusterLibrary::WriteAttributeRecord record;
    record.attributeId = AttributePirOccupiedToUnoccupiedDelay;
    record.dataType = Zigbee::Uint16;
    record.data = ZigbeeDataType(pirOccupiedToUnoccupiedDelay).data();
    return writeAttributes({record});
}

quint16 ZigbeeClusterOccupancySensing::pirUnoccupiedToOccupiedDelay() const
{
    return m_pirUnoccupiedToOccupiedDelay;
}

ZigbeeClusterReply *ZigbeeClusterOccupancySensing::setPirUnoccupiedToOccupiedDelay(quint16 pirUnoccupiedToOccupiedDelay)
{
    ZigbeeClusterLibrary::WriteAttributeRecord record;
    record.attributeId = AttributePirUnoccupiedToOccupiedDelay;
    record.dataType = Zigbee::Uint16;
    record.data = ZigbeeDataType(pirUnoccupiedToOccupiedDelay).data();
    return writeAttributes({record});
}

quint16 ZigbeeClusterOccupancySensing::pirUnoccupiedToOccupiedThreshold() const
{
    return m_pirUnoccupiedToOccupiedThreshold;
}

ZigbeeClusterReply *ZigbeeClusterOccupancySensing::setPirUnoccupiedToOccupiedThreshold(quint16 pirUnoccupiedToOccupiedThreshold)
{
    ZigbeeClusterLibrary::WriteAttributeRecord record;
    record.attributeId = AttributePirUnoccupiedToOccupiedThreshold;
    record.dataType = Zigbee::Uint16;
    record.data = ZigbeeDataType(pirUnoccupiedToOccupiedThreshold).data();
    return writeAttributes({record});
}

void ZigbeeClusterOccupancySensing::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    // Parse the information for convenience
    if (attribute.id() == AttributeOccupancy) {
        bool valueOk = false;
        bool value = attribute.dataType().toBool(&valueOk);
        if (valueOk) {
            m_occupied = value;
            qCDebug(dcZigbeeCluster()) << "Occupancy changed on" << m_node << m_endpoint << this << m_occupied;
            emit occupancyChanged(m_occupied);
        } else {
            qCWarning(dcZigbeeCluster()) << "Failed to convert value from attribute" << m_node << m_endpoint << this << attribute;
        }
    } else if (attribute.id() == AttributePirOccupiedToUnoccupiedDelay) {
        bool valueOk;
        quint16 value = attribute.dataType().toUInt16(&valueOk);
        if (valueOk) {
            m_pirOccupiedToUnoccupiedDelay = value;
            qCDebug(dcZigbeeCluster()) << "PirOccupiedToUnoccupiedDelay changed on" << m_node << m_endpoint << this << m_pirOccupiedToUnoccupiedDelay;
            emit pirOccupiedToUnoccupiedDelayChanged(m_pirOccupiedToUnoccupiedDelay);
        } else {
            qCWarning(dcZigbeeCluster()) << "Failed to convert value from attribute" << m_node << m_endpoint << this << attribute;
        }
    } else if (attribute.id() == AttributePirUnoccupiedToOccupiedDelay) {
        bool valueOk;
        quint16 value = attribute.dataType().toUInt16(&valueOk);
        if (valueOk) {
            m_pirUnoccupiedToOccupiedDelay = value;
            qCDebug(dcZigbeeCluster()) << "PirUnccupiedToOccupiedDelay changed on" << m_node << m_endpoint << this << m_pirOccupiedToUnoccupiedDelay;
            emit pirUnoccupiedToOccupiedDelayChanged(m_pirUnoccupiedToOccupiedDelay);
        } else {
            qCWarning(dcZigbeeCluster()) << "Failed to convert value from attribute" << m_node << m_endpoint << this << attribute;
        }
    } else if (attribute.id() == AttributePirUnoccupiedToOccupiedThreshold) {
        bool valueOk;
        quint16 value = attribute.dataType().toUInt16(&valueOk);
        if (valueOk) {
            m_pirUnoccupiedToOccupiedThreshold = value;
            qCDebug(dcZigbeeCluster()) << "PirUnoccupiedToOccupiedThreshold changed on" << m_node << m_endpoint << this << m_pirOccupiedToUnoccupiedDelay;
            emit pirUnoccupiedToOccupiedThresholdChanged(m_pirUnoccupiedToOccupiedThreshold);
        } else {
            qCWarning(dcZigbeeCluster()) << "Failed to convert value from attribute" << m_node << m_endpoint << this << attribute;
        }
    }
}
