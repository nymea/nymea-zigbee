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

    enum FadeMode {
        FadeModeUp = 0x00,
        FadeModeDown = 0x01
    };
    Q_ENUM(FadeMode)

    explicit ZigbeeClusterLevelControl(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    ZigbeeClusterReply *commandMoveToLevel(quint8 level, quint16 transitionTime = 0xffff);
    ZigbeeClusterReply *commandMove(MoveMode moveMode, quint8 rate = 0xff);
    ZigbeeClusterReply *commandStep(FadeMode fadeMode, quint8 stepSize = 0x01, quint16 transitionTime = 0xffff);
    ZigbeeClusterReply *commandStop();

    // With on/off
    ZigbeeClusterReply *commandMoveToLevelWithOnOff(quint8 level, quint16 transitionTime = 0xffff);
    ZigbeeClusterReply *commandMoveWithOnOff(MoveMode moveMode, quint8 rate = 0xff);
    ZigbeeClusterReply *commandStepWithOnOff(FadeMode fadeMode, quint8 stepSize = 0x01, quint16 transitionTime = 0xffff);
    ZigbeeClusterReply *commandStopWithOnOff();

    quint8 currentLevel() const;

private:
    quint8 m_currentLevel = 0;

    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

protected:
    void processDataIndication(ZigbeeClusterLibrary::Frame frame) override;

signals:
    void currentLevelChanged(quint8 level);
    void commandSent(ZigbeeClusterLevelControl::Command command, const QByteArray &parameter = QByteArray());
    void commandMoveSent(MoveMode moveMode, quint8 rate = 0xff);
    void commandStepSent(FadeMode fadeMode, quint8 stepSize, quint16 transitionTime);

};

#endif // ZIGBEECLUSTERLEVELCONTROL_H
