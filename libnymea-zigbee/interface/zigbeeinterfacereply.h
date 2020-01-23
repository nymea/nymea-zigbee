/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
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

#ifndef ZIGBEEINTERFACEREPLY_H
#define ZIGBEEINTERFACEREPLY_H

#include <QObject>
#include <QTimer>

#include "zigbeeinterfacerequest.h"

class ZigbeeBridgeController;

class ZigbeeInterfaceReply : public QObject
{
    Q_OBJECT
    friend class ZigbeeBridgeController;

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

    // Called by ZigbeeBridgeController
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
