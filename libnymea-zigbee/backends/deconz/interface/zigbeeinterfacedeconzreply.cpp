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

#include "zigbeeinterfacedeconzreply.h"

ZigbeeNetworkRequest ZigbeeInterfaceDeconzReply::networkRequest() const
{
    return m_networkRequest;
}

QString ZigbeeInterfaceDeconzReply::requestName()
{
    return m_requestName;
}

Deconz::Command ZigbeeInterfaceDeconzReply::command() const
{
    return m_command;
}

quint8 ZigbeeInterfaceDeconzReply::sequenceNumber() const
{
    return m_sequenceNumber;
}

QByteArray ZigbeeInterfaceDeconzReply::requestData() const
{
    return m_requestData;
}

QByteArray ZigbeeInterfaceDeconzReply::responseData() const
{
    return m_responseData;
}

Deconz::StatusCode ZigbeeInterfaceDeconzReply::statusCode() const
{
    return m_statusCode;
}

bool ZigbeeInterfaceDeconzReply::timendOut() const
{
    return m_timeout;
}

bool ZigbeeInterfaceDeconzReply::aborted() const
{
    return m_aborted;
}

void ZigbeeInterfaceDeconzReply::abort()
{
    m_timer->stop();
    m_aborted = true;
    emit finished();
}

ZigbeeInterfaceDeconzReply::ZigbeeInterfaceDeconzReply(Deconz::Command command, QObject *parent) :
    QObject(parent),
    m_timer(new QTimer(this)),
    m_command(command)
{
    m_timer->setInterval(10000);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &ZigbeeInterfaceDeconzReply::onTimeout);
}

void ZigbeeInterfaceDeconzReply::setSequenceNumber(quint8 sequenceNumber)
{
    m_sequenceNumber = sequenceNumber;
    // Put the sequence number into the request data payload, it's always the second byte
    m_requestData[1] = m_sequenceNumber;
}

void ZigbeeInterfaceDeconzReply::onTimeout()
{
    m_timeout = true;
    emit timeout();
    emit finished();
}

QDebug operator<<(QDebug debug, ZigbeeInterfaceDeconzReply *reply)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "InterfaceReply(" << reply->requestName() << ", " << reply->sequenceNumber() << ")";
    return debug;
}
