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

#ifndef ZIGBEECLUSTERONOFF_H
#define ZIGBEECLUSTERONOFF_H

#include <QObject>

#include "zcl/zigbeecluster.h"
#include "zcl/zigbeeclusterreply.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterOnOff : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Attribute {
        AttributeOnOff = 0x0000,
        AttributeGlobalSceneControl = 0x4000,
        AttributeOnTime = 0x4001,
        AttributeOffTime = 0x4002
    };
    Q_ENUM(Attribute)

    enum Command {
        CommandOff = 0x00,
        CommandOn = 0x01,
        CommandToggle = 0x02,
        CommandOffWithEffect = 0x40,
        CommandOnWithRecallGlobalScene = 0x41,
        CommandOnWithTimedOff = 0x42
    };
    Q_ENUM(Command)

    enum Effect {
        EffectDelayedAllOff = 0x00,
        EffectDyingLight = 0x01
    };
    Q_ENUM(Effect)

    explicit ZigbeeClusterOnOff(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    ZigbeeClusterReply *commandOff();
    ZigbeeClusterReply *commandOn();
    ZigbeeClusterReply *commandToggle();
    ZigbeeClusterReply *commandOffWithEffect(Effect effect, quint8 effectVariant = 0x00);
    ZigbeeClusterReply *commandOnWithRecallGlobalScene();
    ZigbeeClusterReply *commandOnWithTimedOff(bool acceptOnlyWhenOn, quint16 onTime, quint16 offWaitTime);

private:
    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

protected:
    void processDataIndication(ZigbeeClusterLibrary::Frame frame) override;

signals:
    // Server cluster signals
    void powerChanged(bool power);

    // Client cluster signals
    void commandSent(Command command);

};

#endif // ZIGBEECLUSTERONOFF_H
