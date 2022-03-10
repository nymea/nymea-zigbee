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
