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

#include "zigbeeclusterdoorlock.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeClusterDoorLock::ZigbeeClusterDoorLock(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdDoorLock, direction, parent)
{

}

ZigbeeClusterReply *ZigbeeClusterDoorLock::lockDoor(const QByteArray code)
{
    QByteArray payload;
    if (!code.isEmpty()) {
        QDataStream stream(&payload, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << static_cast<quint8>(Zigbee::OctetString);
        for (int i = 0; i < code.length(); i++) {
            stream << static_cast<quint8>(code.at(i));
        }
    }

    return executeClusterCommand(Command::CommandLockDoor, payload);
}

ZigbeeClusterReply *ZigbeeClusterDoorLock::unlockDoor(const QByteArray code)
{
    QByteArray payload;
    if (!code.isEmpty()) {
        QDataStream stream(&payload, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << static_cast<quint8>(Zigbee::OctetString);
        for (int i = 0; i < code.length(); i++) {
            stream << static_cast<quint8>(code.at(i));
        }
    }

    return executeClusterCommand(Command::CommandUnlockDoor, payload);
}

ZigbeeClusterReply *ZigbeeClusterDoorLock::toggle(const QByteArray code)
{
    QByteArray payload;
    if (!code.isEmpty()) {
        QDataStream stream(&payload, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << static_cast<quint8>(Zigbee::OctetString);
        for (int i = 0; i < code.length(); i++) {
            stream << static_cast<quint8>(code.at(i));
        }
    }

    return executeClusterCommand(Command::CommandToggle, payload);
}

ZigbeeClusterReply *ZigbeeClusterDoorLock::unlockDoorWithTimeout(quint16 timeoutSeconds, const QByteArray code)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << timeoutSeconds;
    if (!code.isEmpty()) {
        stream << static_cast<quint8>(Zigbee::OctetString);
        for (int i = 0; i < code.length(); i++) {
            stream << static_cast<quint8>(code.at(i));
        }
    }

    return executeClusterCommand(Command::CommandUnlockWithTimeout, payload);
}

ZigbeeClusterDoorLock::LockState ZigbeeClusterDoorLock::lockState() const
{
    return m_lockState;
}

ZigbeeClusterDoorLock::DoorState ZigbeeClusterDoorLock::doorState() const
{
    return m_doorState;
}

void ZigbeeClusterDoorLock::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    switch (attribute.id()) {
    case AttributeLockState:
        m_lockState = static_cast<LockState>(attribute.dataType().toUInt8());
        emit lockStateChanged(m_lockState);
        break;
    case AttributeDoorState:
        m_doorState = static_cast<DoorState>(attribute.dataType().toUInt8());
        emit doorStateChanged(m_doorState);
        break;
    }
}

void ZigbeeClusterDoorLock::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    qCDebug(dcZigbeeCluster()) << "Processing cluster frame" << m_node << m_endpoint << this << frame;

    switch (m_direction) {
    case Client:
        // If the client cluster sends data to a server cluster (independent which), the command was executed on the device like button pressed
        if (frame.header.frameControl.direction == ZigbeeClusterLibrary::DirectionClientToServer) {
            // Read the payload which is
            Command command = static_cast<Command>(frame.header.command);
            qCDebug(dcZigbeeCluster()) << "Received" << command << "from" << m_node << m_endpoint << this;
        }
        break;
    case Server:
        qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
        break;
    }
}
