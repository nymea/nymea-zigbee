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

#ifndef ZIGBEEBRIDGECONTROLLERNXP_H
#define ZIGBEEBRIDGECONTROLLERNXP_H

#include <QHash>
#include <QTimer>
#include <QQueue>
#include <QObject>

#include "zigbee.h"
#include "zigbeeaddress.h"
#include "zigbeenetworkkey.h"
#include "zigbeenetworkrequest.h"
#include "zigbeebridgecontroller.h"
#include "firmwareupdatehandlernxp.h"
#include "interface/zigbeeinterfacenxp.h"
#include "interface/zigbeeinterfacenxpreply.h"

class ZigbeeBridgeControllerNxp : public ZigbeeBridgeController
{
    Q_OBJECT

    friend class ZigbeeNetworkNxp;

public:
    explicit ZigbeeBridgeControllerNxp(QObject *parent = nullptr);
    ~ZigbeeBridgeControllerNxp() override;

    enum ControllerState {
        ControllerStateRunning = 0x00,
        ControllerStateBooting = 0x01,
        ControllerStateStarting = 0x02,
        ControllerStateRunningUninitialized = 0x03,
        ControllerStateNotRunning = 0x04
    };
    Q_ENUM(ControllerState)

    enum SecurityMode {
        SecurityModeUnsecure = 0x00,
        SecurityModeSecure = 0x01,
        SecurityModeSecureNwk = 0x02,
        SecurityModeExtendedNonde = 0x10,
        SecurityModeWildProfile = 0x20
    };
    Q_ENUM(SecurityMode)

    ControllerState controllerState() const;
    void refreshControllerState();

    // Controllere requests
    ZigbeeInterfaceNxpReply *requestVersion();
    ZigbeeInterfaceNxpReply *requestControllerState();
    ZigbeeInterfaceNxpReply *requestSoftResetController();
    ZigbeeInterfaceNxpReply *requestFactoryResetController();

    // Configure network
    ZigbeeInterfaceNxpReply *requestSetPanId(quint64 panId);
    ZigbeeInterfaceNxpReply *requestSetChannelMask(quint32 channelMask);
    ZigbeeInterfaceNxpReply *requestSetSecurityKey(Nxp::KeyType keyType, const ZigbeeNetworkKey &key);

    // Network commands
    ZigbeeInterfaceNxpReply *requestStartNetwork();
    ZigbeeInterfaceNxpReply *requestNetworkState();
    ZigbeeInterfaceNxpReply *requestSetPermitJoinCoordinator(quint8 duration);

    // APS
    ZigbeeInterfaceNxpReply *requestSendRequest(const ZigbeeNetworkRequest &request);

    bool updateAvailable(const QString &currentVersion) override;
    QString updateFirmwareVersion() const override;
    void startFirmwareUpdate() override;
    void startFactoryResetUpdate() override;

signals:
    void controllerStateChanged(ControllerState controllerState);
    void interfaceNotificationReceived(Nxp::Notification notification, const QByteArray &data);
    void nodeLeft(const ZigbeeAddress &address, bool rejoining);

private:
    ZigbeeInterfaceNxp *m_interface = nullptr;
    FirmwareUpdateHandlerNxp *m_firmwareUpdateHandler = nullptr;
    QString m_serialPort;
    qint32 m_baudrate;

    ControllerState m_controllerState = ControllerStateNotRunning;
    quint8 m_sequenceNumber = 0;

    ZigbeeInterfaceNxpReply *m_currentReply = nullptr;
    QQueue<ZigbeeInterfaceNxpReply *> m_replyQueue;
    ZigbeeInterfaceNxpReply *createReply(Nxp::Command command, quint8 sequenceNumber, const QString &requestName, const QByteArray &requestData, QObject *parent);

    void bumpSequenceNumber();

    ZigbeeInterfaceNxpReply *requestEnqueueSendDataGroup(quint8 requestId, quint16 groupAddress, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius = 0);
    ZigbeeInterfaceNxpReply *requestEnqueueSendDataShortAddress(quint8 requestId, quint16 shortAddress, quint8 destinationEndpoint, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius = 0);
    ZigbeeInterfaceNxpReply *requestEnqueueSendDataIeeeAddress(quint8 requestId, ZigbeeAddress ieeeAddress, quint8 destinationEndpoint, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius = 0);

protected:
    void initializeUpdateProvider() override;

private slots:
    void onInterfaceAvailableChanged(bool available);
    void onInterfacePackageReceived(const QByteArray &package);

    void sendNextRequest();

public slots:
    bool enable(const QString &serialPort, qint32 baudrate);
    void disable();

};

#endif // ZIGBEEBRIDGECONTROLLERNXP_H
