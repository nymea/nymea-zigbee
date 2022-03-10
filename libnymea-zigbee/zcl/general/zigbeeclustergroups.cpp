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

#include "zigbeeclustergroups.h"
#include "loggingcategory.h"

#include <QDataStream>

ZigbeeClusterGroups::ZigbeeClusterGroups(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdGroups, direction, parent)
{

}

ZigbeeClusterReply *ZigbeeClusterGroups::addGroup(quint16 groupId, const QString &groupName)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << groupId << static_cast<quint8>(Zigbee::CharString);
    for (int i = 0; i < groupName.length(); i++) {
        stream << static_cast<quint8>(groupName.toUtf8().at(i));
    }
    return executeClusterCommand(ZigbeeClusterGroups::CommandAddGroup, payload);
}

ZigbeeClusterReply *ZigbeeClusterGroups::viewGroup(quint16 groupId)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << groupId;
    return executeClusterCommand(ZigbeeClusterGroups::CommandViewGroup, payload);
}

ZigbeeClusterReply *ZigbeeClusterGroups::getGroupMembership(quint8 groupCount, const QList<quint16> &groupList)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << groupCount;
    for (int i = 0; i < groupList.length(); i++) {
        stream << groupList.at(i);
    }
    return executeClusterCommand(ZigbeeClusterGroups::CommandGetGroupMembership, payload);
}

ZigbeeClusterReply *ZigbeeClusterGroups::removeGroup(quint16 groupId)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << groupId;
    return executeClusterCommand(ZigbeeClusterGroups::CommandRemoveGroup, payload);
}

ZigbeeClusterReply *ZigbeeClusterGroups::removeAllGroups()
{
    return executeClusterCommand(ZigbeeClusterGroups::CommandRemoveAllGroups);
}

ZigbeeClusterReply *ZigbeeClusterGroups::addGroupIfIdentifying(quint16 groupId, const QString &groupName)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << groupId << static_cast<quint8>(Zigbee::CharString);
    for (int i = 0; i < groupName.length(); i++) {
        stream << static_cast<quint8>(groupName.toUtf8().at(i));
    }
    return executeClusterCommand(ZigbeeClusterGroups::CommandAddGroup, payload);
}
