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
