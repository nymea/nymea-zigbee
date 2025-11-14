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

#include "zigbeenetworkreply.h"

ZigbeeNetworkReply::Error ZigbeeNetworkReply::error() const
{
    return m_error;
}

ZigbeeNetworkRequest ZigbeeNetworkReply::request() const
{
    return m_request;
}

Zigbee::ZigbeeMacLayerStatus ZigbeeNetworkReply::zigbeeMacStatus() const
{
    return m_zigbeeMacStatus;
}

Zigbee::ZigbeeApsStatus ZigbeeNetworkReply::zigbeeApsStatus() const
{
    return m_zigbeeApsStatus;
}

Zigbee::ZigbeeNwkLayerStatus ZigbeeNetworkReply::zigbeeNwkStatus() const
{
    return m_zigbeeNwkStatus;
}

ZigbeeNetworkReply::ZigbeeNetworkReply(const ZigbeeNetworkRequest &request, QObject *parent) :
    QObject(parent),
    m_request(request)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(20000);
    connect(m_timer, &QTimer::timeout, this, [this](){
        m_error = ErrorTimeout;
        emit finished();
    });
}

