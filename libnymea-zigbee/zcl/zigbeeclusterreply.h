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

#ifndef ZIGBEECLUSTERREPLY_H
#define ZIGBEECLUSTERREPLY_H

#include <QObject>
#include <QTimer>

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
        ErrorZigbeeNwkStatusError, // A NWK layer error occured. See zigbeeNwkStatus()
        ErrorZigbeeMacStatusError, // A MAC layer error occured. See zigbeeNwkStatus()
        ErrorZigbeeClusterLibraryError, // A ZCL error occured. See zigbeeClusterLibraryStatus()
        ErrorInterfaceError, // A transport interface error occured. Could not communicate with the hardware.
        ErrorNetworkOffline // The network is offline. Cannot send any requests
    };
    Q_ENUM(Error)

    Error error() const;
    Zigbee::ZigbeeApsStatus zigbeeApsStatus() const;
    Zigbee::ZigbeeNwkLayerStatus zigbeeNwkStatus() const;
    Zigbee::ZigbeeMacLayerStatus zigbeeMacStatus() const;
    ZigbeeClusterLibrary::Status zigbeeClusterLibraryStatus() const;

    ZigbeeNetworkRequest request() const;
    ZigbeeClusterLibrary::Frame requestFrame() const;

    quint8 transactionSequenceNumber() const;

    QByteArray responseData() const;
    ZigbeeClusterLibrary::Frame responseFrame() const;

    bool isComplete() const;

private:
    explicit ZigbeeClusterReply(const ZigbeeNetworkRequest &request, ZigbeeClusterLibrary::Frame requestFrame, QObject *parent = nullptr);

    Error m_error = ErrorNoError;

    QTimer m_timeoutTimer;

    // Request
    quint8 m_transactionSequenceNumber = 0;
    ZigbeeNetworkRequest m_request;
    ZigbeeClusterLibrary::Frame m_requestFrame;

    // Response
    bool m_apsConfirmReceived = false;
    Zigbee::ZigbeeApsStatus m_zigbeeApsStatus = Zigbee::ZigbeeApsStatusSuccess;
    Zigbee::ZigbeeNwkLayerStatus m_zigbeeNwkStatus = Zigbee::ZigbeeNwkLayerStatusSuccess;
    Zigbee::ZigbeeMacLayerStatus m_zigbeeMacStatus = Zigbee::ZigbeeMacLayerStatusSuccess;
    ZigbeeClusterLibrary::Status m_zigbeeClusterLibraryStatus = ZigbeeClusterLibrary::StatusSuccess;

    ZigbeeClusterLibrary::Command m_expectedResponse;

    bool m_zclIndicationReceived = false;
    QByteArray m_responseData;
    ZigbeeClusterLibrary::Frame m_responseFrame;

signals:
    void finished();

};

#endif // ZIGBEECLUSTERREPLY_H
