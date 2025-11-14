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

#ifndef ZIGBEEINTERFACEDECONZREPLY_H
#define ZIGBEEINTERFACEDECONZREPLY_H

#include <QObject>
#include <QTimer>

#include "deconz.h"
#include "zigbeenetworkrequest.h"

class ZigbeeInterfaceDeconzReply : public QObject
{
    Q_OBJECT

    friend class ZigbeeBridgeControllerDeconz;

public:
    // Request content
    ZigbeeNetworkRequest networkRequest() const;
    QString requestName();
    Deconz::Command command() const;
    quint8 sequenceNumber() const;
    QByteArray requestData() const;
    QByteArray responseData() const;

    // Response content
    Deconz::StatusCode statusCode() const;

    bool timendOut() const;
    bool aborted() const;
    void abort();

private:
    explicit ZigbeeInterfaceDeconzReply(Deconz::Command command, QObject *parent = nullptr);
    ZigbeeNetworkRequest m_networkRequest;
    QTimer *m_timer = nullptr;
    bool m_timeout = false;
    bool m_aborted = false;

    void setSequenceNumber(quint8 sequenceNumber);

    // Request content
    QString m_requestName;
    Deconz::Command m_command;
    quint8 m_sequenceNumber = 0;
    QByteArray m_requestData;

    // Response content
    Deconz::StatusCode m_statusCode = Deconz::StatusCodeError;
    QByteArray m_responseData;

private slots:
    void onTimeout();

signals:
    void timeout();
    void finished();

};

QDebug operator<<(QDebug debug, ZigbeeInterfaceDeconzReply *reply);

#endif // ZIGBEEINTERFACEDECONZREPLY_H
