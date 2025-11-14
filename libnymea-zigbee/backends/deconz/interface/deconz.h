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

#ifndef DECONZ_H
#define DECONZ_H

#include <QObject>

class Deconz
{
    Q_GADGET

public:
    enum StatusCode {
        StatusCodeSuccess = 0x00,
        StatusCodeFailure = 0x01,
        StatusCodeBusy = 0x02,
        StatusCodeTimeout = 0x03,
        StatusCodeUnsupported = 0x04,
        StatusCodeError = 0x05,
        StatusCodeNoNetwork = 0x06,
        StatusCodeInvalidValue = 0x07
    };
    Q_ENUM(StatusCode)

    enum NetworkState {
        NetworkStateOffline = 0x00,
        NetworkStateJoining = 0x01,
        NetworkStateConnected = 0x02,
        NetworkStateLeaving = 0x03
    };
    Q_ENUM(NetworkState)

    enum Command {
        CommandDeviceState = 0x07,
        CommandChangeNetworkState = 0x08,
        CommandReadParameter = 0x0A,
        CommandWriteParameter = 0x0B,
        CommandDeviceStateChanged = 0x0E,
        CommandVersion = 0x0D,
        CommandApsDataRequest = 0x12,
        CommandApsDataConfirm = 0x04,
        CommandApsDataIndication = 0x17,
        CommandGreenPower = 0x19,
        CommandMacPoll = 0x1C,
        CommandMacBeacon = 0x1F,
        CommandUpdateBootloader = 0x21
    };
    Q_ENUM(Command)

    enum Parameter {
        ParameterMacAddress = 0x01, // R
        ParameterPanId = 0x05, // RW
        ParameterNetworkAddress = 0x07, // R
        ParameterNetworkExtendedPanId = 0x08, // R
        ParameterNodeType = 0x09, //RW
        ParameterChannelMask = 0x0A, // RW
        ParameterApsExtendedPanId = 0x0B, //RW
        ParameterTrustCenterAddress = 0x0E, // RW
        ParameterSecurityMode = 0x10, // RW
        ParameterPredefinedNwkPanId = 0x15, // RW
        ParameterNetworkKey = 0x18, // RW
        ParameterLinkKey = 0x19, // RW
        ParameterCurrentChannel = 0x1c, // R
        ParameterPermitJoin = 0x21, // RW
        ParameterProtocolVersion = 0x22, // R
        ParameterNetworkUpdateId = 0x24, // RW
        ParameterWatchdogTtl = 0x26, // RW since protocol version 0x0108
        ParameterNetworkFrameCounter = 0x27 // RW
    };
    Q_ENUM(Parameter)

    enum NodeType {
        NodeTypeRouter = 0x00,
        NodeTypeCoordinator = 0x01
    };
    Q_ENUM(NodeType)

    enum SourceAddressMode {
        SourceAddressModeNone = 0x00,
        SourceAddressModeShortSourceAddress = 0x01,
        SourceAddressModeAddLastHoppAddress = 0x02, // since 0x0108
        SourceAddressModeIeeeSourceAddress = 0x03,
        SourceAddressModeShortAndIeeeSourceAddress = 0x04 // since 0x010B
    };
    Q_ENUM(SourceAddressMode)


    enum SecurityMode {
        SecurityModeNoSecurity = 0x00,
        SecurityModePreconfiguredNetworkKey = 0x01,
        SecurityModeNetworkKeyFromTrustCenter = 0x02,
        SecurityModeNoMasterButTrustCenterKey = 0x03
    };
    Q_ENUM(SecurityMode)

    enum Platform {
        PlatformConbeeRaspbee = 0x05,
        PlatformConbeeII = 0x07
    };
    Q_ENUM(Platform)

};


#endif // DECONZ_H
