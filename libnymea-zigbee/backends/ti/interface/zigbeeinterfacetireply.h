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

#ifndef ZIGBEEINTERFACETIREPLY_H
#define ZIGBEEINTERFACETIREPLY_H

#include <QObject>
#include <QTimer>

#include "ti.h"
#include "zigbeenetworkrequest.h"

class ZigbeeInterfaceTiReply: public QObject
{
    Q_OBJECT

    friend class ZigbeeBridgeControllerTi;

public:
    explicit ZigbeeInterfaceTiReply(QObject *parent = nullptr, int timeout = 5000);
    explicit ZigbeeInterfaceTiReply(Ti::SubSystem subSystem, quint8 command, QObject *parent = nullptr, const QByteArray &requestPayload = QByteArray(), int timeout = 5000);

    // Request content
    Ti::SubSystem subSystem() const;
    quint8 command() const;
    QByteArray requestPayload() const;

    QByteArray responsePayload() const;

    // Response content
    Ti::StatusCode statusCode() const;

    bool timedOut() const;
    bool aborted() const;
    void abort();

signals:
    void timeout();
    void finished();

private slots:
    void onTimeout();
    void finish(Ti::StatusCode statusCode = Ti::StatusCodeSuccess);

private:
    QTimer *m_timer = nullptr;
    bool m_timeout = false;
    bool m_aborted = false;

    void setSequenceNumber(quint8 sequenceNumber);

    // Request content
    Ti::SubSystem m_subSystem = Ti::SubSystemSys;
    quint8 m_command = 0;
    QByteArray m_requestPayload;

    // Response content
    Ti::StatusCode m_statusCode = Ti::StatusCodeError;
    QByteArray m_responsePayload;


};

#endif // ZIGBEEINTERFACETIREPLY_H
