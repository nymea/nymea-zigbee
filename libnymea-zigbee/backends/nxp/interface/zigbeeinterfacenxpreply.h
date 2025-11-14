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

#ifndef ZIGBEEINTERFACENXPREPLY_H
#define ZIGBEEINTERFACENXPREPLY_H

#include <QObject>
#include <QTimer>

#include "nxp.h"
#include "zigbeenetworkrequest.h"

class ZigbeeInterfaceNxpReply : public QObject
{
    Q_OBJECT

    friend class ZigbeeBridgeControllerNxp;

public:
    // Request content
    ZigbeeNetworkRequest networkRequest() const;
    QString requestName();
    Nxp::Command command() const;
    quint8 sequenceNumber() const;
    QByteArray requestData() const;
    QByteArray responseData() const;

    // Response content
    Nxp::Status status() const;

    bool timendOut() const;
    bool aborted() const;
    void abort();

private:
    explicit ZigbeeInterfaceNxpReply(Nxp::Command command, QObject *parent = nullptr);

    ZigbeeNetworkRequest m_networkRequest;
    QTimer *m_timer = nullptr;
    bool m_timeout = false;
    bool m_aborted = false;

    // Request content
    QString m_requestName;
    Nxp::Command m_command;
    quint8 m_sequenceNumber = 0;
    QByteArray m_requestData;

    // Response content
    Nxp::Status m_status = Nxp::StatusUnknownCommand;
    QByteArray m_responseData;

    void setFinished();

private slots:
    void onTimeout();

signals:
    void timeout();
    void finished();

};

QDebug operator<<(QDebug debug, ZigbeeInterfaceNxpReply *reply);


#endif // ZIGBEEINTERFACENXPREPLY_H
