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

#include "zigbeeinterfacereply.h"
#include "loggingcategory.h"

ZigbeeInterfaceReply::ZigbeeInterfaceReply(const ZigbeeInterfaceRequest &request, QObject *parent) :
    QObject(parent),
    m_request(request)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);

    connect(m_timer, &QTimer::timeout, this, &ZigbeeInterfaceReply::onTimeout);
}

ZigbeeInterfaceRequest ZigbeeInterfaceReply::request() const
{
    return m_request;
}

ZigbeeInterfaceMessage ZigbeeInterfaceReply::statusMessage() const
{
    return m_statusMessage;
}

ZigbeeInterfaceMessage ZigbeeInterfaceReply::additionalMessage() const
{
    return m_additionalMessage;
}

ZigbeeInterfaceReply::Status ZigbeeInterfaceReply::status() const
{
    return m_status;
}

quint8 ZigbeeInterfaceReply::statusCode() const
{
    return m_statusCode;
}

quint8 ZigbeeInterfaceReply::sequenceNumber() const
{
    return m_sequenceNumber;
}

QString ZigbeeInterfaceReply::statusErrorMessage() const
{
    return m_statusErrorMessage;
}

void ZigbeeInterfaceReply::setStatusMessage(const ZigbeeInterfaceMessage &statusMessage)
{
    m_statusMessage = statusMessage;

    // Parse status message
    m_statusCode = static_cast<quint8>(statusMessage.data().at(0));

    // Well known error
    if (m_statusCode < 6) {
        m_status = static_cast<Status>(m_statusCode);
    } else if (m_statusCode >= 128 && m_statusCode <= 244) {
        qCWarning(dcZigbeeInterface()) << "Got zigbee status code" << m_statusCode;
        m_status = ZigbeeErrorEvent;
    } else {
        qCWarning(dcZigbeeInterface()) << "Got unknown status code";
    }

    // Sequence number
    m_sequenceNumber = static_cast<quint8>(statusMessage.data().at(0));

    // TODO: compare with the request messageType: 2 bytes of request messageType

    // Got error message
    if (statusMessage.data().count() > 4) {
        m_statusErrorMessage = QString::fromUtf8(statusMessage.data().right(statusMessage.data().count() -4));
    }
}

void ZigbeeInterfaceReply::setAdditionalMessage(const ZigbeeInterfaceMessage &additionalMessage)
{
    m_additionalMessage = additionalMessage;
}

bool ZigbeeInterfaceReply::isComplete() const
{
    // No status received
    if (!m_statusMessage.isValid())
        return false;

    // Status received, check if additional message is expected
    if (!request().expectsAdditionalMessage())
        return true;

    return m_additionalMessage.isValid();
}

void ZigbeeInterfaceReply::startTimer(const int &timeout)
{
    m_timer->start(timeout);
}

void ZigbeeInterfaceReply::setFinished()
{
    m_timer->stop();
    emit finished();
}

void ZigbeeInterfaceReply::onTimeout()
{
    m_timeouted = true;
    m_status = Timeouted;
    emit timeout();
}
