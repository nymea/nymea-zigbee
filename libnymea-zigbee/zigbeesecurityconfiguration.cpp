#include "zigbeesecurityconfiguration.h"

ZigbeeSecurityConfiguration::ZigbeeSecurityConfiguration()
{

}

ZigbeeSecurityConfiguration::ZigbeeSecurityConfiguration(const ZigbeeSecurityConfiguration &other)
{
    m_networkKey = other.networkKey();
    m_globalTrustCenterLinkKey = other.networkKey();
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
    m_networkKey.clear();
}

bool ZigbeeSecurityConfiguration::operator==(const ZigbeeSecurityConfiguration &other) const
{
    return m_networkKey == other.networkKey() && m_globalTrustCenterLinkKey == other.globalTrustCenterLinkKey();
}

bool ZigbeeSecurityConfiguration::operator!=(const ZigbeeSecurityConfiguration &other) const
{
    return !operator==(other);
}
