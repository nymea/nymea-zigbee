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
