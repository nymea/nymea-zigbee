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

private:
    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

protected:
    void processDataIndication(ZigbeeClusterLibrary::Frame frame) override;

    // TODO: identifyQueryResponse signal

};

#endif // ZIGBEECLUSTERIDENTIFY_H
