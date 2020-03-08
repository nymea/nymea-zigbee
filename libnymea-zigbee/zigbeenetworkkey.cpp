#include "zigbeenetworkkey.h"

#include <QDataStream>

ZigbeeNetworkKey::ZigbeeNetworkKey()
{

}

ZigbeeNetworkKey::ZigbeeNetworkKey(const QString &key)
{
    QString rawKey = QString(key).remove(':');
    if (rawKey.isEmpty())
        return;

    Q_ASSERT_X(rawKey.count() == 32, "ZigbeeNetworkKey", "invalid key length in ZigbeeNetworkKey(QString).");
    m_key = QByteArray::fromHex(rawKey.toLatin1());
}

ZigbeeNetworkKey::ZigbeeNetworkKey(const QByteArray &keyString) :
    m_key(keyString)
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
