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

#include "zigbeeinterfacenxpreply.h"

ZigbeeNetworkRequest ZigbeeInterfaceNxpReply::networkRequest() const
{
    return m_networkRequest;
}

QString ZigbeeInterfaceNxpReply::requestName()
{
    return m_requestName;
}

Nxp::Command ZigbeeInterfaceNxpReply::command() const
{
    return m_command;
}

quint8 ZigbeeInterfaceNxpReply::sequenceNumber() const
{
    return m_sequenceNumber;
}

QByteArray ZigbeeInterfaceNxpReply::requestData() const
{
    return m_requestData;
}

QByteArray ZigbeeInterfaceNxpReply::responseData() const
{
    return m_responseData;
}

Nxp::Status ZigbeeInterfaceNxpReply::status() const
{
    return m_status;
}

bool ZigbeeInterfaceNxpReply::timendOut() const
{
    return m_timeout;
}

bool ZigbeeInterfaceNxpReply::aborted() const
{
    return m_aborted;
}

void ZigbeeInterfaceNxpReply::abort()
{
    m_timer->stop();
    m_aborted = true;
    emit finished();
}

ZigbeeInterfaceNxpReply::ZigbeeInterfaceNxpReply(Nxp::Command command, QObject *parent) :
    QObject(parent),
    m_timer(new QTimer(this)),
    m_command(command)
{
    m_timer->setInterval(5000);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &ZigbeeInterfaceNxpReply::onTimeout);
}

void ZigbeeInterfaceNxpReply::setFinished()
{
    m_timer->stop();
    emit finished();
}

void ZigbeeInterfaceNxpReply::onTimeout()
{
    m_timeout = true;
    m_status = Nxp::StatusTimeout;

    emit timeout();
    emit finished();
}

QDebug operator<<(QDebug debug, ZigbeeInterfaceNxpReply *reply)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "InterfaceReply(" << reply->requestName() << ", " << reply->sequenceNumber() << ")";
    return debug;
}
