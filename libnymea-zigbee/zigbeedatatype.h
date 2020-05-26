#ifndef ZIGBEEDATATYPE_H
#define ZIGBEEDATATYPE_H

#include "zigbee.h"


class ZigbeeDataType
{
public:
    ZigbeeDataType(Zigbee::DataType dataType, const QByteArray &data = QByteArray());

    ZigbeeDataType(quint8 value, Zigbee::DataType dataType = Zigbee::Uint8);
    ZigbeeDataType(quint16 value, Zigbee::DataType dataType = Zigbee::Uint16);
    ZigbeeDataType(quint32 value, Zigbee::DataType dataType = Zigbee::Uint32);
    ZigbeeDataType(quint64 value, Zigbee::DataType dataType = Zigbee::Uint64);
    ZigbeeDataType(bool value);

    ZigbeeDataType(const QString &value, Zigbee::DataType dataType = Zigbee::CharString);

    quint8 toUInt8(bool *ok = nullptr) const;
    quint16 toUInt16(bool *ok = nullptr) const;
    quint32 toUInt32(bool *ok = nullptr) const;
    quint64 toUInt64(bool *ok = nullptr) const;

    bool toBool(bool *ok = nullptr) const;

    QString toString(bool *ok = nullptr) const;

    Zigbee::DataType dataType() const;
    QString name() const;
    QString className() const;
    QByteArray data() const;
    int dataLength() const;

    static int typeLength(Zigbee::DataType dataType);

private:
    Zigbee::DataType m_dataType = Zigbee::NoData;
    QByteArray m_data;
    QString m_name = "Unknown";
    QString m_className = "Null";
    int m_typeLength = 0;

    void setDataType(Zigbee::DataType dataType);
};

QDebug operator<<(QDebug debug, const ZigbeeDataType &dataType);

#endif // ZIGBEEDATATYPE_H
