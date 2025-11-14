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

#ifndef ZIGBEEDEVICEOBJECT_H
#define ZIGBEEDEVICEOBJECT_H

#include <QObject>

#include "zigbeenetworkreply.h"
#include "zigbeedeviceobjectreply.h"

class ZigbeeNode;
class ZigbeeNetwork;

class ZigbeeDeviceObject : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeDeviceObject(ZigbeeNetwork *network, ZigbeeNode *node, QObject *parent = nullptr);

    // Device and service discovery
    ZigbeeDeviceObjectReply *requestNetworkAddress();
    ZigbeeDeviceObjectReply *requestIeeeAddress();
    ZigbeeDeviceObjectReply *requestNodeDescriptor();
    ZigbeeDeviceObjectReply *requestPowerDescriptor();
    ZigbeeDeviceObjectReply *requestActiveEndpoints();
    ZigbeeDeviceObjectReply *requestSimpleDescriptor(quint8 endpointId);
    // TODO: implement other device and service discovery methods

    // End device binding
    ZigbeeDeviceObjectReply *requestBindGroupAddress(quint8 sourceEndpointId, quint16 clusterId, quint16 destinationAddress);
    ZigbeeDeviceObjectReply *requestBindIeeeAddress(quint8 sourceEndpointId, quint16 clusterId, const ZigbeeAddress &destinationIeeeAddress, quint8 destinationEndpointId);
    ZigbeeDeviceObjectReply *requestUnbind(const ZigbeeDeviceProfile::BindingTableListRecord &bindingRecord);
    ZigbeeDeviceObjectReply *requestBindRegister(const ZigbeeAddress &ieeeAddress);

    // Management request
    ZigbeeDeviceObjectReply *requestMgmtLeaveNetwork(bool rejoin = false, bool removeChildren = false);
    ZigbeeDeviceObjectReply *requestMgmtLqi(quint8 startIndex = 0x00);
    ZigbeeDeviceObjectReply *requestMgmtBind(quint8 startIndex = 0x00);
    ZigbeeDeviceObjectReply *requestMgmtRtg(quint8 startIndex = 0x00);

    // TODO: write all requests

private:
    ZigbeeNetwork *m_network = nullptr;
    ZigbeeNode *m_node = nullptr;

    quint8 m_transactionSequenceNumber = 0;
    QHash<quint8, ZigbeeDeviceObjectReply *> m_pendingReplies;

    // Helper methods for replies
    ZigbeeNetworkRequest buildZdoRequest(quint16 zdoRequest);
    ZigbeeDeviceObjectReply *createZigbeeDeviceObjectReply(const ZigbeeNetworkRequest &request, quint8 transactionSequenceNumber);
    bool verifyNetworkError(ZigbeeDeviceObjectReply *zdoReply, ZigbeeNetworkReply *networkReply);
    void finishZdoReply(ZigbeeDeviceObjectReply *zdoReply);

public slots:
    void processApsDataIndication(const Zigbee::ApsdeDataIndication &indication);

};

#endif // ZIGBEEDEVICEOBJECT_H
