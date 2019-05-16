#ifndef ZIGBEENODE_H
#define ZIGBEENODE_H

#include <QObject>

#include "zigbee.h"
#include "zigbeecluster.h"
#include "zigbeeaddress.h"

class ZigbeeNode : public QObject
{
    Q_OBJECT

    friend class ZigbeeNetwork;
    friend class ZigbeeNetworkManager;

public:
    enum State {
        StateUninitialized,
        StateInitializing,
        StateInitialized
    };
    Q_ENUM(State)

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

    enum DeviceType {
        DeviceTypeFullFunction,
        DeviceTypeReducedFunction
    };
    Q_ENUM(DeviceType)

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

    State state() const;

    quint16 shortAddress() const;
    ZigbeeAddress extendedAddress() const;
    quint8 endPoint() const;

    // Information from node descriptor
    NodeType nodeType() const;
    FrequencyBand frequencyBand() const;
    Relationship relationship() const;
    Zigbee::ZigbeeProfile profile() const;
    quint16 manufacturerCode() const;
    quint16 deviceId() const;

    bool complexDescriptorAvailable() const;
    bool userDescriptorAvailable() const;

    quint16 maximumRxSize() const;
    quint16 maximumTxSize() const;
    quint8 maximumBufferSize() const;

    QList<ZigbeeCluster *> inputClusters() const;
    ZigbeeCluster *getInputCluster(Zigbee::ClusterId clusterId) const;
    bool hasInputCluster(Zigbee::ClusterId clusterId) const;

    QList<ZigbeeCluster *> outputClusters() const;
    ZigbeeCluster *getOutputCluster(Zigbee::ClusterId clusterId) const;
    bool hasOutputCluster(Zigbee::ClusterId clusterId) const;

    // Server Mask
    bool isPrimaryTrustCenter() const;
    bool isBackupTrustCenter() const;
    bool isPrimaryBindingCache() const;
    bool isBackupBindingCache() const;
    bool isPrimaryDiscoveryCache() const;
    bool isBackupDiscoveryCache() const;
    bool isNetworkManager() const;

    // Descriptor capability
    bool extendedActiveEndpointListAvailable() const;
    bool extendedSimpleDescriptorListAvailable() const;

    // Mac capabilities flag
    bool alternatePanCoordinator() const;
    DeviceType deviceType() const;
    bool powerSourceFlagMainPower() const;
    bool receiverOnWhenIdle() const;
    bool securityCapability() const;
    bool allocateAddress() const;

    // Information from node power descriptor
    PowerMode powerMode() const;
    PowerSource powerSource() const;
    QList<PowerSource> availablePowerSources() const;
    PowerLevel powerLevel() const;

private:
    ZigbeeNode(QObject *parent = nullptr);
    State m_state = StateUninitialized;

    QHash<Zigbee::ClusterId, ZigbeeCluster *> m_inputClusters;
    QHash<Zigbee::ClusterId, ZigbeeCluster *> m_outputClusters;

    quint16 m_shortAddress = 0;
    ZigbeeAddress m_extendedAddress;
    quint8 m_endPoint = 1;

    NodeType m_nodeType = NodeTypeRouter;
    FrequencyBand m_frequencyBand = FrequencyBand2400Mhz;
    Relationship m_relationship = Parent;
    Zigbee::ZigbeeProfile m_profile;
    quint16 m_manufacturerCode = 0;
    quint16 m_deviceId = 0;

    bool m_complexDescriptorAvailable = false;
    bool m_userDescriptorAvailable = false;

    quint16 m_maximumRxSize = 0;
    quint16 m_maximumTxSize = 0;
    quint8 m_maximumBufferSize = 0;

    // Server Mask
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

    // Mac capabilities flag
    bool m_alternatePanCoordinator = false;
    DeviceType m_deviceType = DeviceTypeFullFunction;
    bool m_powerSourceFlagMainPower = false;
    bool m_receiverOnWhenIdle = false;
    bool m_securityCapability = false;
    bool m_allocateAddress = false;

    // Descriptor capability
    bool m_extendedActiveEndpointListAvailable = false;
    bool m_extendedSimpleDescriptorListAvailable = false;

protected:
    void setState(State state);

    void setShortAddress(const quint16 &shortAddress);
    void setExtendedAddress(const ZigbeeAddress &extendedAddress);
    void setEndPoint(quint8 endPoint);

    void setNodeType(NodeType nodeType);
    void setFrequencyBand(FrequencyBand frequencyBand);
    void setRelationship(Relationship relationship);
    void setZigbeeProfile(Zigbee::ZigbeeProfile profile);
    void setManufacturerCode(quint16 manufacturerCode);
    void setDeviceId(quint16 deviceType);

    void setMaximumRxSize(quint16 size);
    void setMaximumTxSize(quint16 size);
    void setMaximumBufferSize(quint8 size);

    void setServerMask(quint16 serverMask);
    void setComplexDescriptorAvailable(bool complexDescriptorAvailable);
    void setUserDescriptorAvailable(bool userDescriptorAvailable);
    void setMacCapabilitiesFlag(quint16 macFlag);
    void setDescriptorFlag(quint8 descriptorFlag);

    void setPowerMode(PowerMode powerMode);
    void setPowerSource(PowerSource powerSource);
    void setAvailablePowerSources(QList<PowerSource> availablePowerSources);
    void setPowerLevel(PowerLevel powerLevel);

    // Cluster commands
    void setClusterAttribute(Zigbee::ClusterId clusterId, const ZigbeeClusterAttribute &attribute = ZigbeeClusterAttribute());

signals:
    void stateChanged(State state);
    void clusterAdded(ZigbeeCluster *cluster);
    void clusterAttributeChanged(ZigbeeCluster *cluster, const ZigbeeClusterAttribute &attribute);

private slots:
    void onClusterAttributeChanged(const ZigbeeClusterAttribute &attribute);

};

QDebug operator<<(QDebug debug, ZigbeeNode *node);


#endif // ZIGBEENODE_H
