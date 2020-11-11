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
    int m_reconnectCounter = 0;

    bool processVersionReply(ZigbeeInterfaceNxpReply *reply);

    // ZDO
    void handleZigbeeDeviceProfileIndication(const Zigbee::ApsdeDataIndication &indication);

    // ZCL
    void handleZigbeeClusterLibraryIndication(const Zigbee::ApsdeDataIndication &indication);

    ZigbeeNetworkReply *requestSetPermitJoin(quint16 shortAddress = Zigbee::BroadcastAddressAllRouters, quint8 duration = 0xfe);

private slots:
    void onControllerAvailableChanged(bool available);
    void onControllerStateChanged(ZigbeeBridgeControllerNxp::ControllerState controllerState);
    void onInterfaceNotificationReceived(Nxp::Notification notification, const QByteArray &payload);

    void onApsDataConfirmReceived(const Zigbee::ApsdeDataConfirm &confirm);
    void onApsDataIndicationReceived(const Zigbee::ApsdeDataIndication &indication);

    void onDeviceAnnounced(quint16 shortAddress, ZigbeeAddress ieeeAddress, quint8 macCapabilities);
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
