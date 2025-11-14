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

#include "zigbeeclusterscenes.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeClusterScenes::ZigbeeClusterScenes(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, ZigbeeCluster::Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdScenes, direction, parent)
{

}

void ZigbeeClusterScenes::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    switch (m_direction) {
    case Client: {
        // Read the payload which is
        Command command = static_cast<Command>(frame.header.command);
        QByteArray payload = frame.payload;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QDataStream payloadStream(&payload, QDataStream::ReadOnly);
#else
        QDataStream payloadStream(&payload, QIODevice::ReadOnly);
#endif
        payloadStream.setByteOrder(QDataStream::LittleEndian);
        quint16 groupId = 0; quint8 sceneId;
        payloadStream >> groupId >> sceneId;
        qCDebug(dcZigbeeCluster()).noquote() << "Received" << command << "for group" << "0x" + QString::number(groupId, 16) << "and scene" << sceneId << "from" << m_node << m_endpoint << this;
        emit commandReceived(command, groupId, sceneId, frame.header.transactionSequenceNumber);
        break;
    }
    case Server:
        qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
        break;
    }
}
