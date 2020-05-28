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

#include "zigbee.h"
#include "zigbeeutils.h"
#include "zdo/zigbeedeviceprofile.h"

QDebug operator<<(QDebug debug, const Zigbee::ApsdeDataConfirm &confirm)
{
    debug.nospace() << "APSDE-DATA.confirm(";
    debug.nospace() << "Request ID: " << confirm.requestId << ", ";

    if (confirm.destinationAddressMode == Zigbee::DestinationAddressModeGroup)
        debug.nospace() << "Group address:" << ZigbeeUtils::convertUint16ToHexString(confirm.destinationShortAddress) << ", ";

    if (confirm.destinationAddressMode == Zigbee::DestinationAddressModeShortAddress)
        debug.nospace() << "NWK address:" << ZigbeeUtils::convertUint16ToHexString(confirm.destinationShortAddress) << ", ";

    if (confirm.destinationAddressMode == Zigbee::DestinationAddressModeIeeeAddress)
        debug.nospace() << "IEEE address:" << ZigbeeAddress(confirm.destinationIeeeAddress).toString() << ", ";

    debug.nospace() << "Destination EP:" << ZigbeeUtils::convertByteToHexString(confirm.destinationEndpoint) << ", ";
    debug.nospace() << "Source EP:" << ZigbeeUtils::convertByteToHexString(confirm.sourceEndpoint) << ", ";
    debug.nospace() << static_cast<Zigbee::ZigbeeStatus>(confirm.zigbeeStatusCode);
    debug.nospace() << ")";

    return debug.space();
}

QDebug operator<<(QDebug debug, const Zigbee::ApsdeDataIndication &indication)
{
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
        debug.nospace() << static_cast<Zigbee::ClusterId>(indication.clusterId) << ", ";
    }

    debug.nospace() << "ASDU: " << ZigbeeUtils::convertByteArrayToHexString(indication.asdu) << ", ";
    debug.nospace() << "LQI: " << indication.lqi << ", ";
    debug.nospace() << "RSSI: " << indication.rssi << "dBm)";
    return debug.space();
}
