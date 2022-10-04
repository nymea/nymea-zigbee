/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2022, nymea GmbH
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

#include "zigbeeclusterfancontrol.h"

#include "loggingcategory.h"
#include "zigbeeutils.h"


ZigbeeClusterFanControl::ZigbeeClusterFanControl(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, ZigbeeCluster::Direction direction, QObject *parent):
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdFanControl, direction, parent)
{
}

ZigbeeClusterFanControl::FanMode ZigbeeClusterFanControl::fanMode() const
{
    return static_cast<FanMode>(m_attributes.value(AttributeFanMode).dataType().toUInt8());
}

ZigbeeClusterFanControl::FanModeSequence ZigbeeClusterFanControl::fanModeSequence() const
{
    return static_cast<FanModeSequence>(m_attributes.value(AttributeFanModeSequence).dataType().toInt16());
}

ZigbeeClusterReply *ZigbeeClusterFanControl::setFanMode(ZigbeeClusterFanControl::FanMode fanMode)
{
    ZigbeeDataType dataType(static_cast<quint8>(fanMode));
    ZigbeeClusterLibrary::WriteAttributeRecord attribute;
    attribute.attributeId = ZigbeeClusterFanControl::AttributeFanMode;
    attribute.dataType = Zigbee::Enum8;
    attribute.data = dataType.data();

    return this->writeAttributes({attribute});
}

ZigbeeClusterReply *ZigbeeClusterFanControl::setFanModeSequence(ZigbeeClusterFanControl::FanModeSequence fanModeSequence)
{
    ZigbeeDataType dataType(static_cast<quint8>(fanModeSequence));
    ZigbeeClusterLibrary::WriteAttributeRecord attribute;
    attribute.attributeId = ZigbeeClusterFanControl::AttributeFanModeSequence;
    attribute.dataType = Zigbee::Enum8;
    attribute.data = dataType.data();

    return this->writeAttributes({attribute});
}

void ZigbeeClusterFanControl::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    switch (attribute.id()) {
    case AttributeFanMode:
        emit fanModeChanged(fanMode());
        break;
    case AttributeFanModeSequence:
        emit fanModeSequenceChanged(fanModeSequence());
        break;
    }
}
