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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QDataStream stream(&payload, QDataStream::WriteOnly);
#else
    QDataStream stream(&payload, QIODevice::WriteOnly);
#endif
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QDataStream stream(&payload, QDataStream::WriteOnly);
#else
    QDataStream stream(&payload, QIODevice::WriteOnly);
#endif
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << groupId;
    return executeClusterCommand(ZigbeeClusterGroups::CommandViewGroup, payload);
}

ZigbeeClusterReply *ZigbeeClusterGroups::getGroupMembership(quint8 groupCount, const QList<quint16> &groupList)
{
    QByteArray payload;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QDataStream stream(&payload, QDataStream::WriteOnly);
#else
    QDataStream stream(&payload, QIODevice::WriteOnly);
#endif
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QDataStream stream(&payload, QDataStream::WriteOnly);
#else
    QDataStream stream(&payload, QIODevice::WriteOnly);
#endif
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QDataStream stream(&payload, QDataStream::WriteOnly);
#else
    QDataStream stream(&payload, QIODevice::WriteOnly);
#endif
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << groupId << static_cast<quint8>(Zigbee::CharString);
    for (int i = 0; i < groupName.length(); i++) {
        stream << static_cast<quint8>(groupName.toUtf8().at(i));
    }
    return executeClusterCommand(ZigbeeClusterGroups::CommandAddGroup, payload);
}
