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

#ifndef ZIGBEECLUSTEROCCUPANCYSENSING_H
#define ZIGBEECLUSTEROCCUPANCYSENSING_H

#include <QObject>

#include "zcl/zigbeecluster.h"
#include "zcl/zigbeeclusterreply.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterOccupancySensing : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Attribute {
        // Occupancy sensor information set
        AttributeOccupancy = 0x0000,
        AttributeOccupancySensorType = 0x0001,

        // PRI configuration set
        AttributePirOccupiedToUnoccupiedDelay = 0x0010,
        AttributePirUnoccupiedToOccupiedDelay = 0x0011,
        AttributePirUnoccupiedToOccupiedThreshold = 0x0012,

        // Ultrasonic configuration set
        AttributeUltrasonicOccupiedToUnoccupiedDelay = 0x0020,
        AttributeUltrasonicUnoccupiedToOccupiedDelay = 0x0021,
        AttributeUltrasonicUnoccupiedToOccupiedThreshold = 0x0022
    };
    Q_ENUM(Attribute)

    enum OccupancySensorType {
        OccupancySensorTypePir = 0x00,
        OccupancySensorTypeUltrasonic = 0x01,
        OccupancySensorTypePirAndUltrasonic = 0x02
    };
    Q_ENUM(OccupancySensorType)

    explicit ZigbeeClusterOccupancySensing(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    bool occupied() const;

    quint16 pirOccupiedToUnoccupiedDelay() const;
    ZigbeeClusterReply *setPirOccupiedToUnoccupiedDelay(quint16 pirOccupiedToUnoccupiedDelay);

    quint16 pirUnoccupiedToOccupiedDelay() const;
    ZigbeeClusterReply *setPirUnoccupiedToOccupiedDelay(quint16 pirUnoccupiedToOccupiedDelay);

    quint16 pirUnoccupiedToOccupiedThreshold() const;
    ZigbeeClusterReply *setPirUnoccupiedToOccupiedThreshold(quint16 pirUnoccupiedToOccupiedThreshold);

private:
    bool m_occupied = false;

    quint16 m_pirOccupiedToUnoccupiedDelay = 0;
    quint16 m_pirUnoccupiedToOccupiedDelay = 0;
    quint16 m_pirUnoccupiedToOccupiedThreshold = 0;

    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

signals:
    void occupancyChanged(bool occupied);
    void pirOccupiedToUnoccupiedDelayChanged(quint16 pirOccupiedToUnoccupiedDelay);
    void pirUnoccupiedToOccupiedDelayChanged(quint16 pirUnoccupiedToOccupiedDelay);
    void pirUnoccupiedToOccupiedThresholdChanged(quint16 pirUnoccupiedToOccupiedThreshold);

};

#endif // ZIGBEECLUSTEROCCUPANCYSENSING_H
