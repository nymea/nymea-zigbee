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
