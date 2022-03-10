/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2022, nymea GmbH
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
