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

#ifndef ZIGBEECLUSTERPOWERCONFIGURATION_H
#define ZIGBEECLUSTERPOWERCONFIGURATION_H

#include <QObject>

#include "zcl/zigbeecluster.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterPowerConfiguration : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Attribute {
        // Mains information
        AttributeMainsVoltage = 0x0000,
        AttributeMainsFrequency = 0x0001,

        // Mains settings
        AttributeMainsAlarmMask = 0x0010,
        AttributeMainsVoltageMinThreshold = 0x0011,
        AttributeMainsVoltageMaxThreshold = 0x0012,
        AttributeMainsVoltageDwellTripPoint = 0x0013,

        // Battery information
        AttributeBatteryVoltage = 0x0020,
        AttributeBatteryPercentageRemaining = 0x0021,

        // Battery settings
        AttributeBatteryManufacturer = 0x0030,
        AttributeBatterySize = 0x0031,
        AttributeBatteryAHrRating = 0x0032,
        AttributeBatteryQuality = 0x0033,
        AttributeBatteryRatedVoltage = 0x0034,
        AttributeBatteryAlarmMask = 0x0035,
        AttributeBatteryVoltageMinThreshold = 0x0036,
        AttributeBatteryVoltageThreshold1 = 0x0037,
        AttributeBatteryVoltageThreshold2 = 0x0038,
        AttributeBatteryVoltageThreshold3 = 0x0039,
        AttributeBatteryPercentageMinThreshold = 0x003a,
        AttributeBatteryPercentageThreshold1 = 0x003b,
        AttributeBatteryPercentageThreshold2 = 0x003c,
        AttributeBatteryPercentageThreshold3 = 0x003d,
        AttributeBatteryAlarmState = 0x003e
    };
    Q_ENUM(Attribute)

    enum MainsAlarm {
        MainsAlarmNone = 0x00,
        MainsAlarmMainVoltageToLow = 0x01,
        MainsAlarmMainVoltageToHigh = 0x02,
        MainsAlarmMainPowerSupplyLost = 0x04
    };
    Q_ENUM(MainsAlarm)
    Q_DECLARE_FLAGS(MainsAlarmMask, MainsAlarm)

    enum BatteryAlarm {
        BatteryAlarmNone = 0x00,
        BatteryAlarmTooLowToOperate = 0x01,
        BatteryAlarm1 = 0x02,
        BatteryAlarm2 = 0x04,
        BatteryAlarm3 = 0x08
    };
    Q_ENUM(BatteryAlarm)
    Q_DECLARE_FLAGS(BatteryAlarmMask, BatteryAlarm)

    explicit ZigbeeClusterPowerConfiguration(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    double batteryPercentage() const;
    double batteryVoltage() const;
    BatteryAlarmMask batteryAlarmState() const;

private:
    double m_batteryPercentage = 0;
    double m_batteryVoltage = 0;
    BatteryAlarmMask m_batteryAlarmState = BatteryAlarmNone;

    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

signals:
    void batteryPercentageChanged(double percentage);
    void batteryVoltageChanged(double voltage);
    void batteryAlarmStateChanged(ZigbeeClusterPowerConfiguration::BatteryAlarmMask alarmState);

};

Q_DECLARE_OPERATORS_FOR_FLAGS(ZigbeeClusterPowerConfiguration::MainsAlarmMask)
Q_DECLARE_OPERATORS_FOR_FLAGS(ZigbeeClusterPowerConfiguration::BatteryAlarmMask)


#endif // ZIGBEECLUSTERPOWERCONFIGURATION_H
