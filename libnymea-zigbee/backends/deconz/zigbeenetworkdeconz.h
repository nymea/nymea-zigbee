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

    explicit ZigbeeNetworkDeconz(QObject *parent = nullptr);

    ZigbeeBridgeController *bridgeController() const override;

    // Sending an APSDE-DATA.request, will be finished on APSDE-DATA.confirm
    ZigbeeNetworkReply *sendRequest(const ZigbeeNetworkRequest &request) override;

    ZigbeeNetworkReply *setPermitJoin(quint16 shortAddress, quint8 duration);

private:
    ZigbeeBridgeControllerDeconz *m_controller = nullptr;
    bool m_networkRunning = false;
    CreateNetworkState m_createState = CreateNetworkStateIdle;
    bool m_createNewNetwork = false;

    QTimer *m_permitJoinRefreshTimer = nullptr;

    QHash<quint8, ZigbeeNetworkReply *> m_pendingReplies;

    QTimer *m_pollNetworkStateTimer = nullptr;
    void setCreateNetworkState(CreateNetworkState state);

    // ZDO
    void handleZigbeeDeviceProfileIndication(const DeconzApsDataIndication &indication);

    // ZZL
    void handleZigbeeLightLinkIndication(const DeconzApsDataIndication &indication);

    // HA
    void handleZigbeeHomeAutomationIndication(const DeconzApsDataIndication &indication);

    // GP

protected:
    void setPermitJoiningInternal(bool permitJoining) override;
    void startNetworkInternally();

private slots:
    void onControllerAvailableChanged(bool available);
    void onPollNetworkStateTimeout();
    void onPermitJoinRefreshTimout();

    void onApsDataConfirmReceived(const DeconzApsDataConfirm &confirm);
    void onApsDataIndicationReceived(const DeconzApsDataIndication &indication);

    void onDeviceAnnounced(quint16 shortAddress, ZigbeeAddress ieeeAddress, quint8 macCapabilities);

public slots:
    void startNetwork() override;
    void stopNetwork() override;
    void reset() override;
    void factoryResetNetwork() override;

};

#endif // ZIGBEENETWORKDECONZ_H
