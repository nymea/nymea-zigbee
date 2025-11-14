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
#include "zigbeenetwork.h"
#include "zigbeebridgecontrollernxp.h"

class ZigbeeNetworkNxp : public ZigbeeNetwork
{
    Q_OBJECT
public:
    explicit ZigbeeNetworkNxp(QObject *parent = nullptr);

    ZigbeeBridgeController *bridgeController() const override;

private:
    enum StartingState {
        StartingStateNone,
        StartingStateErase,
        StartingStateReset,
        StartingStateGetVersion,
        StartingStateSetPanId,
        StartingStateSetChannel,
        StartingStateSetSecurity,
        StartingStateSetNodeType,
        StartingStateStartNetwork
    };

    ZigbeeBridgeControllerNxp *m_controller = nullptr;
    bool m_networkRunning = false;
    bool m_factoryResetting = false;
    StartingState m_startingState = StartingStateNone;

    void setStartingState(StartingState state);
    void readControllerVersion();
    void readPermitJoinStatus();


    ZigbeeNetworkReply *sendRequest(const ZigbeeNetworkRequest &request) override;

protected:
    ZigbeeNode *createNode(QObject *parent) override;
    void setPermitJoiningInternal(bool permitJoining) override;

private slots:
    void onMessageReceived(const ZigbeeInterfaceMessage &message);
    void onControllerAvailableChanged(bool available);

    // Controller command finished slots
    void onCommandResetControllerFinished();
    void onCommandSoftResetControllerFinished();
    void onCommandStartScanFinished();
    //void onCommandEnableWhitelistFinished();

//    void onCommandNodeDescriptorRequestFinished();
//    void onCommandSimpleDescriptorRequestFinished();
//    void onCommandPowerDescriptorRequestFinished();

    void onCommandInitiateTouchLinkFinished();
    void onCommandTouchLinkFactoryResetFinished();
    void onCommandRequestLinkQualityFinished();

    void onCommandRequestMatchDescriptorFinished();
//    void onCommandNetworkAddressRequestFinished();
//    void onCommandAuthenticateDeviceFinished();

    // Process controller notifications/messages
    void processNetworkFormed(const ZigbeeInterfaceMessage &message);
    void processLoggingMessage(const ZigbeeInterfaceMessage &message);
    void processFactoryNewRestart(const ZigbeeInterfaceMessage &message);
    void processNodeClusterList(const ZigbeeInterfaceMessage &message);
    void processNodeAttributeList(const ZigbeeInterfaceMessage &message);
    void processNodeCommandIdList(const ZigbeeInterfaceMessage &message);
    void processDeviceAnnounce(const ZigbeeInterfaceMessage &message);
    void processAttributeReport(const ZigbeeInterfaceMessage &message);
    void processReadAttributeResponse(const ZigbeeInterfaceMessage &message);
    void processLeaveIndication(const ZigbeeInterfaceMessage &message);
    void processRestartProvisioned(const ZigbeeInterfaceMessage &message);
    void processRouterDiscoveryConfirm(const ZigbeeInterfaceMessage &message);
    void processDefaultResponse(const ZigbeeInterfaceMessage &message);

public slots:
    void startNetwork() override;
    void stopNetwork() override;
    void reset() override;
    void factoryResetNetwork() override;

};

#endif // ZIGBEENETWORKNXP_H
