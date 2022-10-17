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
