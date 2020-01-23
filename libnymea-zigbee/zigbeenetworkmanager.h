/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
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

#ifndef ZIGBEEMANAGER_H
#define ZIGBEEMANAGER_H

#include <QObject>

#include "zigbeenetwork.h"
#include "zigbeeaddress.h"
#include "zigbeebridgecontroller.h"
#include "zigbeesecurityconfiguration.h"

class ZigbeeNetworkManager : public ZigbeeNetwork
{
    Q_OBJECT
public:
    explicit ZigbeeNetworkManager(QObject *parent = nullptr);

    QString controllerFirmwareVersion() const;

    // Note: temporary public available for debugging
    ZigbeeBridgeController *controller() const;

    bool networkRunning() const;

    // Note: Follwoing methods should be abstract
    bool permitJoining() const;
    void setPermitJoining(bool permitJoining);

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
        StartingStateStartNetwork,
        StartingStateGetPermitJoinStatus,
        StartingStateReadeNodeDescriptor,
        StartingStateReadSimpleDescriptor,
        StartingStateReadPowerDescriptor
    };

    ZigbeeBridgeController *m_controller = nullptr;
    QString m_controllerFirmwareVersion;

    StartingState m_startingState = StartingStateNone;
    void setStartingState(StartingState state);

    QList<ZigbeeNode *> m_uninitializedNodes;

    bool m_permitJoining = false;
    bool m_networkRunning = false;
    bool m_factoryResetting = false;

    void readControllerVersion();
    void readPermitJoinStatus();

signals:
    void permitJoiningChanged(bool permitJoining);

private slots:
    void onMessageReceived(const ZigbeeInterfaceMessage &message);
    void onControllerAvailableChanged(bool available);

    // Controller command finished slots
    void onCommandResetControllerFinished();
    void onCommandSoftResetControllerFinished();
    void onCommandErasePersistentDataFinished();
    void onCommandSetExtendedPanIdFinished();
    void onCommandSetChannelMaskFinished();
    void onCommandSetNodeTypeFinished();
    void onCommandStartNetworkFinished();
    void onCommandStartScanFinished();
    void onCommandEnableWhitelistFinished();

    void onCommandNodeDescriptorRequestFinished();
    void onCommandSimpleDescriptorRequestFinished();
    void onCommandPowerDescriptorRequestFinished();

    void onCommandInitiateTouchLinkFinished();
    void onCommandTouchLinkFactoryResetFinished();
    void onCommandRequestLinkQualityFinished();

    void onCommandRequestMatchDescriptorFinished();
    void onCommandSetSecurityFinished();
    void onCommandNetworkAddressRequestFinished();
    void onCommandAuthenticateDeviceFinished();

    // Process controller notifications/messages
    void processNetworkFormed(const ZigbeeInterfaceMessage &message);
    void processLoggingMessage(const ZigbeeInterfaceMessage &message);
    void processFactoryNewRestart(const ZigbeeInterfaceMessage &message);
    void processNodeClusterList(const ZigbeeInterfaceMessage &message);
    void processNodeAttributeList(const ZigbeeInterfaceMessage &message);
    void processNodeCommandIdList(const ZigbeeInterfaceMessage &message);
    void processDeviceAnnounce(const ZigbeeInterfaceMessage &message);
    void processAttributeReport(const ZigbeeInterfaceMessage &message);
    void processLeaveIndication(const ZigbeeInterfaceMessage &message);
    void processRestartProvisioned(const ZigbeeInterfaceMessage &message);

public slots:
    void startNetwork() override;
    void stopNetwork() override;
    void factoryResetNetwork() override;

};

#endif // ZIGBEEMANAGER_H
