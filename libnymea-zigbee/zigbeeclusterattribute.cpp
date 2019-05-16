#include "zigbeeutils.h"
#include "zigbeeclusterattribute.h"

ZigbeeClusterAttribute::ZigbeeClusterAttribute()
{

}

ZigbeeClusterAttribute::ZigbeeClusterAttribute(quint16 id, Zigbee::DataType dataType, QByteArray data):
    m_id(id),
    m_dataType(dataType),
    m_data(data)
{

}

ZigbeeClusterAttribute::ZigbeeClusterAttribute(const ZigbeeClusterAttribute &other)
{
    m_id = other.id();
    m_dataType = other.dataType();
    m_data = other.data();
}

quint16 ZigbeeClusterAttribute::id() const
{
    return m_id;
}

Zigbee::DataType ZigbeeClusterAttribute::dataType() const
{
    return m_dataType;
}

QByteArray ZigbeeClusterAttribute::data() const
{
    return m_data;
}

ZigbeeClusterAttribute &ZigbeeClusterAttribute::operator=(const ZigbeeClusterAttribute &other)
{
    m_id = other.id();
    m_dataType = other.dataType();
    m_data = other.data();
    return *this;
}

bool ZigbeeClusterAttribute::operator==(const ZigbeeClusterAttribute &other) const
{
    return m_id == other.id() &&
            m_dataType == other.dataType() &&
            m_data == other.data();
}

bool ZigbeeClusterAttribute::operator!=(const ZigbeeClusterAttribute &other) const
{
    return !operator==(other);
}

bool ZigbeeClusterAttribute::isValid() const
{
    return m_id != 0 ||
            m_dataType != Zigbee::NoData ||
            !m_data.isNull();
}

QDebug operator<<(QDebug debug, const ZigbeeClusterAttribute &attribute)
{
    debug.nospace().noquote() << "ZigbeeClusterAttribute("
                              << ZigbeeUtils::convertUint16ToHexString(attribute.id()) << ", "
                              << attribute.dataType()  << ", "
                              << ZigbeeUtils::convertByteArrayToHexString(attribute.data()) << ")";
    return debug.space();
}
