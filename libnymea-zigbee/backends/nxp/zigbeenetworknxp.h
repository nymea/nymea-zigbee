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

#ifndef ZIGBEENETWORKNXP_H
#define ZIGBEENETWORKNXP_H

#include <QObject>
#include <QTimer>

#include "zigbeenetwork.h"
#include "zigbeechannelmask.h"
#include "zcl/zigbeeclusterlibrary.h"
#include "zigbeebridgecontrollernxp.h"

class ZigbeeNetworkNxp : public ZigbeeNetwork
{
    Q_OBJECT
public:
    explicit ZigbeeNetworkNxp(const QUuid &networkUuid, QObject *parent = nullptr);

    ZigbeeBridgeController *bridgeController() const override;
    Zigbee::ZigbeeBackendType backendType() const override;
    ZigbeeNetworkReply *sendRequest(const ZigbeeNetworkRequest &request) override;

    void setPermitJoining(quint8 duration, quint16 address = Zigbee::BroadcastAddressAllRouters) override;

private:
    ZigbeeBridgeControllerNxp *m_controller = nullptr;
    bool m_networkRunning = false;

    QHash<quint8, ZigbeeNetworkReply *> m_pendingReplies;
    QHash<quint8, ZigbeeNetworkReply *> m_bufferedReplies;

    QQueue<ZigbeeNetworkReply *> m_replyQueue;
    ZigbeeNetworkReply *m_currentReply = nullptr;

    void sendNextReply();
    void finishReplyInternally(ZigbeeNetworkReply *reply, ZigbeeNetworkReply::Error error = ZigbeeNetworkReply::ErrorNoError);

    int m_reconnectCounter = 0;
    bool processVersionReply(ZigbeeInterfaceNxpReply *reply);

    ZigbeeNetworkReply *requestSetPermitJoin(quint16 shortAddress = Zigbee::BroadcastAddressAllRouters, quint8 duration = 0xfe);

private slots:
    void onControllerAvailableChanged(bool available);
    void onControllerStateChanged(ZigbeeBridgeControllerNxp::ControllerState controllerState);
    void onInterfaceNotificationReceived(Nxp::Notification notification, const QByteArray &payload);

    void onApsDataConfirmReceived(const Zigbee::ApsdeDataConfirm &confirm);
    void onApsDataIndicationReceived(const Zigbee::ApsdeDataIndication &indication);
    void onApsDataAckReceived(const Zigbee::ApsdeDataAck &acknowledgement);

    void onNodeLeftIndication(const ZigbeeAddress &ieeeAddress, bool rejoining);

signals:

public slots:
    void startNetwork() override;
    void stopNetwork() override;
    void reset() override;
    void factoryResetNetwork() override;
    void destroyNetwork() override;
};

#endif // ZIGBEENETWORKNXP_H
