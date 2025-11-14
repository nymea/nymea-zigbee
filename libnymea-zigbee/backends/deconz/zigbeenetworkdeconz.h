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

#ifndef ZIGBEENETWORKDECONZ_H
#define ZIGBEENETWORKDECONZ_H

#include <QObject>

#include "zigbeenetwork.h"
#include "zigbeechannelmask.h"
#include "zcl/zigbeeclusterlibrary.h"
#include "zigbeebridgecontrollerdeconz.h"

class ZigbeeNetworkDeconz : public ZigbeeNetwork
{
    Q_OBJECT
public:
    enum CreateNetworkState {
        CreateNetworkStateIdle,
        CreateNetworkStateStopNetwork,
        CreateNetworkStateWriteConfiguration,
        CreateNetworkStateStartNetwork,
        CreateNetworkStateReadConfiguration,
        CreateNetworkStateInitializeCoordinatorNode
    };
    Q_ENUM(CreateNetworkState)

    explicit ZigbeeNetworkDeconz(const QUuid &networkUuid, QObject *parent = nullptr);

    ZigbeeBridgeController *bridgeController() const override;
    Zigbee::ZigbeeBackendType backendType() const override;

    // Sending an APSDE-DATA.request, will be finished on APSDE-DATA.confirm
    ZigbeeNetworkReply *sendRequest(const ZigbeeNetworkRequest &request) override;

    void setPermitJoining(quint8 duration, quint16 address = Zigbee::BroadcastAddressAllRouters) override;

private:
    ZigbeeBridgeControllerDeconz *m_controller = nullptr;
    bool m_networkRunning = false;
    CreateNetworkState m_createState = CreateNetworkStateIdle;
    bool m_createNewNetwork = false;
    bool m_initializing = false;
    QString m_protocolVersion;
    QString m_firmwareVersion;

    QList<ZigbeeNetworkReply*> m_requestQueue;
    QHash<quint8, ZigbeeNetworkReply *> m_pendingReplies;

    QTimer *m_pollNetworkStateTimer = nullptr;
    void setCreateNetworkState(CreateNetworkState state);

    // Init procedure
    int m_initRetry = 0;
    void runNetworkInitProcess();

    ZigbeeNetworkReply *requestSetPermitJoin(quint16 shortAddress = Zigbee::BroadcastAddressAllRouters, quint8 duration = 0xfe);

    void sendPendingRequests();

protected:
    void startNetworkInternally();

private slots:
    void onControllerAvailableChanged(bool available);
    void onPollNetworkStateTimeout();

    void onApsDataConfirmReceived(const Zigbee::ApsdeDataConfirm &confirm);
    void onApsDataIndicationReceived(const Zigbee::ApsdeDataIndication &indication);


public slots:
    void startNetwork() override;
    void stopNetwork() override;
    void reset() override;
    void factoryResetNetwork() override;
    void destroyNetwork() override;

};

#endif // ZIGBEENETWORKDECONZ_H
