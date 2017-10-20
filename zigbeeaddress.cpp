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
    QString a = address;

    if (a.length() == 17)
        a.remove(QLatin1Char(':'));

    if (a.length() == 12) {
        bool ok;
        m_address = a.toULongLong(&ok, 16);
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

ZigbeeAddress::~ZigbeeAddress()
{

}

quint64 ZigbeeAddress::toUInt64() const
{
    return m_address;
}

QString ZigbeeAddress::toString() const
{
    QString s(QStringLiteral("%1:%2:%3:%4:%5:%6"));

    for (int i = 5; i >= 0; --i) {
        const quint8 a = (m_address >> (i*8)) & 0xff;
        s = s.arg(a, 2, 16, QLatin1Char('0'));
    }

    return s.toUpper();
}

bool ZigbeeAddress::isNull() const
{
    return m_address == 0;
}

void ZigbeeAddress::clear()
{
    m_address = 0;
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
