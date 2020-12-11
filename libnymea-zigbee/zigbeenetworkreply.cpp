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

#include "zigbeenetworkreply.h"

ZigbeeNetworkReply::Error ZigbeeNetworkReply::error() const
{
    return m_error;
}

ZigbeeNetworkRequest ZigbeeNetworkReply::request() const
{
    return m_request;
}

Zigbee::ZigbeeMacLayerStatus ZigbeeNetworkReply::zigbeeMacStatus() const
{
    return m_zigbeeMacStatus;
}

Zigbee::ZigbeeApsStatus ZigbeeNetworkReply::zigbeeApsStatus() const
{
    return m_zigbeeApsStatus;
}

bool ZigbeeNetworkReply::buffered() const
{
    return m_buffered;
}

Zigbee::ZigbeeNwkLayerStatus ZigbeeNetworkReply::zigbeeNwkStatus() const
{
    return m_zigbeeNwkStatus;
}

ZigbeeNetworkReply::ZigbeeNetworkReply(const ZigbeeNetworkRequest &request, QObject *parent) :
    QObject(parent),
    m_request(request)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(10000);
    connect(m_timer, &QTimer::timeout, this, [this](){
        if (m_buffered) {
            // We did not receive any reply from the buffered message, assuming the route could not be discovered to the device
            m_zigbeeNwkStatus = Zigbee::ZigbeeNwkLayerStatusRouteDiscoveryFailed;
            m_error = ErrorZigbeeNwkStatusError;
        } else {
            m_error = ErrorTimeout;
        }
        emit finished();
    });
}

