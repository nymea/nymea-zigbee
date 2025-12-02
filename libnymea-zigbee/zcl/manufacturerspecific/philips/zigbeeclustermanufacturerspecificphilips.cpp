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

#include "zigbeeclustermanufacturerspecificphilips.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeClusterManufacturerSpecificPhilips::ZigbeeClusterManufacturerSpecificPhilips(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdManufacturerSpecificPhilips, direction, parent)
{

}

void ZigbeeClusterManufacturerSpecificPhilips::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    qCDebug(dcZigbeeCluster()) << "Processing manufacturer specific (Philips) cluster frame" << m_node << m_endpoint << this << frame;

    switch (m_direction) {
    case Client:
        qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
        break;
    case Server:
        if (frame.header.frameControl.direction == ZigbeeClusterLibrary::DirectionServerToClient) {
            Command command = static_cast<Command>(frame.header.command);
            switch (command) {
            case CommandButtonPress: {
                QDataStream payloadStream(frame.payload);
                payloadStream.setByteOrder(QDataStream::LittleEndian);
                quint8 button; quint16 unknown1; quint8 unknown2; quint8 operation;
                payloadStream >> button >> unknown1 >> unknown2 >> operation;
                qCDebug(dcZigbeeCluster()) << "Received manufacturer specific (Philips) button press. Button:" << button << "Operation:" << operation;
                emit buttonPressed(button, Operation(operation), frame.header.transactionSequenceNumber);
                break;
            }
            default:
                qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
            }
        } else {
            qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
        }
        break;
    }
}
