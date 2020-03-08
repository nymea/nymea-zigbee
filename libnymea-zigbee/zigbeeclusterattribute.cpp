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
