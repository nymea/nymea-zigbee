#ifndef ZIGBEENETWORKKEY_H
#define ZIGBEENETWORKKEY_H

#include <QDebug>
#include <QString>
#include <QByteArray>

class ZigbeeNetworkKey
{

public:
    ZigbeeNetworkKey();
    ZigbeeNetworkKey(const QString &key);
    ZigbeeNetworkKey(const QByteArray &keyString);

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
