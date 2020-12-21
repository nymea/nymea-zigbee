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
