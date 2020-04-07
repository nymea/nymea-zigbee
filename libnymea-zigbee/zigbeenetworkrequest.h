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

#ifndef ZIGBEENETWORKREQUEST_H
#define ZIGBEENETWORKREQUEST_H

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

#endif // ZIGBEENETWORKREQUEST_H
