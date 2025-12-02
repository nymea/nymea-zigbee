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

#include "zigbeeaddress.h"

#include <QDebug>

ZigbeeAddress::ZigbeeAddress()
{

}

ZigbeeAddress::ZigbeeAddress(quint64 address)
{
    m_address = address;
}

ZigbeeAddress::ZigbeeAddress(const QString &address)
{
    QString addressString = address;

    if (addressString.length() == 23)
        addressString.remove(QLatin1Char(':'));

    if (addressString.length() == 16) {
        bool ok;
        m_address = addressString.toULongLong(&ok, 16);
        if (!ok)
            clear();
    } else {
        m_address = 0;
    }
}

ZigbeeAddress::ZigbeeAddress(const ZigbeeAddress &other)
{
    m_address = other.toUInt64();
}

quint64 ZigbeeAddress::toUInt64() const
{
    return m_address;
}

QString ZigbeeAddress::toString() const
{
    QString string(QStringLiteral("%1:%2:%3:%4:%5:%6:%7:%8"));

    for (int i = 7; i >= 0; --i) {
        const quint8 value = (m_address >> (i*8)) & 0xff;
        string = string.arg(value, 2, 16, QLatin1Char('0'));
    }

    return string.toUpper();
}

bool ZigbeeAddress::isNull() const
{
    return m_address == 0;
}

void ZigbeeAddress::clear()
{
    m_address = 0;
}

ZigbeeAddress &ZigbeeAddress::operator=(const ZigbeeAddress &other)
{
    m_address = other.toUInt64();
    return *this;
}

bool ZigbeeAddress::operator<(const ZigbeeAddress &other) const
{
    return m_address < other.toUInt64();
}

bool ZigbeeAddress::operator==(const ZigbeeAddress &other) const
{
    return m_address == other.toUInt64();
}

bool ZigbeeAddress::operator!=(const ZigbeeAddress &other) const
{
    return !operator==(other);
}

QDebug operator<<(QDebug debug, const ZigbeeAddress &address)
{
    debug << address.toString();
    return debug;
}
