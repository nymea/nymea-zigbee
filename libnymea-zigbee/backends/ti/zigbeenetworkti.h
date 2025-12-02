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

#ifndef ZIGBEENETWORKTI_H
#define ZIGBEENETWORKTI_H

#include <QObject>

#include "zigbeenetwork.h"
#include "zigbeechannelmask.h"
#include "zcl/zigbeeclusterlibrary.h"
#include "zigbeebridgecontrollerti.h"

class ZigbeeNetworkTi : public ZigbeeNetwork
{
    Q_OBJECT
public:

    explicit ZigbeeNetworkTi(const QUuid &networkUuid, QObject *parent = nullptr);

    ZigbeeBridgeController *bridgeController() const override;
    Zigbee::ZigbeeBackendType backendType() const override;

    // Sending an APSDE-DATA.request, will be finished on APSDE-DATA.confirm
    ZigbeeNetworkReply *sendRequest(const ZigbeeNetworkRequest &request) override;

    void setPermitJoining(quint8 duration, quint16 address = Zigbee::BroadcastAddressAllRouters) override;

public slots:
    void startNetwork() override;
    void stopNetwork() override;
    void reset() override;
    void factoryResetNetwork() override;
    void destroyNetwork() override;


private slots:
    void onControllerAvailableChanged(bool available);
    void onControllerStateChanged(ZigbeeBridgeControllerTi::ControllerState state);
    void onPermitJoinStateChanged(quint8 duration);
    void onDeviceIndication(quint16 shortAddress, const ZigbeeAddress &ieeeAddress, quint8 macCapabilities);
    void onNodeLeaveIndication(const ZigbeeAddress &ieeeAddress, bool request, bool remove, bool rejoin);

    void onApsDataConfirmReceived(const Zigbee::ApsdeDataConfirm &confirm);
    void onApsDataIndicationReceived(const Zigbee::ApsdeDataIndication &indication);

private:
    void initController();
    void commissionController();
    void startControllerNetwork();

    void processGreenPowerFrame(const Zigbee::ApsdeDataIndication &indication);
    QByteArray encryptSecurityKey(quint32 sourceId, const QByteArray &securityKey);

private:
    ZigbeeBridgeControllerTi *m_controller = nullptr;

    QList<ZigbeeNetworkReply*> m_requestQueue;
};

#endif // ZIGBEENETWORKDECONZ_H
