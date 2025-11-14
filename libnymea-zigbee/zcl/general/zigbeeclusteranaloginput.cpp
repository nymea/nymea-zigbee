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

#include "zigbeeclusteranaloginput.h"
#include "loggingcategory.h"

ZigbeeClusterAnalogInput::ZigbeeClusterAnalogInput(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdAnalogInput, direction, parent)
{

}

bool ZigbeeClusterAnalogInput::outOfService() const
{
    return m_outOfService;
}

float ZigbeeClusterAnalogInput::presentValue() const
{
    return m_presentValue;
}

void ZigbeeClusterAnalogInput::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    switch (attribute.id()) {
    case AttributeOutOfService:
        m_outOfService = attribute.dataType().toBool();
        emit outOfServiceChanged(m_outOfService);
        break;
    case AttributePresentValue:
        m_presentValue = attribute.dataType().toFloat();
        qCDebug(dcZigbeeCluster()) << "Present value changed:" << m_presentValue;
        emit presentValueChanged(m_presentValue);
        break;
    }
}
