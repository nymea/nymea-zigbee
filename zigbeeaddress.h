#ifndef ZIGBEEADDRESS_H
#define ZIGBEEADDRESS_H

#include <QByteArray>
#include <QString>

class ZigbeeAddress
{
public:
    ZigbeeAddress();
    explicit ZigbeeAddress(quint64 address);
    explicit ZigbeeAddress(const QString &address);
    ZigbeeAddress(const ZigbeeAddress &other);
    ~ZigbeeAddress();

    quint64 toUInt64() const;
    QString toString() const;

    bool isNull() const;
    void clear();

    ZigbeeAddress &operator=(const ZigbeeAddress &other);
    bool operator<(const ZigbeeAddress &other) const;
    bool operator==(const ZigbeeAddress &other) const;
    inline bool operator!=(const ZigbeeAddress &other) const;

private:
     quint64 m_address;

};

QDebug operator<<(QDebug debug, const ZigbeeAddress &address);

#endif // ZIGBEEADDRESS_H
