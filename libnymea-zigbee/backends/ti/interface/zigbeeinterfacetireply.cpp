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

#include "zigbeeinterfacetireply.h"

quint8 ZigbeeInterfaceTiReply::command() const
{
    return m_command;
}

Ti::SubSystem ZigbeeInterfaceTiReply::subSystem() const
{
    return m_subSystem;
}

QByteArray ZigbeeInterfaceTiReply::requestPayload() const
{
    return m_requestPayload;
}

QByteArray ZigbeeInterfaceTiReply::responsePayload() const
{
    return m_responsePayload;
}

Ti::StatusCode ZigbeeInterfaceTiReply::statusCode() const
{
    return m_statusCode;
}

bool ZigbeeInterfaceTiReply::timedOut() const
{
    return m_timeout;
}

bool ZigbeeInterfaceTiReply::aborted() const
{
    return m_aborted;
}

void ZigbeeInterfaceTiReply::abort()
{
    m_timer->stop();
    m_aborted = true;
    emit finished();
}

ZigbeeInterfaceTiReply::ZigbeeInterfaceTiReply(QObject *parent, int timeout):
    QObject(parent),
    m_timer(new QTimer(this))
{
    m_timer->setInterval(timeout);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &ZigbeeInterfaceTiReply::onTimeout);

    // We'll auto-delete ourselves.
    connect(this, &ZigbeeInterfaceTiReply::finished, this, &QObject::deleteLater, Qt::QueuedConnection);
}

ZigbeeInterfaceTiReply::ZigbeeInterfaceTiReply(Ti::SubSystem subSystem, quint8 command, QObject *parent, const QByteArray &requestPayload, int timeout) :
    ZigbeeInterfaceTiReply(parent, timeout)
{
    m_subSystem = subSystem;
    m_command = command;
    m_requestPayload = requestPayload;
}

void ZigbeeInterfaceTiReply::finish(Ti::StatusCode statusCode)
{
    m_statusCode = statusCode;
    emit finished();
}

void ZigbeeInterfaceTiReply::onTimeout()
{
    m_timeout = true;
    emit timeout();
    emit finished();
}
