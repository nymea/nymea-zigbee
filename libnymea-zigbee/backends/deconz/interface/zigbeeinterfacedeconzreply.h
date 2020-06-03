/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
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
    Deconz::Command command() const;
    quint8 sequenceNumber() const;
    QByteArray responseData() const;

    // Response content
    Deconz::StatusCode statusCode() const;

    bool timendOut() const;
    bool aborted() const;
    void abort();

private:
    explicit ZigbeeInterfaceDeconzReply(Deconz::Command command, quint8 sequenceNumber, QObject *parent = nullptr);
    ZigbeeNetworkRequest m_networkRequest;
    QTimer *m_timer = nullptr;
    bool m_timeout = false;
    bool m_aborted = false;

    // Request content
    Deconz::Command m_command;
    quint8 m_sequenceNumber = 0;

    // Response content
    Deconz::StatusCode m_statusCode = Deconz::StatusCodeError;
    QByteArray m_responseData;

private slots:
    void onTimeout();

signals:
    void timeout();
    void finished();

};

#endif // ZIGBEEINTERFACEDECONZREPLY_H
