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

#include "zigbee.h"
#include "zigbeeutils.h"
#include "zdo/zigbeedeviceprofile.h"

QDebug operator<<(QDebug debug, const Zigbee::ApsdeDataConfirm &confirm)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "APSDE-DATA.confirm(";
    debug.nospace() << "SQN: " << confirm.requestId << ", ";

    if (confirm.destinationAddressMode == Zigbee::DestinationAddressModeGroup)
        debug.nospace() << "Group address:" << ZigbeeUtils::convertUint16ToHexString(confirm.destinationShortAddress) << ", ";

    if (confirm.destinationAddressMode == Zigbee::DestinationAddressModeShortAddress)
        debug.nospace() << "NWK address:" << ZigbeeUtils::convertUint16ToHexString(confirm.destinationShortAddress) << ", ";

    if (confirm.destinationAddressMode == Zigbee::DestinationAddressModeIeeeAddress)
        debug.nospace() << "IEEE address:" << ZigbeeAddress(confirm.destinationIeeeAddress).toString() << ", ";

    debug.nospace() << "Destination EP:" << ZigbeeUtils::convertByteToHexString(confirm.destinationEndpoint) << ", ";
    debug.nospace() << "Source EP:" << ZigbeeUtils::convertByteToHexString(confirm.sourceEndpoint) << ", ";
    debug.nospace() << "Status:" << ZigbeeUtils::zigbeeStatusToString(confirm.zigbeeStatusCode);
    debug.nospace() << ")";

    return debug;
}

QDebug operator<<(QDebug debug, const Zigbee::ApsdeDataIndication &indication)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "APSDE-DATA.indication(";
    if (indication.destinationAddressMode == Zigbee::DestinationAddressModeGroup)
        debug.nospace() << "Group address:" << ZigbeeUtils::convertUint16ToHexString(indication.destinationShortAddress) << ", ";

    if (indication.destinationAddressMode == Zigbee::DestinationAddressModeShortAddress)
        debug.nospace() << "NWK address:" << ZigbeeUtils::convertUint16ToHexString(indication.destinationShortAddress) << ", ";

    if (indication.destinationAddressMode == Zigbee::DestinationAddressModeIeeeAddress)
        debug.nospace() << "IEEE address:" << ZigbeeAddress(indication.destinationIeeeAddress).toString() << ", ";

    debug.nospace() << "Destination EP:" << ZigbeeUtils::convertByteToHexString(indication.destinationEndpoint) << ", ";
    debug.nospace() << "Source EP:" << ZigbeeUtils::convertByteToHexString(indication.sourceEndpoint) << ", ";

    if (indication.sourceAddressMode == Zigbee::SourceAddressModeShortAddress || indication.sourceAddressMode == Zigbee::SourceAddressModeShortAndIeeeAddress)
        debug.nospace() << "Source NWK address:" << ZigbeeUtils::convertUint16ToHexString(indication.sourceShortAddress) << ", ";

    if (indication.sourceAddressMode == Zigbee::SourceAddressModeIeeeAddress || indication.sourceAddressMode == Zigbee::SourceAddressModeShortAndIeeeAddress)
        debug.nospace() << "Source IEEE address:" << ZigbeeAddress(indication.sourceIeeeAddress).toString() << ", ";

    debug.nospace() << static_cast<Zigbee::ZigbeeProfile>(indication.profileId) << ", ";
    if (indication.profileId == static_cast<quint16>(Zigbee::ZigbeeProfileDevice)) {
        debug.nospace() << static_cast<ZigbeeDeviceProfile::ZdoCommand>(indication.clusterId) << ", ";
    } else {
        debug.nospace() << static_cast<ZigbeeClusterLibrary::ClusterId>(indication.clusterId) << ", ";
    }

    debug.nospace() << "ASDU: " << ZigbeeUtils::convertByteArrayToHexString(indication.asdu) << ", ";
    debug.nospace() << "LQI: " << indication.lqi << ", ";
    debug.nospace() << "RSSI: " << indication.rssi << "dBm)";
    return debug;
}

QDebug operator<<(QDebug debug, const Zigbee::ApsdeDataAck &acknowledgement)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "APSDE-DATA.acknowledgement(";
    debug.nospace() << "SQN: " << acknowledgement.requestId << ", ";
    if (acknowledgement.destinationAddressMode == Zigbee::DestinationAddressModeGroup) {
        debug.nospace() << "Group address:" << ZigbeeUtils::convertUint16ToHexString(acknowledgement.destinationAddress) << ", ";
    } else {
        debug.nospace() << "Network address:" << ZigbeeUtils::convertUint16ToHexString(acknowledgement.destinationAddress) << ", ";
    }
    debug.nospace() << "Destination EP:" << ZigbeeUtils::convertByteToHexString(acknowledgement.destinationEndpoint) << ", ";
    debug.nospace() << "Source EP:" << ZigbeeUtils::convertByteToHexString(acknowledgement.sourceEndpoint) << ", ";
    debug.nospace() << static_cast<Zigbee::ZigbeeProfile>(acknowledgement.profileId) << ", ";
    if (acknowledgement.profileId == static_cast<quint16>(Zigbee::ZigbeeProfileDevice)) {
        debug.nospace() << static_cast<ZigbeeDeviceProfile::ZdoCommand>(acknowledgement.clusterId) << ", ";
    } else {
        debug.nospace() << static_cast<ZigbeeClusterLibrary::ClusterId>(acknowledgement.clusterId) << ", ";
    }
    debug.nospace() << "Status:" << ZigbeeUtils::zigbeeStatusToString(acknowledgement.zigbeeStatusCode);
    debug.nospace() << ")";
    return debug;
}
