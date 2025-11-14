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

#include "zigbeenetworkkey.h"

#include <QDataStream>

ZigbeeNetworkKey::ZigbeeNetworkKey()
{

}

ZigbeeNetworkKey::ZigbeeNetworkKey(const ZigbeeNetworkKey &other)
{
    m_key = other.toByteArray();
}

ZigbeeNetworkKey::ZigbeeNetworkKey(const QString &keyString)
{
    QString rawKey = QString(keyString).remove(':');
    if (rawKey.isEmpty())
        return;

    Q_ASSERT_X(rawKey.size() == 32, "ZigbeeNetworkKey", "invalid key length in ZigbeeNetworkKey(QString).");
    m_key = QByteArray::fromHex(rawKey.toLatin1());
}

ZigbeeNetworkKey::ZigbeeNetworkKey(const QByteArray &key) :
    m_key(key)
{
    Q_ASSERT_X(isValid(), "ZigbeeNetworkKey", "invalid key length in ZigbeeNetworkKey(QByteArray).");
}

bool ZigbeeNetworkKey::isValid() const
{
    return m_key.size() == 16;
}

bool ZigbeeNetworkKey::isNull() const
{
    return m_key.isNull() || m_key == QByteArray("0000000000000000");
}

QString ZigbeeNetworkKey::toString() const
{
    if (isNull())
        return QString("00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00");

    QString keyString;
    for (int i = 0; i < 16; i++) {
        keyString.append(QString("%1").arg(static_cast<quint8>(m_key.at(i)), 2, 16, QLatin1Char('0')));
        if (i < 15)
            keyString.append(":");
    }

    return keyString;
}

QByteArray ZigbeeNetworkKey::toByteArray() const
{
    return m_key;
}

ZigbeeNetworkKey ZigbeeNetworkKey::generateKey()
{
    QByteArray keyData;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QDataStream stream(&keyData, QDataStream::WriteOnly);
#else
    QDataStream stream(&keyData, QIODevice::WriteOnly);
#endif

    for (int i = 0; i < 16; i++) {
        stream << static_cast<quint8>(rand() % (256 - 1) + 1);
    }

    return ZigbeeNetworkKey(keyData);
}

ZigbeeNetworkKey &ZigbeeNetworkKey::operator=(const ZigbeeNetworkKey &other)
{
    m_key = other.toByteArray();
    return *this;
}

bool ZigbeeNetworkKey::operator==(const ZigbeeNetworkKey &other) const
{
    return m_key == other.toByteArray();
}

bool ZigbeeNetworkKey::operator!=(const ZigbeeNetworkKey &other) const
{
    return m_key != other.toByteArray();
}

QDebug operator<<(QDebug debug, const ZigbeeNetworkKey &key)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "ZigbeeNetworkKey(" << key.toString() << ")";
    return debug;
}
