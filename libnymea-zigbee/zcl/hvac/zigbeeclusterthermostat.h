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

#ifndef ZIGBEECLUSTERTHERMOSTAT_H
#define ZIGBEECLUSTERTHERMOSTAT_H

#include "zcl/zigbeecluster.h"

class ZigbeeClusterThermostat : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:

    enum Attribute {
        AttributeLocalTemperature = 0x0000,
        AttributeOutdoorTemperature = 0x0001,
        AttributeOccupancy = 0x0002,
        AttributeAbsMinHeatSetpointLimit = 0x0003,
        AttributeAbsMaxHeatSetpointLimit = 0x0004,
        AttributeAbsMinCoolSetpointLimit = 0x0005,
        AttributeAbsMaxCoolSetpointLimit = 0x0006,
        AttributePICoolingDemand = 0x0007,
        AttributePIHeatingDemand = 0x0008,
        AttributeHVACSystemTypeConfiguration = 0x0009,

        AttributeLocalTemperatureCalibration = 0x0010,
        AttributeOccupiedCoolingSetpoint = 0x0011,
        AttributeOccupiedHeatingSetpoint = 0x0012,
        AttributeUnoccupiedCoolingSetpoint = 0x0013,
        AttributeUnoccupiedHeatingSetpoint = 0x0014,
        AttributeMinHeatSetpointLimit = 0x0015,
        AttributeMaxHeatSetpointLimit = 0x0016,
        AttributeMinCoolSetpointLimit = 0x0017,
        AttributeMaxCoolSetpointLimit = 0x0018,
        AttributeMinSetpointDeadBand = 0x0019,
        AttributeRemoteSensing = 0x001a,
        AttributeControlSequenceOfOperation = 0x001b,
        AttributeSystemMode = 0x001c,
        AttributeAlarmMask = 0x001d,
        AttributeThermostatRunningMode = 0x001e
    };
    Q_ENUM(Attribute)

    enum SystemMode {
        SystemModeOff = 0x00,
        SystemModeAuto = 0x01,
        SystemModeCool = 0x03,
        SystemModeHeat = 0x04,
        SystemModeEmergencyHeating = 0x05,
        SystemModePrecooling = 0x06,
        SystemModeFanOnly = 0x07,
        SystemModeDry = 0x08,
        SystemModeSleep = 0x09
    };
    Q_ENUM(SystemMode)

    explicit ZigbeeClusterThermostat(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    qint16 localTemperature() const;
    qint16 occupiedCoolingSetpoint() const;
    qint16 occupiedHeatingSetpoint() const;

    // targetTemp is °C*100. e.g. 20.5°C -> 2050
    ZigbeeClusterReply *setOccupiedHeatingSetpoint(qint16 occupiedHeatingSetpoint);
    ZigbeeClusterReply *setOccupiedCoolingSetpoint(qint16 occupiedCoolingSetpoint);

signals:
    void localTemperatureChanged(qint16 localTemp);
    void occupiedCoolingSetpointChanged(qint16 occupiedCoolingSetpoint);
    void occupiedHeatingSetpointChanged(qint16 occupiedHeatingSetpoint);

private:
    void setAttribute(const ZigbeeClusterAttribute &attribute) override;
};

#endif // ZIGBEECLUSTERTHERMOSTAT_H
