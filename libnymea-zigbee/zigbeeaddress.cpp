/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
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
