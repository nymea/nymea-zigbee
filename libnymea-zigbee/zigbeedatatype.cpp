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

#include "zigbeedatatype.h"
#include "zigbeeutils.h"

#include <QtGlobal>
#include <QDataStream>

ZigbeeDataType::ZigbeeDataType()
{
    setDataType(Zigbee::NoData);
}

ZigbeeDataType::ZigbeeDataType(const ZigbeeDataType &other)
{
    setDataType(other.dataType());
    m_data = other.data();
}

ZigbeeDataType::ZigbeeDataType(Zigbee::DataType dataType, const QByteArray &data):
    m_data(data)
{
    setDataType(dataType);

    // TODO: verify data length and consistency


    /*
    switch (dataType) {
    case Zigbee::NoData:
        break;
    case Zigbee::Data8:
        break;
    case Zigbee::Data16:
        break;
    case Zigbee::Data24:
        break;
    case Zigbee::Data32:
        break;
    case Zigbee::Data40:
        break;
    case Zigbee::Data48:
        break;
    case Zigbee::Data56:
        break;
    case Zigbee::Data64:
        break;
    case Zigbee::Bool:
        break;
    case Zigbee::BitMap8:
        break;
    case Zigbee::BitMap16:
        break;
    case Zigbee::BitMap24:
        break;
    case Zigbee::BitMap32:
        break;
    case Zigbee::BitMap40:
        break;
    case Zigbee::BitMap48:
        break;
    case Zigbee::BitMap56:
        break;
    case Zigbee::BitMap64:
        break;
    case Zigbee::Uint8:
        break;
    case Zigbee::Uint16:
        break;
    case Zigbee::Uint24:
        break;
    case Zigbee::Uint32:
        break;
    case Zigbee::Uint40:
        break;
    case Zigbee::Uint48:
        break;
    case Zigbee::Uint56:
        break;
    case Zigbee::Uint64:
        break;
    case Zigbee::Int8:
        break;
    case Zigbee::Int16:
        break;
    case Zigbee::Int24:
        break;
    case Zigbee::Int32:
        break;
    case Zigbee::Int40:
        break;
    case Zigbee::Int48:
        break;
    case Zigbee::Int56:
        break;
    case Zigbee::Int64:
        break;
    case Zigbee::Enum8:
        break;
    case Zigbee::Enum16:
        break;
    case Zigbee::FloatSemi:
        break;
    case Zigbee::FloatSingle:
        break;
    case Zigbee::FloatDouble:
        break;
    case Zigbee::OctetString:
        break;
    case Zigbee::CharString:
        break;
    case Zigbee::LongOctetString:
        break;
    case Zigbee::LongCharString:
        break;
    case Zigbee::Array:
        break;
    case Zigbee::Structure:
        break;
    case Zigbee::Set:
        break;
    case Zigbee::Bag:
        break;
    case Zigbee::TimeOfDay:
        break;
    case Zigbee::Date:
        break;
    case Zigbee::UtcTime:
        break;
    case Zigbee::Cluster:
        break;
    case Zigbee::Attribute:
        break;
    case Zigbee::BacnetId:
        break;
    case Zigbee::IeeeAddress:
        break;
    case Zigbee::BitKey128:
        break;
    case Zigbee::Unknown:
        break;
    }
    */

}

ZigbeeDataType::ZigbeeDataType(quint8 value)
{
    setDataType(Zigbee::Uint8);
    m_data.clear();
    QDataStream stream(&m_data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << value;
}

ZigbeeDataType::ZigbeeDataType(quint16 value)
{
    setDataType(Zigbee::Uint16);
    m_data.clear();
    QDataStream stream(&m_data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << value;
}

ZigbeeDataType::ZigbeeDataType(quint32 value, Zigbee::DataType dataType)
{
    Q_ASSERT_X(dataType == Zigbee::Uint24 || dataType == Zigbee::Uint32, "ZigbeeDataType", "invalid data type for quint32 constructor");
    setDataType(dataType);
    m_data.clear();
    QDataStream stream(&m_data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << value;

    if (m_dataType == Zigbee::Uint24) {
        m_data.chop(1);
    }
}

ZigbeeDataType::ZigbeeDataType(quint64 value, Zigbee::DataType dataType)
{
    Q_ASSERT_X(dataType == Zigbee::Uint40 || dataType == Zigbee::Uint48 || dataType == Zigbee::Uint56 || dataType == Zigbee::Uint64, "ZigbeeDataType", "invalid data type for quint64 constructor");
    setDataType(dataType);
    m_data.clear();
    QDataStream stream(&m_data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << value;

    if (m_dataType == Zigbee::Uint40) {
        m_data.chop(3);
    } else if (m_dataType == Zigbee::Uint48) {
        m_data.chop(2);
    } else if (m_dataType == Zigbee::Uint56) {
        m_data.chop(1);
    }
}

ZigbeeDataType::ZigbeeDataType(qint8 value)
{
    setDataType(Zigbee::Int8);
    m_data.clear();
    QDataStream stream(&m_data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << value;
}

ZigbeeDataType::ZigbeeDataType(qint16 value)
{
    setDataType(Zigbee::Int16);
    m_data.clear();
    QDataStream stream(&m_data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << value;
}

ZigbeeDataType::ZigbeeDataType(qint32 value, Zigbee::DataType dataType)
{
    Q_ASSERT_X(dataType == Zigbee::Int24 || dataType == Zigbee::Int32, "ZigbeeDataType", "invalid data type for qint32 constructor");
    setDataType(dataType);
    m_data.clear();
    QDataStream stream(&m_data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << value;

    if (m_dataType == Zigbee::Int24) {
        m_data.chop(1);
    }
}

ZigbeeDataType::ZigbeeDataType(qint64 value, Zigbee::DataType dataType)
{
    Q_ASSERT_X(dataType == Zigbee::Int40 || dataType == Zigbee::Int48 || dataType == Zigbee::Int56 || dataType == Zigbee::Int64, "ZigbeeDataType", "invalid data type for qint64 constructor");
    setDataType(dataType);
    m_data.clear();
    QDataStream stream(&m_data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << value;

    if (m_dataType == Zigbee::Int40) {
        m_data.chop(3);
    } else if (m_dataType == Zigbee::Int48) {
        m_data.chop(2);
    } else if (m_dataType == Zigbee::Int56) {
        m_data.chop(1);
    }
}

ZigbeeDataType::ZigbeeDataType(bool value)
{
    setDataType(Zigbee::Bool);
    m_data.clear();
    QDataStream stream(&m_data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << (value ? static_cast<quint8>(1) : static_cast<quint8>(0));
}

ZigbeeDataType::ZigbeeDataType(const QString &value, Zigbee::DataType dataType)
{
    Q_ASSERT_X(dataType == Zigbee::OctetString || dataType == Zigbee::CharString || dataType == Zigbee::LongOctetString || dataType == Zigbee::LongCharString, "ZigbeeDataType", "invalid data type for QString constructor");
    setDataType(dataType);
    m_data.clear();
    QDataStream stream(&m_data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    if (dataType == Zigbee::OctetString || dataType == Zigbee::CharString) {
        Q_ASSERT_X(value.length() <= 255, "ZigbeeDataType", "the given string is too long for this datatype. Maximum size is 255");
        stream << static_cast<quint8>(value.length());
        for (int i = 0; i < value.length(); i++) {
            stream << static_cast<quint8>(value.at(i).toLatin1());
        }
    } else if (dataType == Zigbee::LongOctetString || dataType == Zigbee::LongCharString) {
        Q_ASSERT_X(value.length() <= 0xffff, "ZigbeeDataType", "the given string is too long for this datatype. Maximum size is 0xffff");
        stream << static_cast<quint16>(value.length());
        for (int i = 0; i < value.length(); i++) {
            stream << static_cast<quint16>(value.at(i).toLatin1());
        }
    }
}

quint8 ZigbeeDataType::toUInt8(bool *ok) const
{
    if (ok) *ok = true;
    if (m_data.count() != 1) {
        if (ok) *ok = false;
        return 0;
    }

    return static_cast<quint8>(m_data.at(0));
}

quint16 ZigbeeDataType::toUInt16(bool *ok) const
{
    if (ok) *ok = true;

    quint16 value = 0;
    if (m_data.count() != 2) {
        if (ok) *ok = false;
        return value;
    }

    QDataStream stream(m_data);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> value;
    return value;
}

quint32 ZigbeeDataType::toUInt32(bool *ok) const
{
    if (ok) *ok = true;
    quint32 value = 0;

    // Verify the data type
    if (m_dataType != Zigbee::Uint24 && m_dataType != Zigbee::Uint32) {
        if (ok) *ok = false;
        return value;
    }

    // Make sure there is enought data
    if (m_data.count() != 3 && m_data.count() != 4) {
        if (ok) *ok = false;
        return value;
    }

    if (m_data.count() == 3) {
        // Make it 32 bit for converting
        QByteArray convertedData(m_data);
        convertedData.append(static_cast<char>(0));
        QDataStream stream(convertedData);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> value;
    } else {
        QDataStream stream(m_data);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> value;
    }

    return value;
}

quint64 ZigbeeDataType::toUInt64(bool *ok) const
{
    if (ok) *ok = true;
    quint64 value = 0;

    switch (m_dataType) {
    case Zigbee::Uint40: {
        if (m_data.count() != 5) {
            if (ok) *ok = false;
            break;
        }

        // Make it 64 bit for converting
        QByteArray convertedData(m_data);
        for (int i = 0; i < 3; i++)
            convertedData.append(static_cast<char>(0));

        QDataStream stream(convertedData);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> value;
        break;
    }
    case Zigbee::Uint48: {
        if (m_data.count() != 6) {
            if (ok) *ok = false;
            break;
        }

        // Make it 64 bit for converting
        QByteArray convertedData(m_data);
        for (int i = 0; i < 2; i++)
            convertedData.append(static_cast<char>(0));

        QDataStream stream(convertedData);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> value;
        break;
    }
    case Zigbee::Uint56: {
        if (m_data.count() != 7) {
            if (ok) *ok = false;
            break;
        }

        // Make it 64 bit for converting
        QByteArray convertedData(m_data);
        convertedData.append(static_cast<char>(0));
        QDataStream stream(convertedData);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> value;
        break;
    }
    case Zigbee::Uint64: {
        if (m_data.count() != 8) {
            if (ok) *ok = false;
            break;
        }

        QDataStream stream(m_data);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> value;
        break;
    }
    default:
        if (ok) *ok = false;
        break;
    }

    return value;
}

qint8 ZigbeeDataType::toInt8(bool *ok) const
{
    if (ok) *ok = true;
    if (m_data.count() != 1) {
        if (ok) *ok = false;
        return 0;
    }

    return static_cast<qint8>(m_data.at(0));
}

qint16 ZigbeeDataType::toInt16(bool *ok) const
{
    if (ok) *ok = true;

    qint16 value = 0;
    if (m_data.count() != 2 || m_dataType != Zigbee::Int16) {
        if (ok) *ok = false;
        return value;
    }

    QDataStream stream(m_data);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> value;
    return value;
}

qint32 ZigbeeDataType::toInt32(bool *ok) const
{
    if (ok) *ok = true;
    qint32 value = 0;

    // Verify the data type
    if (m_dataType != Zigbee::Int24 && m_dataType != Zigbee::Int32) {
        if (ok) *ok = false;
        return value;
    }

    // Make sure there is enought data
    if (m_data.count() != 3 && m_data.count() != 4) {
        if (ok) *ok = false;
        return value;
    }

    if (m_data.count() == 3) {
        // Make it 32 bit for converting
        QByteArray convertedData(m_data);
        convertedData.append(static_cast<char>(0));
        QDataStream stream(convertedData);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> value;
    } else {
        QDataStream stream(m_data);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> value;
    }

    return value;
}

qint64 ZigbeeDataType::toInt64(bool *ok) const
{
    if (ok) *ok = true;
    qint64 value = 0;

    switch (m_dataType) {
    case Zigbee::Int40: {
        if (m_data.count() != 5) {
            if (ok) *ok = false;
            break;
        }

        // Make it 64 bit for converting
        QByteArray convertedData(m_data);
        for (int i = 0; i < 3; i++)
            convertedData.append(static_cast<char>(0));

        QDataStream stream(convertedData);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> value;
        break;
    }
    case Zigbee::Int48: {
        if (m_data.count() != 6) {
            if (ok) *ok = false;
            break;
        }

        // Make it 64 bit for converting
        QByteArray convertedData(m_data);
        for (int i = 0; i < 2; i++)
            convertedData.append(static_cast<char>(0));

        QDataStream stream(convertedData);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> value;
        break;
    }
    case Zigbee::Int56: {
        if (m_data.count() != 7) {
            if (ok) *ok = false;
            break;
        }

        // Make it 64 bit for converting
        QByteArray convertedData(m_data);
        convertedData.append(static_cast<char>(0));
        QDataStream stream(convertedData);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> value;
        break;
    }
    case Zigbee::Int64: {
        if (m_data.count() != 8) {
            if (ok) *ok = false;
            break;
        }

        QDataStream stream(m_data);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> value;
        break;
    }
    default:
        if (ok) *ok = false;
        break;
    }

    return value;
}

bool ZigbeeDataType::toBool(bool *ok) const
{
    if (ok) *ok = true;
    bool value = false;

    if (m_data.count() != 1) {
        if (ok) *ok = false;
        return value;
    }

    if (m_data.at(0) != 0) {
        value = true;
    }

    return value;
}

QString ZigbeeDataType::toString(bool *ok) const
{
    if (ok) *ok = true;
    QString value;

    if (m_dataType == Zigbee::OctetString || m_dataType == Zigbee::CharString) {
        quint8 length = m_data.at(0);
        value = QString::fromUtf8(m_data.right(length));
    } else if (m_dataType == Zigbee::LongOctetString || m_dataType == Zigbee::LongCharString) {
        quint16 length = 0;
        QDataStream lengthStream(m_data.left(2));
        lengthStream.setByteOrder(QDataStream::LittleEndian);
        lengthStream >> length;
        value = QString::fromUtf8(m_data.right(length));
    } else {
        if (ok) *ok = false;
    }
    return value;
}

float ZigbeeDataType::toFloat(bool *ok) const
{
    if (ok) *ok = true;
    float value = 0;
    if (m_dataType == Zigbee::FloatSemi && m_data.length() == 2) {
        value = (m_data.at(0) & 0xFF)
                | ((m_data.at(1) & 0xFF00) << 8);
    } else if (m_dataType == Zigbee::FloatSingle && m_data.length() == 4) {
        value = (m_data.at(0) & 0xFF)
                | ((m_data.at(1) & 0xFF00) << 8)
                | ((m_data.at(2) & 0xFF0000) << 16)
                | ((m_data.at(3) & 0xFF000000) << 24);
    } else {
        if (ok) *ok = false;
    }
    return value;
}

double ZigbeeDataType::toDouble(bool *ok) const
{
    if (ok) *ok = true;
    double value = 0;
    if (m_dataType == Zigbee::FloatSemi && m_data.length() == 2) {
        value = (m_data.at(0) & 0xFF)
                | ((m_data.at(1) & 0xFF00) << 8);
    } else if (m_dataType == Zigbee::FloatSingle && m_data.length() == 4) {
        value = (m_data.at(0) & 0xFF)
                | ((m_data.at(1) & 0xFF00) << 8)
                | ((m_data.at(2) & 0xFF0000) << 16)
                | ((m_data.at(3) & 0xFF000000) << 24);
    } else if (m_dataType == Zigbee::FloatDouble && m_data.length() == 8) {
        value = (m_data.at(0) & 0xFF)
                | ((m_data.at(1) & 0xFF00) << 8)
                | ((m_data.at(2) & 0xFF0000) << 16)
                | ((m_data.at(3) & 0xFF000000) << 24)
                | ((m_data.at(4) & 0xFF00000000) << 32)
                | ((m_data.at(5) & 0xFF0000000000) << 40)
                | ((m_data.at(6) & 0xFF000000000000) << 48)
                | ((m_data.at(7) & 0xFF00000000000000) << 56);
    } else {
        if (ok) *ok = false;
    }
    return value;
}

Zigbee::DataType ZigbeeDataType::dataType() const
{
    return m_dataType;
}

QString ZigbeeDataType::name() const
{
    return m_name;
}

QString ZigbeeDataType::className() const
{
    return m_className;
}

QByteArray ZigbeeDataType::data() const
{
    return m_data;
}

int ZigbeeDataType::dataLength() const
{
    return typeLength(m_dataType);
}

bool ZigbeeDataType::isValid() const
{
    // FIXME: implement validate data depending on the type
    return m_dataType != Zigbee::NoData && !m_data.isNull();
}

int ZigbeeDataType::typeLength(Zigbee::DataType dataType)
{
    int length = 0;
    switch (dataType) {
    case Zigbee::NoData:
        break;
    case Zigbee::Data8:
        length = 1;
        break;
    case Zigbee::Data16:
        length = 2;
        break;
    case Zigbee::Data24:
        length = 3;
        break;
    case Zigbee::Data32:
        length = 4;
        break;
    case Zigbee::Data40:
        length = 5;
        break;
    case Zigbee::Data48:
        length = 6;
        break;
    case Zigbee::Data56:
        length = 7;
        break;
    case Zigbee::Data64:
        length = 8;
        break;
    case Zigbee::Bool:
        length = 1;
        break;
    case Zigbee::BitMap8:
        length = 1;
        break;
    case Zigbee::BitMap16:
        length = 2;
        break;
    case Zigbee::BitMap24:
        length = 3;
        break;
    case Zigbee::BitMap32:
        length = 4;
        break;
    case Zigbee::BitMap40:
        length = 5;
        break;
    case Zigbee::BitMap48:
        length = 6;
        break;
    case Zigbee::BitMap56:
        length = 7;
        break;
    case Zigbee::BitMap64:
        length = 8;
        break;
    case Zigbee::Uint8:
        length = 1;
        break;
    case Zigbee::Uint16:
        length = 2;
        break;
    case Zigbee::Uint24:
        length = 3;
        break;
    case Zigbee::Uint32:
        length = 4;
        break;
    case Zigbee::Uint40:
        length = 5;
        break;
    case Zigbee::Uint48:
        length = 6;
        break;
    case Zigbee::Uint56:
        length = 7;
        break;
    case Zigbee::Uint64:
        length = 8;
        break;
    case Zigbee::Int8:
        length = 1;
        break;
    case Zigbee::Int16:
        length = 2;
        break;
    case Zigbee::Int24:
        length = 3;
        break;
    case Zigbee::Int32:
        length = 4;
        break;
    case Zigbee::Int40:
        length = 5;
        break;
    case Zigbee::Int48:
        length = 6;
        break;
    case Zigbee::Int56:
        length = 7;
        break;
    case Zigbee::Int64:
        length = 8;
        break;
    case Zigbee::Enum8:
        length = 1;
        break;
    case Zigbee::Enum16:
        length = 2;
        break;
    case Zigbee::FloatSemi:
        length = 2;
        break;
    case Zigbee::FloatSingle:
        length = 4;
        break;
    case Zigbee::FloatDouble:
        length = 8;
        break;
    case Zigbee::OctetString:
        // first byte is length
        length = -1;
        break;
    case Zigbee::CharString:
        // first byte is length
        length = -1;
        break;
    case Zigbee::LongOctetString:
        // first 2 byte is length
        length = -2;
        break;
    case Zigbee::LongCharString:
        // first 2 byte is length
        length = -2;
        break;
    case Zigbee::Array:
        // 2 + sum of lengths of content
        length = -3;
        break;
    case Zigbee::Structure:
        // 2 + sum of lengths of content
        length = -3;
        break;
    case Zigbee::Set:
        // sum of lengths of content
        length = -4;
        break;
    case Zigbee::Bag:
        // sum of lengths of content
        length = -4;
        break;
    case Zigbee::TimeOfDay:
        length = 4;
        break;
    case Zigbee::Date:
        length = 4;
        break;
    case Zigbee::UtcTime:
        length = 4;
        break;
    case Zigbee::Cluster:
        length = 2;
        break;
    case Zigbee::Attribute:
        length = 2;
        break;
    case Zigbee::BacnetId:
        length = 4;
        break;
    case Zigbee::IeeeAddress:
        length = 8;
        break;
    case Zigbee::BitKey128:
        length = 16;
        break;
    case Zigbee::Unknown:
        break;
    }

    return length;
}

ZigbeeDataType &ZigbeeDataType::operator=(const ZigbeeDataType &other)
{
    setDataType(other.dataType());
    m_data = other.data();
    return *this;
}

bool ZigbeeDataType::operator==(const ZigbeeDataType &other) const
{
    return m_dataType == other.dataType() && m_data == other.data();
}

bool ZigbeeDataType::operator!=(const ZigbeeDataType &other) const
{
    return !operator==(other);
}

void ZigbeeDataType::setDataType(Zigbee::DataType dataType)
{
    m_dataType = dataType;

    switch (dataType) {
    case Zigbee::NoData:
        m_name = "No data";
        m_className = "Null";
        m_typeLength = typeLength(m_dataType);
        m_data.clear();
        break;
    case Zigbee::Data8:
        m_name = "8-bit data";
        m_className = "General data discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Data16:
        m_name = "16-bit data";
        m_className = "General data discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Data24:
        m_name = "24-bit data";
        m_className = "General data discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Data32:
        m_name = "32-bit data";
        m_className = "General data discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Data40:
        m_name = "40-bit data";
        m_className = "General data discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Data48:
        m_name = "48-bit data";
        m_className = "General data discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Data56:
        m_name = "56-bit data";
        m_className = "General data discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Data64:
        m_name = "64-bit data";
        m_className = "General data discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Bool:
        m_name = "Bool";
        m_className = "Logical discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::BitMap8:
        m_name = "8-bit bitmap";
        m_className = "Bitmap discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::BitMap16:
        m_name = "16-bit bitmap";
        m_className = "Bitmap discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::BitMap24:
        m_name = "24-bit bitmap";
        m_className = "Bitmap discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::BitMap32:
        m_name = "32-bit bitmap";
        m_className = "Bitmap discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::BitMap40:
        m_name = "40-bit bitmap";
        m_className = "Bitmap discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::BitMap48:
        m_name = "48-bit bitmap";
        m_className = "Bitmap discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::BitMap56:
        m_name = "56-bit bitmap";
        m_className = "Bitmap discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::BitMap64:
        m_name = "64-bit bitmap";
        m_className = "Bitmap discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Uint8:
        m_name = "Unsigned 8-bit integer";
        m_className = "Unsigned integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Uint16:
        m_name = "Unsigned 16-bit integer";
        m_className = "Unsigned integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Uint24:
        m_name = "Unsigned 24-bit integer";
        m_className = "Unsigned integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Uint32:
        m_name = "Unsigned 32-bit integer";
        m_className = "Unsigned integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Uint40:
        m_name = "Unsigned 40-bit integer";
        m_className = "Unsigned integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Uint48:
        m_name = "Unsigned 48-bit integer";
        m_className = "Unsigned integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Uint56:
        m_name = "Unsigned 56-bit integer";
        m_className = "Unsigned integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Uint64:
        m_name = "Unsigned 64-bit integer";
        m_className = "Unsigned integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Int8:
        m_name = "Signed 8-bit integer";
        m_className = "Signed integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Int16:
        m_name = "Signed 16-bit integer";
        m_className = "Signed integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Int24:
        m_name = "Signed 24-bit integer";
        m_className = "Signed integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Int32:
        m_name = "Signed 32-bit integer";
        m_className = "Signed integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Int40:
        m_name = "Signed 40-bit integer";
        m_className = "Signed integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Int48:
        m_name = "Signed 48-bit integer";
        m_className = "Signed integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Int56:
        m_name = "Signed 56-bit integer";
        m_className = "Signed integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Int64:
        m_name = "Signed 64-bit integer";
        m_className = "Signed integer analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Enum8:
        m_name = "8-bit enumeration";
        m_className = "Enumeration discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Enum16:
        m_name = "16-bit enumeration";
        m_className = "Enumeration discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::FloatSemi:
        m_name = "Semi-precision";
        m_className = "Floating point analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::FloatSingle:
        m_name = "Single precision";
        m_className = "Floating point analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::FloatDouble:
        m_name = "Double precision";
        m_className = "Floating point analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::OctetString:
        m_name = "Octet string";
        m_className = "String discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::CharString:
        m_name = "Character string";
        m_className = "String discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::LongOctetString:
        m_name = "Long octet string";
        m_className = "String discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::LongCharString:
        m_name = "Long character string";
        m_className = "String discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Array:
        m_name = "Array";
        m_className = "Ordered sequence discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Structure:
        m_name = "Structure";
        m_className = "Ordered sequence discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Set:
        m_name = "Set";
        m_className = "Collection discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Bag:
        m_name = "Bag";
        m_className = "Collection discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::TimeOfDay:
        m_name = "Time of day";
        m_className = "Time analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Date:
        m_name = "Date";
        m_className = "Time analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::UtcTime:
        m_name = "UTC time";
        m_className = "Time analog";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Cluster:
        m_name = "Cluster ID";
        m_className = "Identifier discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Attribute:
        m_name = "Attribute ID";
        m_className = "Identifier discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::BacnetId:
        m_name = "BACnet OID";
        m_className = "Identifier discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::IeeeAddress:
        m_name = "IEEE address";
        m_className = "Miscellaneous discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::BitKey128:
        m_name = "128-bit security key";
        m_className = "Miscellaneous discrete";
        m_typeLength = typeLength(m_dataType);
        break;
    case Zigbee::Unknown:
        break;
    }
}

QDebug operator<<(QDebug debug, const ZigbeeDataType &dataType)
{
    // FIXME: print data depending on the datatype
    QDebugStateSaver saver(debug);
    debug.nospace() << "ZigbeeDataType(" << dataType.name();
    switch (dataType.dataType()) {
    case Zigbee::OctetString:
    case Zigbee::LongOctetString:
    case Zigbee::LongCharString:
    case Zigbee::CharString:
        debug.nospace() << ", " << dataType.toString();
        break;
    default:
        debug.nospace() << ", " << ZigbeeUtils::convertByteArrayToHexString(dataType.data());
    }

    debug.nospace() << ")";
    return debug;
}
