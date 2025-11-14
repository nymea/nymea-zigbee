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

#ifndef ZIGBEEADDRESS_H
#define ZIGBEEADDRESS_H

#include <QString>
#include <QByteArray>

class ZigbeeAddress
{
public:
    ZigbeeAddress();
    explicit ZigbeeAddress(quint64 address);
    explicit ZigbeeAddress(const QString &address);
    ZigbeeAddress(const ZigbeeAddress &other);

    quint64 toUInt64() const;
    QString toString() const;

    bool isNull() const;
    void clear();

    ZigbeeAddress &operator=(const ZigbeeAddress &other);
    bool operator<(const ZigbeeAddress &other) const;
    bool operator==(const ZigbeeAddress &other) const;
    bool operator!=(const ZigbeeAddress &other) const;

private:
     quint64 m_address = 0;

};

QDebug operator<<(QDebug debug, const ZigbeeAddress &address);

#endif // ZIGBEEADDRESS_H
