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

#include "zigbeedeviceobjectreply.h"

#include <QTimer>

ZigbeeDeviceObjectReply::ZigbeeDeviceObjectReply(const ZigbeeNetworkRequest &request, QObject *parent) :
    QObject(parent),
    m_request(request)
{
    m_timeoutTimer.setInterval(10000);
    connect(&m_timeoutTimer, &QTimer::timeout, this, [this](){
        m_error = ErrorTimeout;
        emit finished();
    });
}

void ZigbeeDeviceObjectReply::setZigbeeApsStatus(Zigbee::ZigbeeApsStatus status)
{
    m_zigbeeApsStatus = status;
    if (status != Zigbee::ZigbeeApsStatusSuccess) {
        m_error = ErrorZigbeeApsStatusError;
    }
}

void ZigbeeDeviceObjectReply::setZigbeeNwkLayerStatus(Zigbee::ZigbeeNwkLayerStatus status)
{
    m_zigbeeNwkStatus = status;
    if (status != Zigbee::ZigbeeNwkLayerStatusSuccess) {
        m_error = ErrorZigbeeNwkStatusError;
    }
}

void ZigbeeDeviceObjectReply::setZigbeeMacLayerStatus(Zigbee::ZigbeeMacLayerStatus status)
{
    m_zigbeeMacStatus = status;
    if (status != Zigbee::ZigbeeMacLayerStatusSuccess) {
        m_error = ErrorZigbeeMacStatusError;
    }
}

void ZigbeeDeviceObjectReply::setZigbeeDeviceObjectStatus(ZigbeeDeviceProfile::Status status)
{
    m_zigbeeDeviceObjectStatus = status;
    if (status != ZigbeeDeviceProfile::StatusSuccess) {
        m_error = ErrorZigbeeDeviceObjectStatusError;
    }
}

ZigbeeDeviceObjectReply::Error ZigbeeDeviceObjectReply::error() const
{
    return m_error;
}

Zigbee::ZigbeeApsStatus ZigbeeDeviceObjectReply::zigbeeApsStatus() const
{
    return m_zigbeeApsStatus;
}

Zigbee::ZigbeeNwkLayerStatus ZigbeeDeviceObjectReply::zigbeeNwkStatus() const
{
    return m_zigbeeNwkStatus;
}

Zigbee::ZigbeeMacLayerStatus ZigbeeDeviceObjectReply::zigbeeMacStatus() const
{
    return m_zigbeeMacStatus;
}

ZigbeeDeviceProfile::Status ZigbeeDeviceObjectReply::zigbeeDeviceObjectStatus() const
{
    return m_zigbeeDeviceObjectStatus;
}

ZigbeeNetworkRequest ZigbeeDeviceObjectReply::request() const
{
    return m_request;
}

quint8 ZigbeeDeviceObjectReply::transactionSequenceNumber() const
{
    return m_transactionSequenceNumber;
}

ZigbeeDeviceProfile::ZdoCommand ZigbeeDeviceObjectReply::expectedResponse() const
{
    return m_expectedResponse;
}

QByteArray ZigbeeDeviceObjectReply::responseData() const
{
    return m_responseData;
}

ZigbeeDeviceProfile::Adpu ZigbeeDeviceObjectReply::responseAdpu() const
{
    return m_responseAdpu;
}

bool ZigbeeDeviceObjectReply::isComplete() const
{
    return m_apsConfirmReceived && m_zdpIndicationReceived;
}
