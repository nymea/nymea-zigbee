#include "zigbeemanufacturer.h"

ZigbeeManufacturer::ZigbeeManufacturer()
{

}

ZigbeeManufacturer::ZigbeeManufacturer(quint16 id)
{
    m_id = id;
    // Set name
}

quint16 ZigbeeManufacturer::id() const
{
    return m_id;
}

QString ZigbeeManufacturer::name() const
{
    return m_name;
}
