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

    Q_ASSERT_X(rawKey.count() == 32, "ZigbeeNetworkKey", "invalid key length in ZigbeeNetworkKey(QString).");
    m_key = QByteArray::fromHex(rawKey.toLatin1());
}

ZigbeeNetworkKey::ZigbeeNetworkKey(const QByteArray &key) :
    m_key(key)
{
    Q_ASSERT_X(isValid(), "ZigbeeNetworkKey", "invalid key length in ZigbeeNetworkKey(QByteArray).");
}

bool ZigbeeNetworkKey::isValid() const
{
    return m_key.count() == 16;
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
    QDataStream stream(&keyData, QIODevice::WriteOnly);
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
    debug.nospace() << "ZigbeeNetworkKey(" << key.toString() << ")";
    return debug.space();
}
