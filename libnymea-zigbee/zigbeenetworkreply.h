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

#ifndef ZIGBEENETWORKREPLY_H
#define ZIGBEENETWORKREPLY_H

#include <QObject>
#include <QTimer>

#include "zigbee.h"
#include "zigbeenetworkrequest.h"

class ZigbeeNetworkReply : public QObject
{
    Q_OBJECT

    friend class ZigbeeNetwork;
    friend class ZigbeeNodeEndpoint;

public:
    enum Error {
        ErrorNoError,
        ErrorTimeout,
        ErrorInterfaceError,
        ErrorZigbeeMacStatusError,
        ErrorZigbeeNwkStatusError,
        ErrorZigbeeApsStatusError,
        ErrorNetworkOffline
    };
    Q_ENUM(Error)

    Error error() const;

    ZigbeeNetworkRequest request() const;
    Zigbee::ZigbeeMacLayerStatus zigbeeMacStatus() const;
    Zigbee::ZigbeeNwkLayerStatus zigbeeNwkStatus() const;
    Zigbee::ZigbeeApsStatus zigbeeApsStatus() const;

private:
    explicit ZigbeeNetworkReply(const ZigbeeNetworkRequest &request, QObject *parent = nullptr);
    ZigbeeNetworkRequest m_request;
    QTimer *m_timer = nullptr;

    Error m_error = ErrorNoError;
    Zigbee::ZigbeeMacLayerStatus m_zigbeeMacStatus = Zigbee::ZigbeeMacLayerStatusSuccess;
    Zigbee::ZigbeeApsStatus m_zigbeeApsStatus = Zigbee::ZigbeeApsStatusSuccess;
    Zigbee::ZigbeeNwkLayerStatus m_zigbeeNwkStatus = Zigbee::ZigbeeNwkLayerStatusSuccess;

signals:
    void finished();

};

#endif // ZIGBEENETWORKREPLY_H
