#include "zigbeesecurityconfiguration.h"

ZigbeeSecurityConfiguration::ZigbeeSecurityConfiguration()
{

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
