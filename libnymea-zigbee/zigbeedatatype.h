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

#ifndef ZIGBEEDATATYPE_H
#define ZIGBEEDATATYPE_H

#include "zigbee.h"

class ZigbeeDataType
{
public:
    ZigbeeDataType();
    ZigbeeDataType(const ZigbeeDataType &other);
    ZigbeeDataType(Zigbee::DataType dataType, const QByteArray &data = QByteArray());

    // From uint
    ZigbeeDataType(quint8 value);
    ZigbeeDataType(quint16 value);
    ZigbeeDataType(quint32 value, Zigbee::DataType dataType = Zigbee::Uint32);
    ZigbeeDataType(quint64 value, Zigbee::DataType dataType = Zigbee::Uint64);

    // From int
    ZigbeeDataType(qint8 value);
    ZigbeeDataType(qint16 value);
    ZigbeeDataType(qint32 value, Zigbee::DataType dataType = Zigbee::Int32);
    ZigbeeDataType(qint64 value, Zigbee::DataType dataType = Zigbee::Int64);


    ZigbeeDataType(bool value);
    ZigbeeDataType(const QString &value, Zigbee::DataType dataType = Zigbee::CharString);

    // To uint
    quint8 toUInt8(bool *ok = nullptr) const;
    quint16 toUInt16(bool *ok = nullptr) const;
    quint32 toUInt32(bool *ok = nullptr) const;
    quint64 toUInt64(bool *ok = nullptr) const;

    // Int
    qint8 toInt8(bool *ok = nullptr) const;
    qint16 toInt16(bool *ok = nullptr) const;
    qint32 toInt32(bool *ok = nullptr) const;
    qint64 toInt64(bool *ok = nullptr) const;

    bool toBool(bool *ok = nullptr) const;
    QString toString(bool *ok = nullptr) const;

    float toFloat(bool *ok = nullptr) const;
    double toDouble(bool *ok = nullptr) const;

    Zigbee::DataType dataType() const;
    QString name() const;
    QString className() const;
    QByteArray data() const;
    int dataLength() const;

    bool isValid() const;

    static int typeLength(Zigbee::DataType dataType);

    ZigbeeDataType &operator=(const ZigbeeDataType &other);
    bool operator==(const ZigbeeDataType &other) const;
    bool operator!=(const ZigbeeDataType &other) const;

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
