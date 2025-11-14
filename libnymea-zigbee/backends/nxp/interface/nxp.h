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

#ifndef NXP_H
#define NXP_H

#include <QObject>

class Nxp
{
    Q_GADGET
public:
    enum Command {
        CommandGetVersion = 0x00,
        CommandGetControllerState = 0x01,
        CommandSoftReset = 0x02,
        CommandFactoryReset = 0x03,

        CommandSetPanId = 0x04,
        CommandSetChannelMask = 0x05,
        CommandSetSecurityKey = 0x06,

        CommandStartNetwork = 0x07,
        CommandGetNetworkState = 0x08,
        CommandSetPermitJoinCoordinator = 0x09,

        CommandSendApsDataRequest = 0x20
    };
    Q_ENUM(Command)

    enum Notification {
        NotificationDeviceStatusChanged = 0x7D,
        NotificationNetworkStarted = 0x7E,
        NotificationApsDataConfirm = 0x80,
        NotificationApsDataIndication = 0x81,
        NotificationApsDataAck = 0x82,
        NotificationNodeJoined = 0x90,
        NotificationNodeLeft = 0x91,
        NotificationDebugMessage = 0xFE
    };
    Q_ENUM(Notification)

    enum Status {
        StatusSuccess = 0x00,
        StatusProtocolError = 0x01,
        StatusUnknownCommand = 0x02,
        StatusInvalidCrc = 0x03,
        StatusStackError = 0x04,
        StatusTimeout = 0xff
    };
    Q_ENUM(Status)

    enum LogLevel {
        LogLevelEmergency = 0x00,
        LogLevelAlert = 0x01,
        LogLevelCritical = 0x02,
        LogLevelError = 0x03,
        LogLevelWarning = 0x04,
        LogLevelNotice = 0x05,
        LogLevelInfo = 0x06,
        LogLevelDebug = 0x07
    };
    Q_ENUM(LogLevel)

    enum KeyType {
        KeyTypeGlobalLinkKey = 0x00,
        KeyTypeUniqueLinkKey = 0x01
    };
    Q_ENUM(KeyType)
};

#endif // NXP_H
