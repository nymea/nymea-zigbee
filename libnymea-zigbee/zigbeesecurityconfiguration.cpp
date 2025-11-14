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

#include "zigbeesecurityconfiguration.h"

ZigbeeSecurityConfiguration::ZigbeeSecurityConfiguration()
{

}

ZigbeeSecurityConfiguration::ZigbeeSecurityConfiguration(const ZigbeeSecurityConfiguration &other)
{
    m_zigbeeSecurityMode = other.zigbeeSecurityMode();
    m_networkKey = other.networkKey();
    m_globalTrustCenterLinkKey = other.globalTrustCenterLinkKey();
}

ZigbeeSecurityConfiguration::ZigbeeSecurityMode ZigbeeSecurityConfiguration::zigbeeSecurityMode() const
{
    return m_zigbeeSecurityMode;
}

void ZigbeeSecurityConfiguration::setZigbeeSecurityMode(ZigbeeSecurityConfiguration::ZigbeeSecurityMode zigbeeSecurityMode)
{
    m_zigbeeSecurityMode = zigbeeSecurityMode;
}

ZigbeeNetworkKey ZigbeeSecurityConfiguration::networkKey() const
{
    return m_networkKey;
}

void ZigbeeSecurityConfiguration::setNetworkKey(const ZigbeeNetworkKey &networkKey)
{
    m_networkKey = networkKey;
}

ZigbeeNetworkKey ZigbeeSecurityConfiguration::globalTrustCenterLinkKey() const
{
    return m_globalTrustCenterLinkKey;
}

void ZigbeeSecurityConfiguration::setGlobalTrustCenterlinkKey(const ZigbeeNetworkKey &globalTrustCenterLinkKey)
{
    m_globalTrustCenterLinkKey = globalTrustCenterLinkKey;
}

void ZigbeeSecurityConfiguration::clear()
{
    m_zigbeeSecurityMode = ZigbeeSecurityModeNone;
    m_networkKey = ZigbeeNetworkKey();
    m_globalTrustCenterLinkKey = ZigbeeNetworkKey(QString("5A6967426565416C6C69616E63653039"));
}

ZigbeeSecurityConfiguration &ZigbeeSecurityConfiguration::operator=(const ZigbeeSecurityConfiguration &other)
{
    m_networkKey = other.networkKey();
    m_zigbeeSecurityMode = other.zigbeeSecurityMode();
    m_globalTrustCenterLinkKey = other.globalTrustCenterLinkKey();
    return *this;
}

bool ZigbeeSecurityConfiguration::operator==(const ZigbeeSecurityConfiguration &other) const
{
    return m_networkKey == other.networkKey() && m_globalTrustCenterLinkKey == other.globalTrustCenterLinkKey() && m_zigbeeSecurityMode == other.zigbeeSecurityMode();
}

bool ZigbeeSecurityConfiguration::operator!=(const ZigbeeSecurityConfiguration &other) const
{
    return !operator==(other);
}
