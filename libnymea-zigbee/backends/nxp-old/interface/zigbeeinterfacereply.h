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

#ifndef ZIGBEEINTERFACEREPLY_H
#define ZIGBEEINTERFACEREPLY_H

#include <QObject>
#include <QTimer>

#include "zigbeeinterfacerequest.h"

class ZigbeeBridgeControllerNxp;

class ZigbeeInterfaceReply : public QObject
{
    Q_OBJECT
    friend class ZigbeeBridgeControllerNxp;

public:
    enum Status {
        Success = 0,
        InvalidParameter = 1,
        UnhandledCommand = 2,
        CommandFailed = 3,
        Busy = 4,
        StackAlreadyStarted = 5,
        ZigbeeErrorEvent = 6,
        Timeouted = 7
    };
    Q_ENUM(Status)

    explicit ZigbeeInterfaceReply(const ZigbeeInterfaceRequest &request, QObject *parent = nullptr);

    ZigbeeInterfaceRequest request() const;
    ZigbeeInterfaceMessage statusMessage() const;
    ZigbeeInterfaceMessage additionalMessage() const;

    bool timeouted() const;

    Status status() const;
    quint8 statusCode() const;
    quint8 sequenceNumber() const;
    QString statusErrorMessage() const;

private:
    QTimer *m_timer = nullptr;
    bool m_timeouted = false;

    ZigbeeInterfaceRequest m_request;
    ZigbeeInterfaceMessage m_statusMessage;
    ZigbeeInterfaceMessage m_additionalMessage;

    // Status content
    Status m_status;
    quint8 m_statusCode;
    quint8 m_sequenceNumber;
    QString m_statusErrorMessage;

    // Called by ZigbeeBridgeControllerNxp
    void setStatusMessage(const ZigbeeInterfaceMessage &statusMessage);
    void setAdditionalMessage(const ZigbeeInterfaceMessage &additionalMessage);

    bool isComplete() const;
    void startTimer(const int &timeout = 500);
    void setFinished();

signals:
    void finished();
    void timeout();

private slots:
    void onTimeout();

};

#endif // ZIGBEEINTERFACEREPLY_H
