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

#ifndef ZIGBEECLUSTERANALOGINPUT_H
#define ZIGBEECLUSTERANALOGINPUT_H

#include <QObject>

#include "zcl/zigbeecluster.h"

class ZigbeeClusterAnalogInput : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Attribute {
        AttributeDescription = 0x001C,
        AttributeMaxPresentValue = 0x0041,
        AttributeMinPresentValue = 0x0045,
        AttributeOutOfService = 0x0051,
        AttributePresentValue = 0x0055,
        AttributeReliability = 0x0067,
        AttributeResolution = 0x006A,
        AttributeStatusFlags = 0x006F,
        AttributeEngineeringUnits = 0x0075,
        AttributeApplicationType = 0x0100
    };
    Q_ENUM(Attribute)

    explicit ZigbeeClusterAnalogInput(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    bool outOfService() const;
    float presentValue() const;

signals:
    void outOfServiceChanged(bool outOfService);
    void presentValueChanged(float presentValue);

private:
    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

    bool m_outOfService = false;
    float m_presentValue = 0;
};

#endif // ZIGBEECLUSTERANALOGINPUT_H
