#ifndef ZIGBEENODE_H
#define ZIGBEENODE_H

#include <QObject>

#include "zigbee.h"
#include "zigbeeaddress.h"
#include "zigbeebridgecontroller.h"

class ZigbeeNetworkManager;

class ZigbeeNode : public QObject
{
    Q_OBJECT

    friend class ZigbeeNetworkManager;

public:
    enum NodeType {
        NodeTypeCoordinator = 0,
        NodeTypeRouter = 1,
        NodeTypeEndDevice = 2
    };
    Q_ENUM(NodeType)

    enum FrequencyBand {
        FrequencyBand868Mhz,
        FrequencyBand902Mhz,
        FrequencyBand2400Mhz
    };
    Q_ENUM(FrequencyBand)

    enum Relationship {
        Parent,
        Child,
        Sibling
    };
    Q_ENUM(Relationship)

    enum PowerMode {
        PowerModeAlwaysOn,
        PowerModeOnPeriodically,
        PowerModeOnWhenStimulated
    };
    Q_ENUM(PowerMode)

    enum PowerSource {
        PowerSourcePermanentMainSupply,
        PowerSourceRecharchableBattery,
        PowerSourceDisposableBattery
    };
    Q_ENUM(PowerSource)

    enum PowerLevel {
        PowerLevelCriticalLow,
        PowerLevelLow,
        PowerLevelOk,
        PowerLevelFull
    };
    Q_ENUM(PowerLevel)

    quint16 shortAddress() const;
    ZigbeeAddress extendedAddress() const;
    int endPoint() const;

    // Information from node descriptor
    NodeType nodeType() const;
    FrequencyBand frequencyBand() const;
    Relationship relationShip() const;
    Zigbee::ZigbeeProfile profile() const;
    quint16 manufacturerCode() const;

    PowerMode powerMode() const;
    PowerSource powerSource() const;
    QList<PowerSource> availablePowerSources() const;
    PowerLevel powerLevel() const;

    // Node specific zigbee commands
    void init();
    void identify();
    void toggle(int addressMode);

private:
    ZigbeeBridgeController *m_controller;

    quint16 m_shortAddress = 0;
    ZigbeeAddress m_extendedAddress;
    int m_endPoint = 0;

    NodeType m_nodeType = NodeTypeRouter;
    FrequencyBand m_frequencyBand = FrequencyBand2400Mhz;
    Relationship m_relationShip = Parent;

    Zigbee::ZigbeeProfile m_profile;
    quint16 m_manufacturerCode = 0;

    quint16 m_maximalRxSize = 0;
    quint16 m_maximalTxSize = 0;

    bool m_isPrimaryTrustCenter = false;
    bool m_isBackupTrustCenter = false;
    bool m_isPrimaryBindingCache = false;
    bool m_isBackupBindingCache = false;
    bool m_isPrimaryDiscoveryCache = false;
    bool m_isBackupDiscoveryCache = false;
    bool m_isNetworkManager = false;

    // Power information
    PowerMode m_powerMode;
    PowerSource m_powerSource;
    QList<PowerSource> m_availablePowerSources;
    PowerLevel m_powerLevel;

    // Mac capabilities
    bool m_receiverOnWhenIdle = false;
    bool m_securityCapability = false;

    void requestNodeDescription();
    void requestSimpleNodeDescription();
    void requestPowerDescriptor();
    void requestUserDescriptor();

    void saveToSettings();

protected:
    ZigbeeNode(ZigbeeBridgeController *controller, QObject *parent = nullptr);

    ZigbeeBridgeController *controller();

    void setShortAddress(const quint16 &shortAddress);
    void setExtendedAddress(const ZigbeeAddress &extendedAddress);

signals:

private slots:
    void onRequestNodeDescriptionFinished();
    void onRequestSimpleNodeDescriptionFinished();
    void onRequestPowerDescriptorFinished();
    void onRequestUserDescriptorFinished();
    void onToggleFinished();
    void onIdentifyFinished();

public slots:

};

QDebug operator<<(QDebug debug, ZigbeeNode *node);


#endif // ZIGBEENODE_H
