/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea-zigbee.
* This project including source code and documentation is protected by copyright law, and
* remains the property of nymea GmbH. All rights, including reproduction, publication,
* editing and translation, are reserved. The use of this project is subject to the terms of a
* license agreement to be concluded with nymea GmbH in accordance with the terms
* of use of nymea GmbH, available under https://nymea.io/license
*
* GNU Lesser General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the terms of the GNU
* Lesser General Public License as published by the Free Software Foundation; version 3.
* this project is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License along with this project.
* If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under contact@nymea.io
* or see our FAQ/Licensing Information on https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
