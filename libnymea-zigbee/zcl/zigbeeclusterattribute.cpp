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

#include "zigbeeutils.h"
#include "zigbeeclusterattribute.h"

ZigbeeClusterAttribute::ZigbeeClusterAttribute()
{

}

ZigbeeClusterAttribute::ZigbeeClusterAttribute(quint16 id, const ZigbeeDataType &dataType):
    m_id(id),
    m_dataType(dataType)
{

}

ZigbeeClusterAttribute::ZigbeeClusterAttribute(const ZigbeeClusterAttribute &other)
{
    m_id = other.id();
    m_dataType = other.dataType();
}

quint16 ZigbeeClusterAttribute::id() const
{
    return m_id;
}

ZigbeeDataType ZigbeeClusterAttribute::dataType() const
{
    return m_dataType;
}

ZigbeeClusterAttribute &ZigbeeClusterAttribute::operator=(const ZigbeeClusterAttribute &other)
{
    m_id = other.id();
    m_dataType = other.dataType();
    return *this;
}

bool ZigbeeClusterAttribute::operator==(const ZigbeeClusterAttribute &other) const
{
    return m_id == other.id() && m_dataType == other.dataType();
}

bool ZigbeeClusterAttribute::operator!=(const ZigbeeClusterAttribute &other) const
{
    return !operator==(other);
}

bool ZigbeeClusterAttribute::isValid() const
{
    return m_id != 0xffff && m_dataType.isValid();
}

QDebug operator<<(QDebug debug, const ZigbeeClusterAttribute &attribute)
{
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << "ZigbeeClusterAttribute("
                              << ZigbeeUtils::convertUint16ToHexString(attribute.id()) << ", "
                              << attribute.dataType()
                              << ")";
    return debug;
}
