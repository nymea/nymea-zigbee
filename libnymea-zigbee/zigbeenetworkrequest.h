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

#ifndef ZIGBEENETWORKREQUEST_H
#define ZIGBEENETWORKREQUEST_H

#include <QDebug>
#include <QObject>

#include "zigbee.h"
#include "zigbeeaddress.h"

class ZigbeeNetworkRequest
{
public:
    ZigbeeNetworkRequest();

    quint8 requestId() const;
    void setRequestId(quint8 requestId);

    Zigbee::DestinationAddressMode destinationAddressMode() const;
    void setDestinationAddressMode(Zigbee::DestinationAddressMode destinationAddressMode);

    quint16 destinationShortAddress() const;
    void setDestinationShortAddress(quint16 destinationShortAddress);

    ZigbeeAddress destinationIeeeAddress() const;
    void setDestinationIeeeAddress(const ZigbeeAddress &destinationIeeeAddress);

    quint8 destinationEndpoint() const;
    void setDestinationEndpoint(quint8 destinationEndpoint);

    quint16 profileId() const;
    void setProfileId(quint16 profileId);

    quint16 clusterId() const;
    void setClusterId(quint16 clusterId);

    quint8 sourceEndpoint() const;
    void setSourceEndpoint(quint8 sourceEndpoint);

    QByteArray asdu() const;
    void setAsdu(const QByteArray &asdu);

    Zigbee::ZigbeeTxOptions txOptions() const;
    void setTxOptions(Zigbee::ZigbeeTxOptions txOptions);

    quint8 radius() const;
    void setRadius(quint8 radius);

private:
    quint8 m_requestId = 0;
    Zigbee::DestinationAddressMode m_destinationAddressMode = Zigbee::DestinationAddressModeShortAddress;
    quint16 m_destinationShortAddress = 0;
    ZigbeeAddress m_destinationIeeeAddress;
    quint8 m_destinationEndpoint = 0;
    quint16 m_profileId = 0;
    quint16 m_clusterId = 0;
    quint8 m_sourceEndpoint = 0;
    QByteArray m_asdu;
    Zigbee::ZigbeeTxOptions m_txOptions = Zigbee::ZigbeeTxOptions(Zigbee::ZigbeeTxOptionAckTransmission);
    quint8 m_radius = 0;

};

QDebug operator<<(QDebug debug, const ZigbeeNetworkRequest &request);


#endif // ZIGBEENETWORKREQUEST_H
