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

#include "zigbeeclusterreply.h"

ZigbeeClusterReply::ZigbeeClusterReply(const ZigbeeNetworkRequest &request, ZigbeeClusterLibrary::Frame requestFrame, QObject *parent) :
    QObject(parent),
    m_request(request),
    m_requestFrame(requestFrame)
{
    m_timeoutTimer.setInterval(20000);
    connect(&m_timeoutTimer, &QTimer::timeout, this, [this](){
        m_error = ErrorTimeout;
        emit finished();
    });
}

ZigbeeClusterReply::Error ZigbeeClusterReply::error() const
{
    return m_error;
}

Zigbee::ZigbeeApsStatus ZigbeeClusterReply::zigbeeApsStatus() const
{
    return m_zigbeeApsStatus;
}

Zigbee::ZigbeeNwkLayerStatus ZigbeeClusterReply::zigbeeNwkStatus() const
{
    return m_zigbeeNwkStatus;
}

Zigbee::ZigbeeMacLayerStatus ZigbeeClusterReply::zigbeeMacStatus() const
{
    return m_zigbeeMacStatus;
}

ZigbeeNetworkRequest ZigbeeClusterReply::request() const
{
    return m_request;
}

ZigbeeClusterLibrary::Frame ZigbeeClusterReply::requestFrame() const
{
    return m_requestFrame;
}

quint8 ZigbeeClusterReply::transactionSequenceNumber() const
{
    return m_transactionSequenceNumber;
}

QByteArray ZigbeeClusterReply::responseData() const
{
    return m_responseData;
}

ZigbeeClusterLibrary::Frame ZigbeeClusterReply::responseFrame() const
{
    return m_responseFrame;
}

bool ZigbeeClusterReply::isComplete() const
{
    return m_apsConfirmReceived && m_zclIndicationReceived;
}
