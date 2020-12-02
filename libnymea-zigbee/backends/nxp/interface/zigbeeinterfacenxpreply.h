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
