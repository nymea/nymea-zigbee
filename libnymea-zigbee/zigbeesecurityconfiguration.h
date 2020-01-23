/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
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

#ifndef ZIGBEESECURITYCONFIGURATION_H
#define ZIGBEESECURITYCONFIGURATION_H

#include <QString>

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

    QString networkKey() const;
    void setNetworkKey(const QString &networkKey);

    QString globalTrustCenterLinkKey() const;
    void setGlobalTrustCenterlinkKey(const QString & globalTrustCenterLinkKey);

    void clear();

    ZigbeeSecurityConfiguration &operator=(const ZigbeeSecurityConfiguration &other);
    bool operator==(const ZigbeeSecurityConfiguration &other) const;
    bool operator!=(const ZigbeeSecurityConfiguration &other) const;

private:
    ZigbeeSecurityMode m_zigbeeSecurityMode = ZigbeeSecurityModeNone;

    // This is the local network key
    QString m_networkKey;

    // Note: this is the zigbee master key from (ZigBeeAlliance09)
    QString m_globalTrustCenterLinkKey = "5A6967426565416C6C69616E63653039";

};

#endif // ZIGBEESECURITYCONFIGURATION_H
