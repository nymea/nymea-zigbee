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

#include "zigbeenetworkrequest.h"
#include "zdo/zigbeedeviceprofile.h"
#include "zigbeeutils.h"

ZigbeeNetworkRequest::ZigbeeNetworkRequest()
{

}

quint8 ZigbeeNetworkRequest::requestId() const
{
    return m_requestId;
}

void ZigbeeNetworkRequest::setRequestId(quint8 requestId)
{
    m_requestId = requestId;
}

Zigbee::DestinationAddressMode ZigbeeNetworkRequest::destinationAddressMode() const
{
    return m_destinationAddressMode;
}

void ZigbeeNetworkRequest::setDestinationAddressMode(Zigbee::DestinationAddressMode destinationAddressMode)
{
    m_destinationAddressMode = destinationAddressMode;
}

quint16 ZigbeeNetworkRequest::destinationShortAddress() const
{
    return m_destinationShortAddress;
}

void ZigbeeNetworkRequest::setDestinationShortAddress(quint16 destinationShortAddress)
{
    m_destinationShortAddress = destinationShortAddress;
}

ZigbeeAddress ZigbeeNetworkRequest::destinationIeeeAddress() const
{
    return m_destinationIeeeAddress;
}

void ZigbeeNetworkRequest::setDestinationIeeeAddress(const ZigbeeAddress &destinationIeeeAddress)
{
    m_destinationIeeeAddress = destinationIeeeAddress;
}

quint8 ZigbeeNetworkRequest::destinationEndpoint() const
{
    return m_destinationEndpoint;
}

void ZigbeeNetworkRequest::setDestinationEndpoint(quint8 destinationEndpoint)
{
    m_destinationEndpoint = destinationEndpoint;
}

quint16 ZigbeeNetworkRequest::profileId() const
{
    return m_profileId;
}

void ZigbeeNetworkRequest::setProfileId(quint16 profileId)
{
    m_profileId = profileId;
}

quint16 ZigbeeNetworkRequest::clusterId() const
{
    return m_clusterId;
}

void ZigbeeNetworkRequest::setClusterId(quint16 clusterId)
{
    m_clusterId = clusterId;
}

quint8 ZigbeeNetworkRequest::sourceEndpoint() const
{
    return m_sourceEndpoint;
}

void ZigbeeNetworkRequest::setSourceEndpoint(quint8 sourceEndpoint)
{
    m_sourceEndpoint = sourceEndpoint;
}

QByteArray ZigbeeNetworkRequest::asdu() const
{
    return m_asdu;
}

void ZigbeeNetworkRequest::setAsdu(const QByteArray &asdu)
{
    m_asdu = asdu;
}

Zigbee::ZigbeeTxOptions ZigbeeNetworkRequest::txOptions() const
{
    return m_txOptions;
}

void ZigbeeNetworkRequest::setTxOptions(Zigbee::ZigbeeTxOptions txOptions)
{
    m_txOptions = txOptions;
}

quint8 ZigbeeNetworkRequest::radius() const
{
    return m_radius;
}

void ZigbeeNetworkRequest::setRadius(quint8 radius)
{
    m_radius = radius;
}

QDebug operator<<(QDebug debug, const ZigbeeNetworkRequest &request)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "Request(ID:" << request.requestId() << ", ";
    debug.nospace() << static_cast<Zigbee::ZigbeeProfile>(request.profileId()) << ", ";
    if (request.profileId() == Zigbee::ZigbeeProfileDevice) {
        debug.nospace() << static_cast<ZigbeeDeviceProfile::ZdoCommand>(request.clusterId()) << ", ";
    } else {
        debug.nospace() << static_cast<ZigbeeClusterLibrary::ClusterId>(request.clusterId()) << ", ";
    }

    if (request.destinationAddressMode() == Zigbee::DestinationAddressModeGroup)
        debug.nospace() << "Group address:" << ZigbeeUtils::convertUint16ToHexString(request.destinationShortAddress()) << ", ";

    if (request.destinationAddressMode() == Zigbee::DestinationAddressModeShortAddress)
        debug.nospace() << "NWK address:" << ZigbeeUtils::convertUint16ToHexString(request.destinationShortAddress()) << ", ";

    if (request.destinationAddressMode() == Zigbee::DestinationAddressModeIeeeAddress)
        debug.nospace() << "IEEE address:" << ZigbeeAddress(request.destinationIeeeAddress()).toString() << ", ";

    debug.nospace() << "Destination EP:" << ZigbeeUtils::convertByteToHexString(request.destinationEndpoint()) << ", ";
    debug.nospace() << "Source EP:" << ZigbeeUtils::convertByteToHexString(request.sourceEndpoint()) << ", ";
    debug.nospace() << "Radius:" << request.radius() << ", ";
    debug.nospace() << request.txOptions() << ", ";
    debug.nospace() << ZigbeeUtils::convertByteArrayToHexString(request.asdu());
    debug.nospace() << ")";
    return debug;
}
