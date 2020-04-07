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

#include "zigbeenetworkrequest.h"

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
