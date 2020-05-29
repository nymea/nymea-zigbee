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

#include "zigbeeutils.h"
#include "zigbeenetwork.h"
#include "loggingcategory.h"

#include <QSqlQuery>

ZigbeeNetwork::ZigbeeNetwork(QObject *parent) :
    QObject(parent)
{

}

ZigbeeNetwork::State ZigbeeNetwork::state() const
{
    return m_state;
}

ZigbeeNetwork::Error ZigbeeNetwork::error() const
{
    return m_error;
}

QString ZigbeeNetwork::settingsFilenName() const
{
    return m_settingsFileName;
}

void ZigbeeNetwork::setSettingsFileName(const QString &settingsFileName)
{
    if (m_settingsFileName == settingsFileName)
        return;

    m_settingsFileName = settingsFileName;
    emit settingsFileNameChanged(m_settingsFileName);
}

QString ZigbeeNetwork::serialPortName() const
{
    return m_serialPortName;
}

void ZigbeeNetwork::setSerialPortName(const QString &serialPortName)
{
    if (m_serialPortName == serialPortName)
        return;

    m_serialPortName = serialPortName;
    emit serialPortNameChanged(m_serialPortName);
}

qint32 ZigbeeNetwork::serialBaudrate() const
{
    return m_serialBaudrate;
}

void ZigbeeNetwork::setSerialBaudrate(qint32 baudrate)
{
    if (m_serialBaudrate == baudrate)
        return;

    m_serialBaudrate = baudrate;
    emit serialBaudrateChanged(m_serialBaudrate);
}

quint16 ZigbeeNetwork::panId()
{
    return m_panId;
}

void ZigbeeNetwork::setPanId(quint16 panId)
{
    if (m_panId == panId)
        return;

    m_panId = panId;
    emit panIdChanged(m_panId);
}

quint64 ZigbeeNetwork::extendedPanId() const
{
    return m_extendedPanId;
}

void ZigbeeNetwork::setExtendedPanId(quint64 extendedPanId)
{
    if (m_extendedPanId == extendedPanId)
        return;

    m_extendedPanId = extendedPanId;
    emit extendedPanIdChanged(m_extendedPanId);
}

quint32 ZigbeeNetwork::channel() const
{
    return m_channel;
}

void ZigbeeNetwork::setChannel(quint32 channel)
{
    if (m_channel == channel)
        return;

    m_channel = channel;
    emit channelChanged(m_channel);
}

ZigbeeChannelMask ZigbeeNetwork::channelMask() const
{
    return m_channelMask;
}

void ZigbeeNetwork::setChannelMask(const ZigbeeChannelMask &channelMask)
{
    if (m_channelMask == channelMask)
        return;

    m_channelMask = channelMask;
    emit channelMaskChanged(m_channelMask);
}

ZigbeeSecurityConfiguration ZigbeeNetwork::securityConfiguration() const
{
    return m_securityConfiguration;
}

void ZigbeeNetwork::setSecurityConfiguration(const ZigbeeSecurityConfiguration &securityConfiguration)
{
    if (m_securityConfiguration == securityConfiguration)
        return;

    m_securityConfiguration = securityConfiguration;
    emit securityConfigurationChanged(m_securityConfiguration);
}

bool ZigbeeNetwork::permitJoining() const
{
    return m_permitJoining;
}

void ZigbeeNetwork::setPermitJoining(bool permitJoining)
{
    setPermitJoiningInternal(permitJoining);
}

quint8 ZigbeeNetwork::generateSequenceNumber()
{
    return m_sequenceNumber++;
}

quint8 ZigbeeNetwork::generateTranactionSequenceNumber()
{
    return m_transactionSequenceNumber++;
}

QList<ZigbeeNode *> ZigbeeNetwork::nodes() const
{
    return m_nodes;
}

ZigbeeNode *ZigbeeNetwork::coordinatorNode() const
{
    return m_coordinatorNode;
}

ZigbeeNode *ZigbeeNetwork::getZigbeeNode(quint16 shortAddress) const
{
    foreach (ZigbeeNode *node, m_uninitializedNodes) {
        if (node->shortAddress() == shortAddress) {
            return node;
        }
    }

    foreach (ZigbeeNode *node, m_nodes) {
        if (node->shortAddress() == shortAddress) {
            return node;
        }
    }

    return nullptr;
}

ZigbeeNode *ZigbeeNetwork::getZigbeeNode(const ZigbeeAddress &address) const
{
    foreach (ZigbeeNode *node, m_uninitializedNodes) {
        if (node->extendedAddress() == address) {
            return node;
        }
    }

    foreach (ZigbeeNode *node, m_nodes) {
        if (node->extendedAddress() == address) {
            return node;
        }
    }

    return nullptr;
}

bool ZigbeeNetwork::hasNode(quint16 shortAddress) const
{
    return getZigbeeNode(shortAddress) != nullptr;
}

bool ZigbeeNetwork::hasNode(const ZigbeeAddress &address) const
{
    return getZigbeeNode(address) != nullptr;
}

void ZigbeeNetwork::addNodeInternally(ZigbeeNode *node)
{
    if (m_nodes.contains(node)) {
        qCWarning(dcZigbeeNetwork()) << "The node" << node << "has already been added.";
        return;
    }

    // Set the coordinator node if the short address is 0x0000
    if (node->shortAddress() == 0) {
        m_coordinatorNode = node;
    }

    // FIXME: check when and how the note will be reachable
    //node->setConnected(state() == StateRunning);

    m_nodes.append(node);
    emit nodeAdded(node);
}

void ZigbeeNetwork::removeNodeInternally(ZigbeeNode *node)
{
    if (!m_nodes.contains(node)) {
        qCWarning(dcZigbeeNetwork()) << "Try to remove node" << node << "but not in the node list.";
        return;
    }

    m_nodes.removeAll(node);
    emit nodeRemoved(node);
    node->deleteLater();
}

ZigbeeNode *ZigbeeNetwork::createNode(quint16 shortAddress, const ZigbeeAddress &extendedAddress, QObject *parent)
{
    return new ZigbeeNode(this, shortAddress, extendedAddress, parent);
}

ZigbeeNode *ZigbeeNetwork::createNode(quint16 shortAddress, const ZigbeeAddress &extendedAddress, quint8 macCapabilities, QObject *parent)
{
    ZigbeeNode *node = createNode(shortAddress, extendedAddress, parent);
    node->setMacCapabilitiesFlag(macCapabilities);
    return node;
}

void ZigbeeNetwork::saveNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Save current network configuration to" << m_settingsFileName;
    QSettings settings(m_settingsFileName, QSettings::IniFormat, this);
    settings.beginGroup("Network");
    settings.setValue("panId", panId());
    settings.setValue("channel", channel());
    settings.setValue("networkKey", securityConfiguration().networkKey().toString());
    settings.setValue("trustCenterLinkKey", securityConfiguration().globalTrustCenterLinkKey().toString());
    settings.endGroup();

    foreach (ZigbeeNode *node, nodes()) {
        saveNode(node);
    }
}

void ZigbeeNetwork::loadNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Load current network configuration from" << m_settingsFileName;
    QSettings settings(m_settingsFileName, QSettings::IniFormat, this);

    settings.beginGroup("Network");
    quint16 panId = static_cast<quint16>(settings.value("panId", 0).toUInt());
    setPanId(panId);
    setChannel(settings.value("channel", 0).toUInt());
    ZigbeeNetworkKey netKey(settings.value("networkKey", QString()).toString());
    if (netKey.isValid())
        m_securityConfiguration.setNetworkKey(netKey);

    ZigbeeNetworkKey tcKey(settings.value("trustCenterLinkKey", QString("5A6967426565416C6C69616E63653039")).toString());
    if (!tcKey.isValid())
        m_securityConfiguration.setGlobalTrustCenterlinkKey(tcKey);

    settings.endGroup(); // Network

    // Load nodes
    settings.beginGroup("Nodes");
    foreach (const QString ieeeAddressString, settings.childGroups()) {
        settings.beginGroup(ieeeAddressString);
        quint16 shortAddress = static_cast<quint16>(settings.value("nwkAddress", 0).toUInt());
        ZigbeeNode *node = createNode(shortAddress, ZigbeeAddress(ieeeAddressString), this);

        // Node descriptor
        node->m_nodeType = static_cast<ZigbeeNode::NodeType>(settings.value("nodeType", 0).toUInt());
        node->m_complexDescriptorAvailable = settings.value("complexDescriptor", false).toBool();
        node->m_userDescriptorAvailable = settings.value("userDescriptor", false).toBool();
        node->m_frequencyBand = static_cast<ZigbeeNode::FrequencyBand>(settings.value("frequencyBand", 0).toUInt());
        node->setMacCapabilitiesFlag(static_cast<quint8>(settings.value("macCapabilitiesFlag", 0).toUInt()));
        node->m_manufacturerCode = static_cast<quint16>(settings.value("manufacturerCode", 0).toUInt());
        node->m_maximumBufferSize = static_cast<quint8>(settings.value("maximumBufferSize", 0).toUInt());
        node->m_maximumRxSize = static_cast<quint16>(settings.value("maximumRxSize", 0).toUInt());
        node->m_maximumTxSize = static_cast<quint16>(settings.value("maximumTxSize", 0).toUInt());
        node->setServerMask(static_cast<quint16>(settings.value("serverMask", 0).toUInt()));
        node->setDescriptorFlag(static_cast<quint8>(settings.value("descriptorCapabilities", 0).toUInt()));

        // Power descriptor
        node->setPowerDescriptorFlag(static_cast<quint16>(settings.value("powerDescriptorFlag", 0).toUInt()));

        int endpointsCount = settings.beginReadArray("endpoints");
        //qCDebug(dcZigbeeNetwork()) << "loading endpoints" << endpointsCount << settings.childKeys() << settings.childGroups();
        for (int i = 0; i < endpointsCount; i++) {
            settings.setArrayIndex(i);
            quint8 endpointId = static_cast<quint8>(settings.value("id", 0).toUInt());
            ZigbeeNodeEndpoint *endpoint = new ZigbeeNodeEndpoint(this, node, endpointId, node);
            endpoint->m_profile = static_cast<Zigbee::ZigbeeProfile>(settings.value("profile", 0).toUInt());
            endpoint->m_deviceId = static_cast<quint16>(settings.value("deviceId", 0).toUInt());
            endpoint->m_deviceVersion = static_cast<quint8>(settings.value("deviceId", 0).toUInt());
            endpoint->m_manufacturerName = settings.value("manufacturerName").toString();
            endpoint->m_modelIdentifier = settings.value("modelIdentifier").toString();
            endpoint->m_softwareBuildId = settings.value("softwareBuildId").toString();

            int inputClustersCount = settings.beginReadArray("inputClusters");
            for (int n = 0; n < inputClustersCount; n ++) {
                settings.setArrayIndex(n);
                Zigbee::ClusterId clusterId = static_cast<Zigbee::ClusterId>(settings.value("clusterId", 0).toUInt());
                ZigbeeCluster *cluster = endpoint->createCluster(clusterId, ZigbeeCluster::Server);
                endpoint->addInputCluster(cluster);
            }
            settings.endArray(); // inputClusters

            int outputClustersCount = settings.beginReadArray("outputClusters");
            for (int n = 0; n < outputClustersCount; n ++) {
                settings.setArrayIndex(n);
                Zigbee::ClusterId clusterId = static_cast<Zigbee::ClusterId>(settings.value("clusterId", 0).toUInt());
                ZigbeeCluster *cluster = endpoint->createCluster(clusterId, ZigbeeCluster::Client);
                endpoint->addOutputCluster(cluster);
            }
            settings.endArray(); // outputClusters

            node->m_endpoints.append(endpoint);
        }

        settings.endArray(); // endpoints

        settings.endGroup(); // ieeeAddress

        node->setState(ZigbeeNode::StateInitialized);
        addNodeInternally(node);
    }
    settings.endGroup(); // Nodes
}

void ZigbeeNetwork::clearSettings()
{
    qCDebug(dcZigbeeNetwork()) << "Remove zigbee nodes from network";
    foreach (ZigbeeNode *node, m_nodes) {
        removeNode(node);
    }

    foreach (ZigbeeNode *node, m_uninitializedNodes) {
        m_uninitializedNodes.removeAll(node);
        node->deleteLater();
    }

    qCDebug(dcZigbeeNetwork()) << "Clear network settings" << m_settingsFileName;
    QSettings settings(m_settingsFileName, QSettings::IniFormat, this);
    settings.clear();

    // Reset network configurations
    qCDebug(dcZigbeeNetwork()) << "Clear network properties";
    m_extendedPanId = 0;
    m_channel = 0;
    m_securityConfiguration.clear();
    m_nodeType = ZigbeeNode::NodeTypeCoordinator;
}

void ZigbeeNetwork::saveNode(ZigbeeNode *node)
{
    QSettings settings(m_settingsFileName, QSettings::IniFormat, this);
    settings.beginGroup("Nodes");

    // Save this node
    settings.beginGroup(node->extendedAddress().toString());
    settings.setValue("nwkAddress", node->shortAddress());

    // Node descriptor
    settings.setValue("nodeType", node->m_nodeType);
    settings.setValue("complexDescriptor", node->complexDescriptorAvailable());
    settings.setValue("userDescriptor", node->userDescriptorAvailable());
    settings.setValue("frequencyBand", node->frequencyBand());
    settings.setValue("macCapabilitiesFlag", node->m_macCapabilitiesFlag);
    settings.setValue("manufacturerCode", node->m_manufacturerCode);
    settings.setValue("maximumBufferSize", node->m_maximumBufferSize);
    settings.setValue("maximumRxSize", node->m_maximumRxSize);
    settings.setValue("maximumTxSize", node->m_maximumTxSize);
    settings.setValue("serverMask", node->m_serverMask);
    settings.setValue("descriptorCapabilities", node->m_descriptorFlag);

    // Power descriptor
    settings.setValue("powerDescriptorFlag", node->m_powerDescriptorFlag);

    settings.beginWriteArray("endpoints");
    for (int i = 0; i < node->endpoints().count(); i++) {
        ZigbeeNodeEndpoint *endpoint = node->endpoints().at(i);
        settings.setArrayIndex(i);
        settings.setValue("id", endpoint->endpointId());
        settings.setValue("profile", endpoint->profile());
        settings.setValue("deviceId", endpoint->deviceId());
        settings.setValue("deviceVersion", endpoint->deviceVersion());
        settings.setValue("manufacturerName", endpoint->manufacturerName());
        settings.setValue("modelIdentifier", endpoint->modelIdentifier());
        settings.setValue("softwareBuildId", endpoint->softwareBuildId());

        settings.beginWriteArray("inputClusters");
        for (int n = 0; n < endpoint->inputClusters().count(); n++) {
            ZigbeeCluster *cluster = endpoint->inputClusters().at(n);
            settings.setArrayIndex(n);
            settings.setValue("clusterId", cluster->clusterId());
        }
        settings.endArray(); // inputClusters

        settings.beginWriteArray("outputClusters");
        for (int n = 0; n < endpoint->outputClusters().count(); n++) {
            ZigbeeCluster *cluster = endpoint->outputClusters().at(n);
            settings.setArrayIndex(n);
            settings.setValue("clusterId", cluster->clusterId());
        }
        settings.endArray(); // outputClusters
    }

    settings.endArray(); // endpoints

    settings.endGroup(); // Node ieee address

    settings.endGroup(); // Nodes
}

void ZigbeeNetwork::removeNodeFromSettings(ZigbeeNode *node)
{
    qCDebug(dcZigbeeNetwork()) << "Remove node" << node << "from settings" << m_settingsFileName;
    QSettings settings(m_settingsFileName, QSettings::IniFormat, this);
    settings.beginGroup("Nodes");

    // Clear settings for this node before storing it
    settings.beginGroup(node->extendedAddress().toString());
    settings.remove("");
    settings.endGroup(); // Node ieee address

    settings.endGroup(); // Nodes
}

void ZigbeeNetwork::addNode(ZigbeeNode *node)
{
    qCDebug(dcZigbeeNetwork()) << "Add node" << node;
    if (hasNode(node->extendedAddress())) {
        qCWarning(dcZigbeeNetwork()) << "Not adding node to the system since already added" << node;
        return;
    }

    addNodeInternally(node);
    saveNode(node);
}

void ZigbeeNetwork::addUnitializedNode(ZigbeeNode *node)
{
    if (m_uninitializedNodes.contains(node)) {
        qCWarning(dcZigbeeNetwork()) << "The uninitialized node" << node << "has already been added.";
        return;
    }

    connect(node, &ZigbeeNode::stateChanged, this, &ZigbeeNetwork::onNodeStateChanged);
    m_uninitializedNodes.append(node);
}

void ZigbeeNetwork::removeNode(ZigbeeNode *node)
{
    qCDebug(dcZigbeeNetwork()) << "Remove node" << node;
    removeNodeInternally(node);
    removeNodeFromSettings(node);
}

void ZigbeeNetwork::setState(ZigbeeNetwork::State state)
{
    if (m_state == state)
        return;

    qCDebug(dcZigbeeNetwork()) << "State changed" << state;
    m_state = state;
    emit stateChanged(m_state);

    if (state == StateRunning) {
        saveNetwork();
        qCDebug(dcZigbeeNetwork()) << this;
    }
}

void ZigbeeNetwork::setError(ZigbeeNetwork::Error error)
{
    if (m_error == error)
        return;

    if (m_error != ErrorNoError) qCDebug(dcZigbeeNetwork()) << "Error occured" << error;
    m_error = error;
    emit errorOccured(m_error);
}

bool ZigbeeNetwork::networkConfigurationAvailable() const
{
    return m_extendedPanId != 0 && m_channel != 0;
}

ZigbeeNetworkReply *ZigbeeNetwork::createNetworkReply(const ZigbeeNetworkRequest &request)
{
    ZigbeeNetworkReply *reply = new ZigbeeNetworkReply(request, this);
    // Make sure the reply will be deleted
    connect(reply, &ZigbeeNetworkReply::finished, reply, &ZigbeeNetworkReply::deleteLater);
    return reply;
}

void ZigbeeNetwork::setReplyResponseError(ZigbeeNetworkReply *reply, Zigbee::ZigbeeApsStatus zigbeeApsStatus)
{
    reply->m_zigbeeApsStatus = zigbeeApsStatus;

    if (reply->m_zigbeeApsStatus == Zigbee::ZigbeeApsStatusSuccess) {
        finishNetworkReply(reply);
    } else {
        finishNetworkReply(reply, ZigbeeNetworkReply::ErrorZigbeeApsStatusError);
    }
}

void ZigbeeNetwork::finishNetworkReply(ZigbeeNetworkReply *reply, ZigbeeNetworkReply::Error error)
{
    qCDebug(dcZigbeeNetwork()) << "Reply finished" << error << reply->request();
    reply->m_error = error;
    reply->finished();
}

void ZigbeeNetwork::onNodeStateChanged(ZigbeeNode::State state)
{
    ZigbeeNode *node = qobject_cast<ZigbeeNode *>(sender());
    if (state == ZigbeeNode::StateInitialized && m_uninitializedNodes.contains(node)) {
        m_uninitializedNodes.removeAll(node);
        disconnect(node, &ZigbeeNode::stateChanged, this, &ZigbeeNetwork::onNodeStateChanged);
        addNode(node);
    }
}

void ZigbeeNetwork::onNodeClusterAttributeChanged(ZigbeeCluster *cluster, const ZigbeeClusterAttribute &attribute)
{
    Q_UNUSED(cluster)
    Q_UNUSED(attribute)

    ZigbeeNode *node = qobject_cast<ZigbeeNode *>(sender());
    saveNode(node);
}


QDebug operator<<(QDebug debug, ZigbeeNetwork *network)
{
    debug.nospace().noquote() << "ZigbeeNetwork (" << ZigbeeUtils::convertUint64ToHexString(network->extendedPanId())
                              << ", Channel " << network->channel()
                              << ")" << endl;
    foreach (ZigbeeNode *node, network->nodes()) {
        debug.nospace().noquote() << "    - " << node << endl;
        debug.nospace().noquote() << "      Node type:" << node->nodeType() << endl;
        debug.nospace().noquote() << "      Manufacturer code: " << ZigbeeUtils::convertUint16ToHexString(node->manufacturerCode()) << endl;
        debug.nospace().noquote() << "      Maximum Rx size: " << ZigbeeUtils::convertUint16ToHexString(node->maximumRxSize()) << endl;
        debug.nospace().noquote() << "      Maximum Tx size: " << ZigbeeUtils::convertUint16ToHexString(node->maximumTxSize()) << endl;
        debug.nospace().noquote() << "      Maximum buffer size: " << ZigbeeUtils::convertByteToHexString(node->maximumBufferSize()) << endl;
        debug.nospace().noquote() << "      Primary Trust center: " << node->isPrimaryTrustCenter() << endl;
        debug.nospace().noquote() << "      Backup Trust center: " << node->isBackupTrustCenter() << endl;
        debug.nospace().noquote() << "      Primary Binding cache: " << node->isPrimaryBindingCache() << endl;
        debug.nospace().noquote() << "      Backup Binding cache: " << node->isBackupBindingCache() << endl;
        debug.nospace().noquote() << "      Primary Discovery cache: " << node->isPrimaryDiscoveryCache() << endl;
        debug.nospace().noquote() << "      Backup Discovery cache: " << node->isBackupDiscoveryCache() << endl;
        debug.nospace().noquote() << "      Network Manager: " << node->isNetworkManager() << endl;
        debug.nospace().noquote() << "      Extended active endpoint list available: " << node->extendedActiveEndpointListAvailable() << endl;
        debug.nospace().noquote() << "      Extended simple descriptor list available: " << node->extendedSimpleDescriptorListAvailable() << endl;
        debug.nospace().noquote() << "      Alternate PAN coordinator: " << node->alternatePanCoordinator() << endl;
        debug.nospace().noquote() << "      Device type: " << node->deviceType() << endl;
        debug.nospace().noquote() << "      Power source flag main power: " << node->powerSourceFlagMainPower() << endl;
        debug.nospace().noquote() << "      Receiver on when idle: " << node->receiverOnWhenIdle() << endl;
        debug.nospace().noquote() << "      Security capability: " << node->securityCapability() << endl;
        debug.nospace().noquote() << "      Allocate address: " << node->allocateAddress() << endl;
        debug.nospace().noquote() << "      Complex desciptor available: " << node->complexDescriptorAvailable() << endl;
        debug.nospace().noquote() << "      User desciptor available: " << node->userDescriptorAvailable() << endl;
        debug.nospace().noquote() << "      Power mode: " << node->powerMode() << endl;
        debug.nospace().noquote() << "      Available power sources:" << endl;
        foreach (const ZigbeeNode::PowerSource &source, node->availablePowerSources()) {
            debug.nospace().noquote() << "       - " << source << endl;
        }
        debug.nospace().noquote() << "      Power source: " << node->powerSource() << endl;
        debug.nospace().noquote() << "      Power level: " << node->powerLevel() << endl;
        debug.nospace().noquote() << "      Endpoints: " << node->endpoints().count() << endl;
        foreach (ZigbeeNodeEndpoint *endpoint, node->endpoints()) {
            debug.nospace().noquote() << "      - " << endpoint << endl;
            debug.nospace().noquote() << "        Input clusters:" << endl;
            foreach (ZigbeeCluster *cluster, endpoint->inputClusters()) {
                debug.nospace().noquote() << "        - " << cluster << endl;
            }
            debug.nospace().noquote() << "        Output clusters:" << endl;
            foreach (ZigbeeCluster *cluster, endpoint->outputClusters()) {
                debug.nospace().noquote() << "        - " << cluster << endl;
            }
        }
    }

    return debug.space();
}
