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

#ifndef ZIGBEEDEVICEOBJECTREPLY_H
#define ZIGBEEDEVICEOBJECTREPLY_H

#include <QObject>
#include <QTimer>

#include "zigbeedeviceprofile.h"
#include "zigbeenetworkrequest.h"

class ZigbeeDeviceObjectReply : public QObject
{
    Q_OBJECT

    friend class ZigbeeDeviceObject;

public:
    enum Error {
        ErrorNoError, // All OK, no error occured, the message was transported successfully
        ErrorTimeout, // The request timeouted
        ErrorInterfaceError, // A transport interface error occured. Could not communicate with the hardware.
        ErrorZigbeeApsStatusError, // An APS transport error occured. See zigbeeApsStatus()
        ErrorZigbeeNwkStatusError, // A NWK layer error occured. See zigbeeNwkStatus()
        ErrorZigbeeMacStatusError, // A MAC layer error occured. See zigbeeMacStatus()
        ErrorZigbeeDeviceObjectStatusError, // A ZDP error occured. See zigbeeDeviceObjectStatus()
        ErrorNetworkOffline // The network is offline. Cannot send any requests
    };
    Q_ENUM(Error)

    Error error() const;
    Zigbee::ZigbeeApsStatus zigbeeApsStatus() const;
    Zigbee::ZigbeeNwkLayerStatus zigbeeNwkStatus() const;
    Zigbee::ZigbeeMacLayerStatus zigbeeMacStatus() const;
    ZigbeeDeviceProfile::Status zigbeeDeviceObjectStatus() const;

    ZigbeeNetworkRequest request() const;
    quint8 transactionSequenceNumber() const;
    ZigbeeDeviceProfile::ZdoCommand expectedResponse() const;

    QByteArray responseData() const;
    ZigbeeDeviceProfile::Adpu responseAdpu() const;

    bool isComplete() const;

private:
    explicit ZigbeeDeviceObjectReply(const ZigbeeNetworkRequest &request, QObject *parent = nullptr);

    Error m_error = ErrorNoError;

    QTimer m_timeoutTimer;

    // Request information
    ZigbeeNetworkRequest m_request;
    quint8 m_transactionSequenceNumber = 0;

    // Transport layer errors
    Zigbee::ZigbeeApsStatus m_zigbeeApsStatus = Zigbee::ZigbeeApsStatusSuccess;
    void setZigbeeApsStatus(Zigbee::ZigbeeApsStatus status);

    Zigbee::ZigbeeNwkLayerStatus m_zigbeeNwkStatus = Zigbee::ZigbeeNwkLayerStatusSuccess;
    void setZigbeeNwkLayerStatus(Zigbee::ZigbeeNwkLayerStatus status);

    Zigbee::ZigbeeMacLayerStatus m_zigbeeMacStatus = Zigbee::ZigbeeMacLayerStatusSuccess;
    void setZigbeeMacLayerStatus(Zigbee::ZigbeeMacLayerStatus status);

    ZigbeeDeviceProfile::Status m_zigbeeDeviceObjectStatus = ZigbeeDeviceProfile::StatusSuccess;
    void setZigbeeDeviceObjectStatus(ZigbeeDeviceProfile::Status status);

    // ZDP response data
    bool m_apsConfirmReceived = false;
    bool m_zdpIndicationReceived = false;
    ZigbeeDeviceProfile::ZdoCommand m_expectedResponse;
    QByteArray m_responseData;
    ZigbeeDeviceProfile::Adpu m_responseAdpu;

signals:
    void finished();

};

#endif // ZIGBEEDEVICEOBJECTREPLY_H
