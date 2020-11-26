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

private:
    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

};

#endif // ZIGBEECLUSTERANALOGINPUT_H
