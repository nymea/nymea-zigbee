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

    bool timendOut() const;
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
