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
