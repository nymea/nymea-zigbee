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

#ifndef ZIGBEENETWORKKEY_H
#define ZIGBEENETWORKKEY_H

#include <QDebug>
#include <QString>
#include <QByteArray>

class ZigbeeNetworkKey
{

public:
    ZigbeeNetworkKey();
    ZigbeeNetworkKey(const ZigbeeNetworkKey &other);
    ZigbeeNetworkKey(const QString &keyString);
    ZigbeeNetworkKey(const QByteArray &key);

    bool isValid() const;
    bool isNull() const;

    QString toString() const;
    QByteArray toByteArray() const;

    static ZigbeeNetworkKey generateKey();

    ZigbeeNetworkKey &operator=(const ZigbeeNetworkKey &other);
    bool operator==(const ZigbeeNetworkKey &other) const;
    bool operator!=(const ZigbeeNetworkKey &other) const;

private:
    QByteArray m_key;

};

QDebug operator<<(QDebug debug, const ZigbeeNetworkKey &key);


#endif // ZIGBEENETWORKKEY_H
