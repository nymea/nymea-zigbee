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

#ifndef ZIGBEECLUSTERLEVELCONTROL_H
#define ZIGBEECLUSTERLEVELCONTROL_H

#include <QObject>

#include "zcl/zigbeecluster.h"
#include "zcl/zigbeeclusterreply.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterLevelControl : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Attribute {
        AttributeCurrentLevel = 0x0000,
        AttributeRemainingTime = 0x0001,
        AttributeOnOffTransitionTime = 0x0010,
        AttributeOnLevel = 0x0011,
        AttributeOnTransitionTime = 0x0012,
        AttributeOffTransitionTime = 0x0013,
        AttributeDefaultMoveRate = 0x0014
    };
    Q_ENUM(Attribute)

    enum Command {
        CommandMoveToLevel = 0x00,
        CommandMove = 0x01,
        CommandStep = 0x02,
        CommandStop = 0x03,
        CommandMoveToLevelWithOnOff = 0x04,
        CommandMoveWithOnOff = 0x05,
        CommandStepWithOnOff = 0x06,
        CommandStopWithOnOff = 0x07
    };
    Q_ENUM(Command)

    enum MoveMode {
        MoveModeUp = 0x00,
        MoveModeDown = 0x01
    };
    Q_ENUM(MoveMode)

    enum StepMode {
        StepModeUp = 0x00,
        StepModeDown = 0x01
    };
    Q_ENUM(StepMode)

    explicit ZigbeeClusterLevelControl(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    ZigbeeClusterReply *commandMoveToLevel(quint8 level, quint16 transitionTime = 0xffff);
    ZigbeeClusterReply *commandMove(MoveMode moveMode, quint8 rate = 0xff);
    ZigbeeClusterReply *commandStep(StepMode stepMode, quint8 stepSize = 0x01, quint16 transitionTime = 0xffff);
    ZigbeeClusterReply *commandStop();

    // With on/off
    ZigbeeClusterReply *commandMoveToLevelWithOnOff(quint8 level, quint16 transitionTime = 0xffff);
    ZigbeeClusterReply *commandMoveWithOnOff(MoveMode moveMode, quint8 rate = 0xff);
    ZigbeeClusterReply *commandStepWithOnOff(StepMode stepMode, quint8 stepSize = 0x01, quint16 transitionTime = 0xffff);
    ZigbeeClusterReply *commandStopWithOnOff();

    quint8 currentLevel() const;

private:
    quint8 m_currentLevel = 0;

    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

protected:
    void processDataIndication(ZigbeeClusterLibrary::Frame frame) override;

signals:
    void currentLevelChanged(quint8 level);
    void commandReceived(ZigbeeClusterLevelControl::Command command, const QByteArray &parameter, quint8 transactionSequenceNumber);
    void commandMoveToLevelReceived(bool withOnOff, quint8 level, quint16 transitionTime, quint8 transactionSequenceNumber);
    void commandMoveReceived(bool withOnOff, MoveMode moveMode, quint8 rate, quint8 transactionSeqenceNumber);
    void commandStepReceived(bool withOnOff, StepMode stepMode, quint8 stepSize, quint16 transitionTime, quint8 transactionSequenceNumber);
    void commandStopReceived(bool withOnOff, quint8 transactionSequenceNumber);

};

#endif // ZIGBEECLUSTERLEVELCONTROL_H
