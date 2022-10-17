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

#ifndef ZIGBEECLUSTERFANCONTROL_H
#define ZIGBEECLUSTERFANCONTROL_H

#include "zcl/zigbeecluster.h"

class ZigbeeClusterFanControl : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:

    enum Attribute {
        AttributeFanMode = 0x0000,
        AttributeFanModeSequence = 0x0001,
    };
    Q_ENUM(Attribute)

    enum FanMode {
        FanModeOff = 0x00,
        FanModeLow = 0x01,
        FanModeMedium = 0x02,
        FanModeHigh = 0x03,
        FanModeOn = 0x04,
        FanModeAuto = 0x05,
        FanModeSmart = 0x06
    };
    Q_ENUM(FanMode)

    enum FanModeSequence {
        FanModeSequenceLowMedHigh = 0x00,
        FanModeSequenceLowHigh = 0x01,
        FanModeSequenceLowMedHighAuto = 0x02,
        FanModeSequenceLowHighAuto = 0x03,
        FanModeSequenceOnAuto = 0x04
    };
    Q_ENUM(FanModeSequence)

    explicit ZigbeeClusterFanControl(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    FanMode fanMode() const;
    FanModeSequence fanModeSequence() const;

    ZigbeeClusterReply *setFanMode(FanMode fanMode);
    ZigbeeClusterReply *setFanModeSequence(FanModeSequence fanModeSequence);

signals:
    void fanModeChanged(FanMode fanMode);
    void fanModeSequenceChanged(FanModeSequence fanModeSequence);

private:
    void setAttribute(const ZigbeeClusterAttribute &attribute) override;
};

#endif // ZIGBEECLUSTERFANCONTROL_H
