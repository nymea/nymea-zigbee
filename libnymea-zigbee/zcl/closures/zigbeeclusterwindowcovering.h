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

#ifndef ZIGBEECLUSTERWINDOWCOVERING_H
#define ZIGBEECLUSTERWINDOWCOVERING_H

#include <QObject>
#include "zcl/zigbeecluster.h"
#include "zcl/zigbeeclusterreply.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterWindowCovering : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Attribute {
        // Window covering information
        AttributeWindowCoveringType = 0x0000,
        AttributeLockPhysicalClosedLimitLift = 0x0001,
        AttributePhysicalClosedLimitTilt = 0x0002,
        AttributeCurrentPositionLift = 0x0003,
        AttributeCurrentPositionTilt = 0x0004,
        AttributeNumberOfActuationsLift = 0x0005,
        AttributeNumberOfActuationsTilt = 0x0006,
        AttributeConfigStatus = 0x0007,
        AttributeCurrentPositionLiftPercentage = 0x0008,
        AttributeCurrentPositionTiltPercentage = 0x0009,

        // Window covering settings
        AttributeInstalledOpenLimitLift = 0x0100,
        AttributeInstalledClosedLimitLift = 0x0101,
        AttributeInstalledOpenLimitTilt = 0x0102,
        AttributeInstalledClosedLimitTilt = 0x0103,
        AttributeVelocityLift = 0x0104,
        AttributeAccelerationTileLift = 0x0105,
        AttributeDecelerationTimeLift = 0x0106,
        AttributeMode = 0x0107,
        AttributeIntermediateSetpointsLift = 0x0108,
        AttributeIntermediateSetpointsTilt = 0x0109
    };
    Q_ENUM(Attribute)

    enum WindowCoveringType {
        WindowCoveringTypeRollerShade = 0x00,
        WindowCoveringTypeRollerShade2Motor = 0x01,
        WindowCoveringTypeRollerShadeExterior = 0x02,
        WindowCoveringTypeRollerShadeExterior2Motor = 0x03,
        WindowCoveringTypeDrapery = 0x04,
        WindowCoveringTypeAwning = 0x05,
        WindowCoveringTypeShutter = 0x06,
        WindowCoveringTypeTiltBlindTiltOnly = 0x07,
        WindowCoveringTypeTiltBlindLiftAndTilt = 0x08,
        WindowCoveringTypeProjectorScreen = 0x09
    };
    Q_ENUM(WindowCoveringType)

    enum ConfigStatusFlag {
        ConfigStatusFlagOperational = 0x01,
        ConfigStatusFlagOnline = 0x02,
        ConfigStatusFlagReversed = 0x04,
        ConfigStatusFlagLiftControlClosedLoop = 0x08,
        ConfigStatusFlagTiltControlClosedLoop = 0x10,
        ConfigStatusFlagLiftEncoderControlled = 0x20,
        ConfigStatusFlagTiltEncoderControlled = 0x40
    };
    Q_ENUM(ConfigStatusFlag)
    Q_DECLARE_FLAGS(ConfigStatus, ConfigStatusFlag)
    Q_FLAG(ConfigStatus)

    enum ModeFlag {
        ModeFlagReversed = 0x01,
        ModeFlagCalibration = 0x02,
        ModeFlagMaintenance = 0x04,
        ModeFlagLEDsOn = 0x08
    };
    Q_ENUM(ModeFlag)
    Q_DECLARE_FLAGS(Mode, ModeFlag)
    Q_FLAG(Mode)

    enum Command {
        CommandOpen = 0x00, // M
        CommandClose = 0x01, // M
        CommandStop = 0x02, // M
        CommandGoToLiftValue = 0x04,
        CommandGoToLiftPercentage = 0x05,
        CommandGoToTiltValue = 0x07,
        CommandGoToTiltPercentage = 0x08
    };
    Q_ENUM(Command)

    explicit ZigbeeClusterWindowCovering(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    ZigbeeClusterReply *open();
    ZigbeeClusterReply *close();
    ZigbeeClusterReply *stop();
    ZigbeeClusterReply *goToLiftValue(quint16 liftValue);
    ZigbeeClusterReply *goToLiftPercentage(quint8 liftPercentage);
    ZigbeeClusterReply *goToTiltValue(quint16 tiltValue);
    ZigbeeClusterReply *goToTiltPercentage(quint8 tiltPercentage);

    quint8 currentLiftPercentage() const;
    quint8 currentTiltPercentage() const;

private:
    quint8 m_currentLiftPercentage = 0;
    quint8 m_currentTiltPercentage = 0;

    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

signals:
    void currentLiftPercentageChanged(quint8 liftPercentage);
    void currentTiltPercentageChanged(quint8 tiltPercentage);
};

#endif // ZIGBEECLUSTERWINDOWCOVERING_H
