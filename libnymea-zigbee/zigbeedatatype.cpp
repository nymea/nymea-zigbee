#include "zigbeedatatype.h"

ZigbeeDataType::ZigbeeDataType(Zigbee::DataType dataType, const QByteArray &data):
    m_dataType(dataType),
    m_data(data)
{
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

int ZigbeeDataType::dataLength() const
{
    return typeLength(m_dataType);
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
        length = -1;
        break;
    case Zigbee::LongCharString:
        // first 2 byte is length
        length = -1;
        break;
    case Zigbee::Array:
        // 2 + sum of lengths of content
        length = -1;
        break;
    case Zigbee::Structure:
        // 2 + sum of lengths of content
        length = -1;
        break;
    case Zigbee::Set:
        // 2 + sum of lengths of content
        length = -1;
        break;
    case Zigbee::Bag:
        // 2 + sum of lengths of content
        length = -1;
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
