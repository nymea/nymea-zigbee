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

#include "zigbeedeviceobjectreply.h"

#include <QTimer>

ZigbeeDeviceObjectReply::ZigbeeDeviceObjectReply(const ZigbeeNetworkRequest &request, QObject *parent) :
    QObject(parent),
    m_request(request)
{
    m_timeoutTimer.setInterval(20000);
    connect(&m_timeoutTimer, &QTimer::timeout, this, [this](){
        m_error = ErrorTimeout;
        emit finished();
    });
}

void ZigbeeDeviceObjectReply::setZigbeeApsStatus(Zigbee::ZigbeeApsStatus status)
{
    m_zigbeeApsStatus = status;
    if (status != Zigbee::ZigbeeApsStatusSuccess) {
        m_error = ErrorZigbeeApsStatusError;
    }
}

void ZigbeeDeviceObjectReply::setZigbeeNwkLayerStatus(Zigbee::ZigbeeNwkLayerStatus status)
{
    m_zigbeeNwkStatus = status;
    if (status != Zigbee::ZigbeeNwkLayerStatusSuccess) {
        m_error = ErrorZigbeeNwkStatusError;
    }
}

void ZigbeeDeviceObjectReply::setZigbeeMacLayerStatus(Zigbee::ZigbeeMacLayerStatus status)
{
    m_zigbeeMacStatus = status;
    if (status != Zigbee::ZigbeeMacLayerStatusSuccess) {
        m_error = ErrorZigbeeMacStatusError;
    }
}

void ZigbeeDeviceObjectReply::setZigbeeDeviceObjectStatus(ZigbeeDeviceProfile::Status status)
{
    m_zigbeeDeviceObjectStatus = status;
    if (status != ZigbeeDeviceProfile::StatusSuccess) {
        m_error = ErrorZigbeeDeviceObjectStatusError;
    }
}

ZigbeeDeviceObjectReply::Error ZigbeeDeviceObjectReply::error() const
{
    return m_error;
}

Zigbee::ZigbeeApsStatus ZigbeeDeviceObjectReply::zigbeeApsStatus() const
{
    return m_zigbeeApsStatus;
}

Zigbee::ZigbeeNwkLayerStatus ZigbeeDeviceObjectReply::zigbeeNwkStatus() const
{
    return m_zigbeeNwkStatus;
}

Zigbee::ZigbeeMacLayerStatus ZigbeeDeviceObjectReply::zigbeeMacStatus() const
{
    return m_zigbeeMacStatus;
}

ZigbeeDeviceProfile::Status ZigbeeDeviceObjectReply::zigbeeDeviceObjectStatus() const
{
    return m_zigbeeDeviceObjectStatus;
}

ZigbeeNetworkRequest ZigbeeDeviceObjectReply::request() const
{
    return m_request;
}

quint8 ZigbeeDeviceObjectReply::transactionSequenceNumber() const
{
    return m_transactionSequenceNumber;
}

ZigbeeDeviceProfile::ZdoCommand ZigbeeDeviceObjectReply::expectedResponse() const
{
    return m_expectedResponse;
}

QByteArray ZigbeeDeviceObjectReply::responseData() const
{
    return m_responseData;
}

ZigbeeDeviceProfile::Adpu ZigbeeDeviceObjectReply::responseAdpu() const
{
    return m_responseAdpu;
}

bool ZigbeeDeviceObjectReply::isComplete() const
{
    return m_apsConfirmReceived && m_zdpIndicationReceived;
}
