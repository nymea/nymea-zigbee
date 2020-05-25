#ifndef ZIGBEEDATATYPE_H
#define ZIGBEEDATATYPE_H

#include "zigbee.h"

class ZigbeeDataType
{
public:
    ZigbeeDataType(Zigbee::DataType dataType, const QByteArray &data = QByteArray());

    Zigbee::DataType dataType() const;
    QString name() const;
    QString className() const;
    QByteArray data() const;

    static int typeLength(Zigbee::DataType dataType);

private:
    Zigbee::DataType m_dataType = Zigbee::NoData;
    QByteArray m_data;
    QString m_name = "Unknown";
    QString m_className = "Null";
    int m_typeLength = 0;
};

#endif // ZIGBEEDATATYPE_H
