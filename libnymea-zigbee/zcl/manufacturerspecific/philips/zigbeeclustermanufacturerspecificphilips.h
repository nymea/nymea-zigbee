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

#ifndef ZIGBEECLUSTERMANUFACTURERSPECIFICPHILIPS_H
#define ZIGBEECLUSTERMANUFACTURERSPECIFICPHILIPS_H

#include <QObject>

#include "zcl/zigbeecluster.h"
#include "zcl/zigbeeclusterreply.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterManufacturerSpecificPhilips : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Command {
        CommandButtonPress = 0x00,
    };
    Q_ENUM(Command)

    enum Operation {
        OperationButtonPress = 0x00,
        OperationButtonHold = 0x01,
        OperationButtonShortRelease = 0x02,
        OperationButtonLongRelease = 0x03
    };
    Q_ENUM(Operation)

    explicit ZigbeeClusterManufacturerSpecificPhilips(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

signals:
    // Server cluster signals
    void buttonPressed(quint8 button, ZigbeeClusterManufacturerSpecificPhilips::Operation operation, quint8 transactionSequenceNumber);

protected:
    void processDataIndication(ZigbeeClusterLibrary::Frame frame) override;

};

#endif // ZIGBEECLUSTERMANUFACTURERSPECIFICPHILIPS_H
