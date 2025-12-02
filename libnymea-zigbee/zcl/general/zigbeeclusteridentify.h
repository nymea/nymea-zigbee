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

#ifndef ZIGBEECLUSTERIDENTIFY_H
#define ZIGBEECLUSTERIDENTIFY_H

#include <QObject>

#include "zcl/zigbeecluster.h"
#include "zcl/zigbeeclusterreply.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterIdentify : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:

    enum Attribute {
        AttributeIdentifyTime = 0x0000
    };
    Q_ENUM(Attribute)

    enum Command {
        CommandIdentify = 0x00,
        CommandIdentifyQuery = 0x01,
        CommandTriggerEffect = 0x40
    };
    Q_ENUM(Command)

    enum Effect {
        EffectBlink = 0x00,
        EffectBreath = 0x01,
        EffectOkay = 0x02,
        EffectChannelChange = 0x0b,
        EffectFinishEffect = 0xfe,
        EffectStopEffect = 0xff
    };
    Q_ENUM(Effect)

    explicit ZigbeeClusterIdentify(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    ZigbeeClusterReply *identify(quint16 seconds);
    ZigbeeClusterReply *identifyQuery();
    ZigbeeClusterReply *triggerEffect(Effect effect, quint8 effectVariant = 0x00);

protected:
    void processDataIndication(ZigbeeClusterLibrary::Frame frame) override;

    // TODO: identifyQueryResponse signal

};

#endif // ZIGBEECLUSTERIDENTIFY_H
