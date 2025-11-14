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

#ifndef ZIGBEECLUSTERSCENES_H
#define ZIGBEECLUSTERSCENES_H

#include <QObject>

#include "zcl/zigbeecluster.h"

class ZigbeeClusterScenes : public ZigbeeCluster
{
    Q_OBJECT
public:
    enum Attribute {
        AttributeSceneCount = 0x0000,
        AttributeCurrentScene = 0x0001,
        AttributeCurrentGroup = 0x0002,
        AttributeSceneValid = 0x0003,
        AttributeNameSupported = 0x0004,
        AttributeLastConfiguredBy = 0x0005 // Optional
    };
    Q_ENUM(Attribute)

    enum Command {
        CommandAddScene = 0x00,
        CommandViewScene = 0x01,
        CommandRemoveScene = 0x02,
        CommandRemoveAllScenes = 0x03,
        CommandStoreScene = 0x04,
        CommandRecallScene = 0x05,
        CommandGetSceneMembership = 0x06,
        CommandEnhancedAddScene= 0x40, // O
        CommandEnhancedViewScene= 0x41, // O
        CommandCopyScene= 0x42 // O
    };
    Q_ENUM(Command)

    explicit ZigbeeClusterScenes(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

signals:
    void commandReceived(ZigbeeClusterScenes::Command command, quint16 groupId, quint8 sceneId, quint8 transactionSequenceNumber);

protected:
    void processDataIndication(ZigbeeClusterLibrary::Frame frame) override;

};

#endif // ZIGBEECLUSTERSCENES_H
