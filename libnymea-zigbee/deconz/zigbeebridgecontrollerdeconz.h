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

#ifndef ZIGBEEBRIDGECONTROLLERDECONZ_H
#define ZIGBEEBRIDGECONTROLLERDECONZ_H

#include <QHash>
#include <QObject>
#include <QTimer>

#include "zigbee.h"
#include "zigbeeaddress.h"
#include "zigbeenetworkkey.h"
#include "zigbeebridgecontroller.h"

#include "interface/deconz.h"
#include "interface/zigbeeinterfacedeconz.h"
#include "interface/zigbeeinterfacedeconzreply.h"

typedef struct DeconzNetworkConfiguration {
    ZigbeeAddress ieeeAddress; // R
    quint16 panId; // R
    quint16 shortAddress; // R
    quint64 extendedPanId; // R
    Deconz::NodeType nodeType; // RW
    quint32 channelMask; // RW
    quint64 apsExtendedPanId; // RW
    ZigbeeAddress trustCenterAddress; // RW
    Deconz::SecurityMode securityMode; // RW
    ZigbeeNetworkKey networkKey; // RW
    quint8 currentChannel; // R
    quint16 protocolVersion; // R
    quint8 networkUpdateId; // RW
    quint32 watchdogTimeout; // RW
} DeconzNetworkConfiguration;


class ZigbeeBridgeControllerDeconz : public ZigbeeBridgeController
{
    Q_OBJECT

    friend class ZigbeeNetworkDeconz;

public:
    explicit ZigbeeBridgeControllerDeconz(QObject *parent = nullptr);
    ~ZigbeeBridgeControllerDeconz() override;

    ZigbeeInterfaceDeconzReply *requestVersion();
    ZigbeeInterfaceDeconzReply *requestDeviceState();
    ZigbeeInterfaceDeconzReply *requestReadParameter(Deconz::Parameter parameter);
    ZigbeeInterfaceDeconzReply *requestWriteParameter(Deconz::Parameter parameter, const QByteArray &data);
    ZigbeeInterfaceDeconzReply *requestStartJoinNetwork();

private:
    ZigbeeInterfaceDeconz *m_interface = nullptr;
    quint8 m_sequenceNumber = 0;
    quint32 m_watchdogTimeout = 85;
    int m_watchdogResetTimout = 60;
    QHash<quint8, ZigbeeInterfaceDeconzReply *> m_pendingReplies;
    DeconzNetworkConfiguration m_networkConfiguration;
    QTimer *m_watchdogTimer = nullptr;

    quint8 generateSequenceNumber();

    ZigbeeInterfaceDeconzReply *createReply(Deconz::Command command, quint8 sequenceNumber, QObject *parent);

    // Note: this method reads all parameters individual. The returned reply it self will not send or receive any data.
    // The data can be fetched from m_networkConfiguration on success.
    ZigbeeInterfaceDeconzReply *readNetworkParameters();

    ZigbeeInterfaceDeconzReply *resetWatchdog();

signals:

private slots:
    void onInterfaceAvailableChanged(bool available);
    void onInterfacePackageReceived(const QByteArray &package);

    void onWatchdogTimerTimeout();

public slots:
    bool enable(const QString &serialPort, qint32 baudrate);
    void disable();
};

#endif // ZIGBEEBRIDGECONTROLLERDECONZ_H
