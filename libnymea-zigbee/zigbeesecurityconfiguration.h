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

#ifndef ZIGBEESECURITYCONFIGURATION_H
#define ZIGBEESECURITYCONFIGURATION_H

#include <QString>

#include "zigbeenetworkkey.h"

class ZigbeeSecurityConfiguration
{
public:
    enum ZigbeeSecurityMode {
        ZigbeeSecurityModeNone,
        ZigbeeSecurityModeNetworkLayer,
        ZigbeeSecurityModeApplicationLayer
    };

    explicit ZigbeeSecurityConfiguration();
    ZigbeeSecurityConfiguration(const ZigbeeSecurityConfiguration &other);

    ZigbeeSecurityMode zigbeeSecurityMode() const;
    void setZigbeeSecurityMode(ZigbeeSecurityMode zigbeeSecurityMode);

    ZigbeeNetworkKey networkKey() const;
    void setNetworkKey(const ZigbeeNetworkKey &networkKey);

    ZigbeeNetworkKey globalTrustCenterLinkKey() const;
    void setGlobalTrustCenterlinkKey(const ZigbeeNetworkKey &globalTrustCenterLinkKey);

    void clear();

    ZigbeeSecurityConfiguration &operator=(const ZigbeeSecurityConfiguration &other);
    bool operator==(const ZigbeeSecurityConfiguration &other) const;
    bool operator!=(const ZigbeeSecurityConfiguration &other) const;

private:
    ZigbeeSecurityMode m_zigbeeSecurityMode = ZigbeeSecurityModeNone;

    // This is the local network key
    ZigbeeNetworkKey m_networkKey;

    // Note: this is the zigbee master key from (ZigBeeAlliance09)
    ZigbeeNetworkKey m_globalTrustCenterLinkKey = ZigbeeNetworkKey(QString("5A6967426565416C6C69616E63653039"));

};

#endif // ZIGBEESECURITYCONFIGURATION_H
