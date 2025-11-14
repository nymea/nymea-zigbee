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

#ifndef ZIGBEECLUSTERBINARYOUTPUT_H
#define ZIGBEECLUSTERBINARYOUTPUT_H

#include <QObject>

#include "zcl/zigbeecluster.h"

class ZigbeeClusterBinaryOutput : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Attribute {
        AttributeActiveText = 0x0004,
        AttributeDescription = 0x001C,
        AttributeInactiveText = 0x002E,
        AttributeMinimumOffTime = 0x0042,
        AttributeMinimumOnTime = 0x0043,
        AttributeOutOfService = 0x0051,
        AttributePolarity = 0x0054,
        AttributePresentValue = 0x0055,
        AttributePriorityArray = 0x0057,
        AttributeReliability = 0x0067,
        AttributeRelinquishDefault = 0x0068,
        AttributeStatusFlags = 0x006F,
        AttributeApplicationType = 0x0100
    };
    Q_ENUM(Attribute)

    explicit ZigbeeClusterBinaryOutput(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

};

#endif // ZIGBEECLUSTERBINARYOUTPUT_H
