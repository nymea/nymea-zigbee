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

#ifndef ZIGBEECLUSTERREPLY_H
#define ZIGBEECLUSTERREPLY_H

#include <QObject>

#include "zigbeenetworkrequest.h"
#include "zigbeeclusterlibrary.h"

class ZigbeeClusterReply : public QObject
{
    Q_OBJECT

    friend class ZigbeeCluster;

public:
    enum Error {
        ErrorNoError, // All OK, no error occured, the message was transported successfully
        ErrorTimeout, // The request timeouted
        ErrorZigbeeApsStatusError, // An APS transport error occured. See zigbeeApsStatus()
        ErrorInterfaceError, // A transport interface error occured. Could not communicate with the hardware.
        ErrorNetworkOffline // The network is offline. Cannot send any requests
    };
    Q_ENUM(Error)

    Error error() const;

    ZigbeeNetworkRequest request() const;
    ZigbeeClusterLibrary::Frame requestFrame() const;

    quint8 transactionSequenceNumber() const;

    QByteArray responseData() const;
    ZigbeeClusterLibrary::Frame responseFrame() const;

    bool isComplete() const;

private:
    explicit ZigbeeClusterReply(const ZigbeeNetworkRequest &request, ZigbeeClusterLibrary::Frame requestFrame, QObject *parent = nullptr);

    Error m_error = ErrorNoError;

    // Request
    quint8 m_transactionSequenceNumber = 0;
    ZigbeeNetworkRequest m_request;
    ZigbeeClusterLibrary::Frame m_requestFrame;

    // Response
    bool m_apsConfirmReceived = false;
    Zigbee::ZigbeeApsStatus m_zigbeeApsStatus = Zigbee::ZigbeeApsStatusSuccess;

    ZigbeeClusterLibrary::Command m_expectedResponse;

    bool m_zclIndicationReceived = false;
    QByteArray m_responseData;
    ZigbeeClusterLibrary::Frame m_responseFrame;

signals:
    void finished();

};

#endif // ZIGBEECLUSTERREPLY_H
