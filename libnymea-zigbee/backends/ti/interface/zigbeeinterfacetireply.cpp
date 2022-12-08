/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2022, nymea GmbH
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
