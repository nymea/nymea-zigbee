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

#ifndef ZIGBEENODE_H
#define ZIGBEENODE_H

#include <QObject>

#include "zigbee.h"
#include "zigbeeaddress.h"
#include "zigbeenodeendpoint.h"
#include "zdo/zigbeedeviceobject.h"

class ZigbeeNetwork;

class ZigbeeNode : public QObject
{
    Q_OBJECT

    friend class ZigbeeNetwork;

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
    bool connected() const;

    ZigbeeDeviceObject *deviceObject() const;

    quint16 shortAddress() const;
    ZigbeeAddress extendedAddress() const;

    QList<ZigbeeNodeEndpoint *> endpoints() const;
    bool hasEndpoint(quint8 endpointId) const;
    ZigbeeNodeEndpoint *getEndpoint(quint8 endpointId) const;

    // Information from node descriptor
    NodeType nodeType() const;
    FrequencyBand frequencyBand() const;
    Relationship relationship() const;
    quint16 manufacturerCode() const;

    bool complexDescriptorAvailable() const;
    bool userDescriptorAvailable() const;

    quint16 maximumRxSize() const;
    quint16 maximumTxSize() const;
    quint8 maximumBufferSize() const;

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

    virtual void leaveNetworkRequest(bool rejoin = false, bool removeChildren = false) = 0;

private:
    bool m_connected = false;
    State m_state = StateUninitialized;

    quint16 m_shortAddress = 0;
    ZigbeeAddress m_extendedAddress;

    // Server Mask
    quint16 m_serverMask = 0;
    bool m_isPrimaryTrustCenter = false;
    bool m_isBackupTrustCenter = false;
    bool m_isPrimaryBindingCache = false;
    bool m_isBackupBindingCache = false;
    bool m_isPrimaryDiscoveryCache = false;
    bool m_isBackupDiscoveryCache = false;
    bool m_isNetworkManager = false;

    // Power information
    quint16 m_powerDescriptorFlag = 0;
    PowerMode m_powerMode;
    PowerSource m_powerSource;
    QList<PowerSource> m_availablePowerSources;
    PowerLevel m_powerLevel;

    // Mac capabilities flag
    quint8 m_macCapabilitiesFlag = 0;
    bool m_alternatePanCoordinator = false;
    DeviceType m_deviceType = DeviceTypeFullFunction;
    bool m_powerSourceFlagMainPower = false;
    bool m_receiverOnWhenIdle = false;
    bool m_securityCapability = false;
    bool m_allocateAddress = false;

    // Descriptor capability
    quint8 m_descriptorFlag = 0;
    bool m_extendedActiveEndpointListAvailable = false;
    bool m_extendedSimpleDescriptorListAvailable = false;

    virtual void setClusterAttributeReport(const ZigbeeClusterAttributeReport &report) = 0;

protected:
    ZigbeeNode(ZigbeeNetwork *network, QObject *parent = nullptr);

    ZigbeeNetwork *m_network;
    ZigbeeDeviceObject *m_deviceObject = nullptr;
    QList<ZigbeeNodeEndpoint *> m_endpoints;

    // Node descriptor information
    QByteArray m_nodeDescriptorRawData;
    NodeType m_nodeType = NodeTypeRouter;
    FrequencyBand m_frequencyBand = FrequencyBand2400Mhz;
    Relationship m_relationship = Parent;
    quint16 m_manufacturerCode = 0;

    bool m_complexDescriptorAvailable = false;
    bool m_userDescriptorAvailable = false;

    quint16 m_maximumRxSize = 0;
    quint16 m_maximumTxSize = 0;
    quint8 m_maximumBufferSize = 0;

    void setState(State state);
    void setConnected(bool connected);

    void setShortAddress(const quint16 &shortAddress);
    void setExtendedAddress(const ZigbeeAddress &extendedAddress);

    quint16 serverMask() const;
    void setServerMask(quint16 serverMask);

    // MAC capability raw data flag for settings
    quint8 macCapabilitiesFlag() const;
    void setMacCapabilitiesFlag(quint8 macFlag);

    quint8 descriptorFlag() const;
    void setDescriptorFlag(quint8 descriptorFlag);

    // Power decriptor data
    quint16 powerDescriptorFlag() const;
    void setPowerDescriptorFlag(quint16 powerDescriptorFlag);

    // This method starts the node initialization phase (read descriptors and endpoints)
    virtual void startInitialization();
    virtual ZigbeeNodeEndpoint *createNodeEndpoint(quint8 endpointId, QObject *parent) = 0;

signals:
    void stateChanged(State state);
    void connectedChanged(bool connected);
    void clusterAdded(ZigbeeCluster *cluster);
    void clusterAttributeChanged(ZigbeeCluster *cluster, const ZigbeeClusterAttribute &attribute);

private slots:
    void onClusterAttributeChanged(const ZigbeeClusterAttribute &attribute);

};

QDebug operator<<(QDebug debug, ZigbeeNode *node);


#endif // ZIGBEENODE_H
