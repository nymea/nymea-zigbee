#include "zigbeesecurityconfiguration.h"

ZigbeeSecurityConfiguration::ZigbeeSecurityConfiguration()
{

}

ZigbeeSecurityConfiguration::ZigbeeSecurityConfiguration(const ZigbeeSecurityConfiguration &other)
{
    m_networkKey = other.networkKey();
    m_globalTrustCenterLinkKey = other.networkKey();
}

ZigbeeSecurityConfiguration::ZigbeeSecurityMode ZigbeeSecurityConfiguration::zigbeeSecurityMode() const
{
    return m_zigbeeSecurityMode;
}

void ZigbeeSecurityConfiguration::setZigbeeSecurityMode(ZigbeeSecurityConfiguration::ZigbeeSecurityMode zigbeeSecurityMode)
{
    m_zigbeeSecurityMode = zigbeeSecurityMode;
}

QString ZigbeeSecurityConfiguration::networkKey() const
{
    return m_networkKey;
}

void ZigbeeSecurityConfiguration::setNetworkKey(const QString &networkKey)
{
    m_networkKey = networkKey;
}

QString ZigbeeSecurityConfiguration::globalTrustCenterLinkKey() const
{
    return m_globalTrustCenterLinkKey;
}

void ZigbeeSecurityConfiguration::setGlobalTrustCenterlinkKey(const QString &globalTrustCenterLinkKey)
{
    m_globalTrustCenterLinkKey = globalTrustCenterLinkKey;
}

void ZigbeeSecurityConfiguration::clear()
{
    m_zigbeeSecurityMode = ZigbeeSecurityModeNone;
    m_networkKey.clear();
    m_globalTrustCenterLinkKey = "5A6967426565416C6C69616E63653039";
}

ZigbeeSecurityConfiguration &ZigbeeSecurityConfiguration::operator=(const ZigbeeSecurityConfiguration &other)
{
    m_networkKey = other.networkKey();
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
