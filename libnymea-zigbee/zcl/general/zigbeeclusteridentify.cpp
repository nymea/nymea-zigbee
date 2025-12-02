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

#include "zigbeeclusteridentify.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"

#include <QDataStream>

ZigbeeClusterIdentify::ZigbeeClusterIdentify(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, ZigbeeCluster::Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdIdentify, direction, parent)
{

}

ZigbeeClusterReply *ZigbeeClusterIdentify::identify(quint16 seconds)
{
    // Note: the identify time unit is 0.5 seconds
    QByteArray payload = ZigbeeDataType(seconds * 2).data();
    return executeClusterCommand(ZigbeeClusterIdentify::CommandIdentify, payload);
}

ZigbeeClusterReply *ZigbeeClusterIdentify::identifyQuery()
{
    return executeClusterCommand(ZigbeeClusterIdentify::CommandIdentifyQuery);
}

ZigbeeClusterReply *ZigbeeClusterIdentify::triggerEffect(ZigbeeClusterIdentify::Effect effect, quint8 effectVariant)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(effect);
    stream << static_cast<quint8>(effectVariant);

    return executeClusterCommand(ZigbeeClusterIdentify::CommandTriggerEffect, payload);
}

void ZigbeeClusterIdentify::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    Command command = static_cast<Command>(frame.header.command);
    qCDebug(dcZigbeeCluster()) << "Processing cluster frame" << m_node << m_endpoint << this << frame << command;

    switch (command) {
    case CommandIdentifyQuery:
        // We are not identifying, we can ignore the command according to the specs
        qCDebug(dcZigbeeCluster()) << "Received identify query command. We ignore this request according to specs, since we are not identifying our selfs visually.";
        break;
    default:
        qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame << command;
        break;
    }


}
