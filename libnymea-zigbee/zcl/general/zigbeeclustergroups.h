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

#ifndef ZIGBEECLUSTERGROUPS_H
#define ZIGBEECLUSTERGROUPS_H

#include <QObject>

#include "zcl/zigbeecluster.h"

class ZigbeeClusterGroups : public ZigbeeCluster
{
    Q_OBJECT
public:
    enum Attribute {
        // 1 supported, 0 not supported
        AttributeNameSupport = 0x0000
    };
    Q_ENUM(Attribute)

    enum Command {
        CommandAddGroup = 0x00,
        CommandViewGroup = 0x01,
        CommandGetGroupMembership = 0x02,
        CommandRemoveGroup = 0x03,
        CommandRemoveAllGroups = 0x04,
        CommandAddGroupIfIdentifying = 0x05
    };
    Q_ENUM(Command)

    explicit ZigbeeClusterGroups(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    ZigbeeClusterReply *addGroup(quint16 groupId, const QString &groupName);
    ZigbeeClusterReply *viewGroup(quint16 groupId);
    ZigbeeClusterReply *getGroupMembership(quint8 groupCount, const QList<quint16> &groupList);
    ZigbeeClusterReply *removeGroup(quint16 groupId);
    ZigbeeClusterReply *removeAllGroups();
    ZigbeeClusterReply *addGroupIfIdentifying(quint16 groupId, const QString &groupName);

signals:


};

#endif // ZIGBEECLUSTERGROUPS_H
