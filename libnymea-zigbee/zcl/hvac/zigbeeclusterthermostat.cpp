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

#include "zigbeeclusterthermostat.h"

#include "loggingcategory.h"
#include "zigbeeutils.h"


ZigbeeClusterThermostat::ZigbeeClusterThermostat(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, ZigbeeCluster::Direction direction, QObject *parent):
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdThermostat, direction, parent)
{
}

qint16 ZigbeeClusterThermostat::localTemperature() const
{
    return m_attributes.value(AttributeLocalTemperature).dataType().toInt16();
}

qint16 ZigbeeClusterThermostat::occupiedCoolingSetpoint() const
{
    return m_attributes.value(AttributeOccupiedCoolingSetpoint).dataType().toInt16();
}

qint16 ZigbeeClusterThermostat::occupiedHeatingSetpoint() const
{
    return m_attributes.value(AttributeOccupiedHeatingSetpoint).dataType().toInt16();
}

ZigbeeClusterReply *ZigbeeClusterThermostat::setOccupiedHeatingSetpoint(qint16 occupiedHeatingSetpoint)
{
    ZigbeeDataType dataType(occupiedHeatingSetpoint);
    QList<ZigbeeClusterLibrary::WriteAttributeRecord> attributes;
    ZigbeeClusterLibrary::WriteAttributeRecord attribute;
    attribute.attributeId = ZigbeeClusterThermostat::AttributeOccupiedHeatingSetpoint;
    attribute.dataType = dataType.dataType();
    attribute.data = dataType.data();
    attributes.append(attribute);

    return this->writeAttributes(attributes);
}

ZigbeeClusterReply *ZigbeeClusterThermostat::setOccupiedCoolingSetpoint(qint16 occupiedCoolingSetpoint)
{
    ZigbeeDataType dataType(occupiedCoolingSetpoint);
    QList<ZigbeeClusterLibrary::WriteAttributeRecord> attributes;
    ZigbeeClusterLibrary::WriteAttributeRecord attribute;
    attribute.attributeId = ZigbeeClusterThermostat::AttributeOccupiedCoolingSetpoint;
    attribute.dataType = dataType.dataType();
    attribute.data = dataType.data();
    attributes.append(attribute);

    return this->writeAttributes(attributes);
}

void ZigbeeClusterThermostat::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    switch (attribute.id()) {
    case AttributeLocalTemperature:
        emit localTemperatureChanged(localTemperature());
        break;
    case AttributeOccupiedCoolingSetpoint:
        emit occupiedCoolingSetpointChanged(occupiedCoolingSetpoint());
        break;
    case AttributeOccupiedHeatingSetpoint:
        emit occupiedHeatingSetpointChanged(occupiedHeatingSetpoint());
        break;
    }
}
