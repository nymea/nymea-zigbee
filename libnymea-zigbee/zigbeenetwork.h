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

#ifndef ZIGBEENETWORK_H
#define ZIGBEENETWORK_H

#include <QDir>
#include <QUuid>
#include <QObject>
#include <QSettings>

#include "zigbeenode.h"
#include "zigbeechannelmask.h"
#include "zigbeesecurityconfiguration.h"

class ZigbeeNetworkDatabase;
class ZigbeeBridgeController;

class ZigbeeNetwork : public QObject
{
    Q_OBJECT

public:
    enum State {
        StateUninitialized,
        StateOffline,
        StateStarting,
        StateUpdating,
        StateRunning,
        StateStopping
    };
    Q_ENUM(State)

    enum Error {
        ErrorNoError,
        ErrorHardwareUnavailable,
        ErrorHardwareModuleChanged,
        ErrorZigbeeError
    };
    Q_ENUM(Error)

    explicit ZigbeeNetwork(const QUuid &networkUuid, QObject *parent = nullptr);

    QUuid networkUuid() const;

    State state() const;

    Error error() const;

    QDir settingsDirectory() const;
    void setSettingsDirectory(const QDir &settingsDirectory);

    virtual ZigbeeBridgeController *bridgeController() const = 0;
    virtual Zigbee::ZigbeeBackendType backendType() const = 0;

    // Serial port configuration
    QString serialPortName() const;
    void setSerialPortName(const QString &serialPortName);

    qint32 serialBaudrate() const;
    void setSerialBaudrate(qint32 baudrate);

    ZigbeeAddress macAddress() const;
    void setMacAddress(const ZigbeeAddress &zigbeeAddress);

    // Network configurations
    QString firmwareVersion() const;

    quint16 panId();
    void setPanId(quint16 panId);

    quint64 extendedPanId() const;
    void setExtendedPanId(quint64 extendedPanId);

    quint32 channel() const;
    void setChannel(quint32 channel);

    ZigbeeChannelMask channelMask() const;
    void setChannelMask(const ZigbeeChannelMask &channelMask);

    ZigbeeSecurityConfiguration securityConfiguration() const;
    void setSecurityConfiguration(const ZigbeeSecurityConfiguration &securityConfiguration);

    bool permitJoiningEnabled() const;
    quint8 permitJoiningDuration() const;
    quint8 permitJoiningRemaining() const;
    virtual void setPermitJoining(quint8 duration, quint16 address = Zigbee::BroadcastAddressAllRouters) = 0;

    quint8 generateSequenceNumber();

    // Network nodes
    QList<ZigbeeNode *> nodes() const;

    ZigbeeNode *coordinatorNode() const;

    ZigbeeNode *getZigbeeNode(quint16 shortAddress) const;
    ZigbeeNode *getZigbeeNode(const ZigbeeAddress &address) const;

    bool hasNode(quint16 shortAddress) const;
    bool hasNode(const ZigbeeAddress &address) const;

    virtual ZigbeeNetworkReply *sendRequest(const ZigbeeNetworkRequest &request) = 0;

    void loadNetwork();

    void removeZigbeeNode(const ZigbeeAddress &address);

private:
    QUuid m_networkUuid;
    State m_state = StateUninitialized;

    // Serial port configuration
    QString m_serialPortName = "/dev/ttyUSB0";
    qint32 m_serialBaudrate = 115200;
    ZigbeeAddress m_macAddress;

    // Continuous ASP sequence number for network requests
    quint8 m_sequenceNumber = 0;

    // Network configurations
    quint16 m_panId = 0;
    quint64 m_extendedPanId = 0;
    quint32 m_channel = 0;
    ZigbeeChannelMask m_channelMask = ZigbeeChannelMask(ZigbeeChannelMask::ChannelConfigurationAllChannels);
    ZigbeeDeviceProfile::NodeType m_nodeType = ZigbeeDeviceProfile::NodeTypeCoordinator;

    // Network storage
    QDir m_settingsDirectory = QDir("/etc/nymea/");
    QList<ZigbeeNode *> m_nodes;
    QList<ZigbeeNode *> m_uninitializedNodes;

    void printNetwork();

private:
    void addNodeInternally(ZigbeeNode *node);
    void removeNodeInternally(ZigbeeNode *node);

protected:
    Error m_error = ErrorNoError;
    ZigbeeNode *m_coordinatorNode = nullptr;
    ZigbeeSecurityConfiguration m_securityConfiguration;
    ZigbeeNetworkDatabase *m_database = nullptr;
    bool m_networkLoaded = false;

    ZigbeeNode *createNode(quint16 shortAddress, const ZigbeeAddress &extendedAddress, QObject *parent);
    ZigbeeNode *createNode(quint16 shortAddress, const ZigbeeAddress &extendedAddress, quint8 macCapabilities, QObject *parent);

    // Permit join
    QTimer *m_permitJoinTimer = nullptr;
    bool m_permitJoiningEnabled = false;
    quint8 m_permitJoiningDuration = 120;
    quint8 m_permitJoiningRemaining = 0;

    QTimer *m_reachableRefreshTimer = nullptr;

    void setPermitJoiningEnabled(bool permitJoiningEnabled);
    void setPermitJoiningDuration(quint8 duration);
    void setPermitJoiningRemaining(quint8 remaining);

    void clearSettings();

    bool hasUninitializedNode(const ZigbeeAddress &address) const;

    void addNode(ZigbeeNode *node);
    void addUnitializedNode(ZigbeeNode *node);
    void removeNode(ZigbeeNode *node);
    void removeUninitializedNode(ZigbeeNode *node);

    void setNodeReachable(ZigbeeNode *node, bool reachable);

    // Set the coordinator infromation since they cannot be fetched
    void setNodeInformation(ZigbeeNode *node, const QString &manufacturerName, const QString &modelName, const QString &version);

    void setState(State state);
    void setError(Error error);

    bool networkConfigurationAvailable() const;

    void handleNodeIndication(ZigbeeNode *node, const Zigbee::ApsdeDataIndication indication);

    // ZDO
    void handleZigbeeDeviceProfileIndication(const Zigbee::ApsdeDataIndication &indication);

    // ZCL
    void handleZigbeeClusterLibraryIndication(const Zigbee::ApsdeDataIndication &indication);

    void onDeviceAnnounced(quint16 shortAddress, ZigbeeAddress ieeeAddress, quint8 macCapabilities);

    // Network reply methods
    ZigbeeNetworkReply *createNetworkReply(const ZigbeeNetworkRequest &request = ZigbeeNetworkRequest());
    void setReplyResponseError(ZigbeeNetworkReply *reply, Zigbee::ZigbeeApsStatus zigbeeApsStatus = Zigbee::ZigbeeApsStatusSuccess);
    void finishNetworkReply(ZigbeeNetworkReply *reply, ZigbeeNetworkReply::Error error = ZigbeeNetworkReply::ErrorNoError);
    void startWaitingReply(ZigbeeNetworkReply *reply);

signals:
    void settingsDirectoryChanged(const QDir &settingsDirectory);
    void serialPortNameChanged(const QString &serialPortName);
    void serialBaudrateChanged(qint32 serialBaudrate);
    void macAddressChanged(const ZigbeeAddress &macAddress);
    void firmwareVersionChanged(const QString &firmwareVersion);

    void panIdChanged(quint16 panId);
    void extendedPanIdChanged(quint64 extendedPanId);
    void channelChanged(uint channel);
    void channelMaskChanged(const ZigbeeChannelMask &channelMask);
    void securityConfigurationChanged(const ZigbeeSecurityConfiguration &securityConfiguration);

    // Will be emitted if node has joined and the initialization has been finished
    void nodeAdded(ZigbeeNode *node);
    void nodeRemoved(ZigbeeNode *node);

    // Will be emited when a node joined and starts initializing
    void nodeJoined(ZigbeeNode *node);

    void permitJoiningEnabledChanged(bool permitJoiningEnabled);
    void permitJoinDurationChanged(quint8 duration);
    void permitJoinRemainingChanged(quint8 remaining);

    void errorOccured(Error error);
    void stateChanged(State state);

private slots:
    void onNodeStateChanged(ZigbeeNode::State state);
    void onNodeClusterAttributeChanged(ZigbeeCluster *cluster, const ZigbeeClusterAttribute &attribute);
    void evaluateNodeReachableStates();

public slots:
    virtual void startNetwork() = 0;
    virtual void stopNetwork() = 0;
    virtual void reset() = 0;
    virtual void factoryResetNetwork() = 0;
    virtual void destroyNetwork() = 0;

};

QDebug operator<<(QDebug debug, ZigbeeNetwork *network);


#endif // ZIGBEENETWORK_H
