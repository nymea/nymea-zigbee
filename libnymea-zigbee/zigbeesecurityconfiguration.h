#ifndef ZIGBEESECURITYCONFIGURATION_H
#define ZIGBEESECURITYCONFIGURATION_H

#include <QString>

class ZigbeeSecurityConfiguration
{
public:
    explicit ZigbeeSecurityConfiguration();
    ZigbeeSecurityConfiguration(const ZigbeeSecurityConfiguration &other);

    QString networkKey() const;
    void setNetworkKey(const QString &networkKey);

    QString globalTrustCenterLinkKey() const;
    void setGlobalTrustCenterlinkKey(const QString & globalTrustCenterLinkKey);

    bool operator==(const ZigbeeSecurityConfiguration &other) const;
    bool operator!=(const ZigbeeSecurityConfiguration &other) const;

private:
    // This is the local network key
    QString m_networkKey;

    // Note: this is the zigbee master key from (ZigBeeAlliance09)
    QString m_globalTrustCenterLinkKey = "5A6967426565416C6C69616E63653039";

};

#endif // ZIGBEESECURITYCONFIGURATION_H
