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

#ifndef ZIGBEEREPLY_H
#define ZIGBEEREPLY_H

#include <QObject>

#include "zigbee.h"

class ZigbeeReply : public QObject
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;
    friend class ZigbeeNodeEndpoint;

public:
    enum Error {
        ErrorNoError,
        ErrorTimeout,
        ErrorInterfaceError,
        ErrorZigbeeError,
        ErrorNetworkOffline
    };
    Q_ENUM(Error)

    Error error() const;

protected:
    explicit ZigbeeReply(QObject *parent = nullptr);

    Error m_error = ErrorNoError;

    void finishReply(Error error = ErrorNoError);

signals:
    void finished();
};

#endif // ZIGBEEREPLY_H
