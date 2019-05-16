#ifndef ZIGBEECLUSTERATTRIBUTE_H
#define ZIGBEECLUSTERATTRIBUTE_H

#include <QDebug>

#include "zigbee.h"

class ZigbeeClusterAttribute
{
public:
    ZigbeeClusterAttribute();
    ZigbeeClusterAttribute(quint16 id, Zigbee::DataType dataType, QByteArray data);
    ZigbeeClusterAttribute(const ZigbeeClusterAttribute &other);

    quint16 id() const;
    Zigbee::DataType dataType() const;
    QByteArray data() const;

    ZigbeeClusterAttribute &operator=(const ZigbeeClusterAttribute &other);
    bool operator==(const ZigbeeClusterAttribute &other) const;
    bool operator!=(const ZigbeeClusterAttribute &other) const;

    bool isValid() const;

private:
    quint16 m_id = 0;
    Zigbee::DataType m_dataType = Zigbee::NoData;
    QByteArray m_data;
};

QDebug operator<<(QDebug debug, const ZigbeeClusterAttribute &attribute);


#endif // ZIGBEECLUSTERATTRIBUTE_H
