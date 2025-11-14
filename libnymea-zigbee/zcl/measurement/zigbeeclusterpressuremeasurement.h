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

#ifndef ZIGBEECLUSTERPRESSUREMEASUREMENT_H
#define ZIGBEECLUSTERPRESSUREMEASUREMENT_H

#include <QObject>
#include "zcl/zigbeecluster.h"
#include "zcl/zigbeeclusterreply.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterPressureMeasurement : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Attribute {
        AttributeMeasuredValue = 0x0000,
        AttributeMinMeasuredValue = 0x0001,
        AttributeMaxMeasuredValue = 0x0002,
        AttributeTolerance = 0x0003,

        // Extended pressure measurment information
        AttributeScaledValue = 0x0010,
        AttributeMinScaledValue = 0x0011,
        AttributeMaxScaledValue = 0x0012,
        AttributeScaledTolerance = 0x0013,
        AttributeScale = 0x0014
    };
    Q_ENUM(Attribute)

    explicit ZigbeeClusterPressureMeasurement(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    double pressure() const;
    double pressureScaled() const;

private:
    double m_pressure = 0;
    double m_pressureScaled = 0;
    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

signals:
    // kPa
    void pressureChanged(double pressure);

    // Pa
    void pressureScaledChanged(double pressure);


};

#endif // ZIGBEECLUSTERPRESSUREMEASUREMENT_H
