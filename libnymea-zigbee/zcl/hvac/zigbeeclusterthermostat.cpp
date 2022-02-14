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
