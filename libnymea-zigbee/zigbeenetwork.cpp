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
#include "zdo/zigbeedeviceprofile.h"
#include "zigbeebridgecontroller.h"

#include <QDir>
#include <QFileInfo>

ZigbeeNetwork::ZigbeeNetwork(const QUuid &networkUuid, QObject *parent) :
    QObject(parent),
    m_networkUuid(networkUuid)
{

}

QUuid ZigbeeNetwork::networkUuid() const
{
    return m_networkUuid;
}

ZigbeeNetwork::State ZigbeeNetwork::state() const
{
    return m_state;
}

ZigbeeNetwork::Error ZigbeeNetwork::error() const
{
    return m_error;
}

QDir ZigbeeNetwork::settingsDirectory() const
{
    return m_settingsDirectory;
}

void ZigbeeNetwork::setSettingsDirectory(const QDir &settingsDirectory)
{
    qCDebug(dcZigbeeNetwork()) << "Using settings directory" << settingsDirectory.absolutePath();
    m_settingsDirectory = settingsDirectory;
    emit settingsDirectoryChanged(m_settingsDirectory);

    bridgeController()->setSettingsDirectory(m_settingsDirectory);
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

ZigbeeAddress ZigbeeNetwork::macAddress() const
{
    return m_macAddress;
}

void ZigbeeNetwork::setMacAddress(const ZigbeeAddress &zigbeeAddress)
{
    if (m_macAddress == zigbeeAddress)
        return;

    m_macAddress = zigbeeAddress;
    emit macAddressChanged(m_macAddress);
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

void ZigbeeNetwork::removeZigbeeNode(const ZigbeeAddress &address)
{
    ZigbeeNode *node = getZigbeeNode(address);
    if (!node) {
        qCWarning(dcZigbeeNetwork()) << "Failed remove zigbee node since there is no node with" << address;
        return;
    }

    qCDebug(dcZigbeeNetwork()) << "Removing" << node << "from the newtork";
    ZigbeeDeviceObjectReply *zdoReply = node->deviceObject()->requestMgmtLeaveNetwork();
    connect(zdoReply, &ZigbeeDeviceObjectReply::finished, this, [this, zdoReply, node](){
        if (zdoReply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Failed to send management leave request to" << node << zdoReply->error();
            qCWarning(dcZigbeeNode()) << "This node is gonna be removed internally. TODO: try to remove using ZDO once it shows up the next time.";
        }

        removeNode(node);
    });

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

    // Update database metrics of the node
    connect(node, &ZigbeeNode::lqiChanged, this, [this, node](quint8 lqi){
        m_database->updateNodeLqi(node, lqi);
    });

    connect(node, &ZigbeeNode::lastSeenChanged, this, [this, node](const QDateTime &lastSeen){
        m_database->updateNodeLastSeen(node, lastSeen);
    });

    // Note: if a cluster shows up after initialization (out of spec devices), save the cluster and it's attributes
    foreach (ZigbeeNodeEndpoint *endpoint, node->endpoints()) {
        connect(endpoint, &ZigbeeNodeEndpoint::clusterAttributeChanged, this, &ZigbeeNetwork::onNodeClusterAttributeChanged);
    }

    m_nodes.append(node);
    emit nodeAdded(node);
}

void ZigbeeNetwork::removeNodeInternally(ZigbeeNode *node)
{
    if (!m_nodes.contains(node)) {
        qCWarning(dcZigbeeNetwork()) << "Try to remove node" << node << "but not in the node list.";
        return;
    }

    if (node == m_coordinatorNode) {
        m_coordinatorNode = nullptr;
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
    node->m_macCapabilities = ZigbeeDeviceProfile::parseMacCapabilities(macCapabilities);
    return node;
}

void ZigbeeNetwork::saveNetwork()
{
//    qCDebug(dcZigbeeNetwork()) << "Save current network configuration to" << m_settingsFileName;
//    QSettings settings(m_settingsFileName, QSettings::IniFormat, this);
//    settings.beginGroup("ZigbeeNetwork");
//    settings.setValue("panId", panId());
//    settings.setValue("channel", channel());
//    settings.setValue("networkKey", securityConfiguration().networkKey().toString());
//    settings.setValue("trustCenterLinkKey", securityConfiguration().globalTrustCenterLinkKey().toString());
//    settings.endGroup();
}

void ZigbeeNetwork::loadNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Loading network from settings directory" << m_settingsDirectory.absolutePath();
    if (!m_database) {
        QString networkDatabaseFileName = m_settingsDirectory.absolutePath() + QDir::separator() + QString("zigbee-network-%1.db").arg(m_networkUuid.toString());
        qCDebug(dcZigbeeNetwork()) << "Using ZigBee network database" << QFileInfo(networkDatabaseFileName).fileName();
        m_database = new ZigbeeNetworkDatabase(this, networkDatabaseFileName, this);
    }

//    QSettings settings(m_settingsFileName, QSettings::IniFormat, this);
//    settings.beginGroup("ZigbeeNetwork");
//    quint16 panId = static_cast<quint16>(settings.value("panId", 0).toUInt());
//    setPanId(panId);
//    setChannel(settings.value("channel", 0).toUInt());
//    ZigbeeNetworkKey netKey(settings.value("networkKey", QString()).toString());
//    if (netKey.isValid())
//        m_securityConfiguration.setNetworkKey(netKey);

//    ZigbeeNetworkKey tcKey(settings.value("trustCenterLinkKey", QString("5A6967426565416C6C69616E63653039")).toString());
//    if (!tcKey.isValid())
//        m_securityConfiguration.setGlobalTrustCenterlinkKey(tcKey);

//    settings.endGroup(); // Network

    QList<ZigbeeNode *> nodes = m_database->loadNodes();
    foreach (ZigbeeNode *node, nodes) {
        node->setState(ZigbeeNode::StateInitialized);
        addNodeInternally(node);
    }
}

void ZigbeeNetwork::clearSettings()
{
    // Note: this clears the database
    qCDebug(dcZigbeeNetwork()) << "Remove zigbee nodes from network";
    foreach (ZigbeeNode *node, m_nodes) {
        removeNode(node);
    }

    qCDebug(dcZigbeeNetwork()) << "Clear all uninitialized nodes";
    foreach (ZigbeeNode *node, m_uninitializedNodes) {
        qCDebug(dcZigbeeNetwork()) << "Remove uninitialized" << node;
        m_uninitializedNodes.removeAll(node);
        node->deleteLater();
    }

    qCDebug(dcZigbeeNetwork()) << "Delete network database";
    if (m_database) {
        if (!m_database->wipeDatabase()) {
            qCWarning(dcZigbeeNetwork()) << "Failed to wipe the network database" << m_database->databaseName();
        }
    }

    // Reset network configurations
    qCDebug(dcZigbeeNetwork()) << "Clear network properties";
    setExtendedPanId(0);
    setChannel(0);
    setSecurityConfiguration(ZigbeeSecurityConfiguration());
    setState(StateUninitialized);
    m_nodeType = ZigbeeDeviceProfile::NodeTypeCoordinator;
}

bool ZigbeeNetwork::hasUninitializedNode(const ZigbeeAddress &address) const
{
    foreach (ZigbeeNode *node, m_uninitializedNodes) {
        if (node->extendedAddress() == address) {
            return true;
        }
    }

    return false;
}

void ZigbeeNetwork::addNode(ZigbeeNode *node)
{
    qCDebug(dcZigbeeNetwork()) << "Add node" << node;
    if (hasNode(node->extendedAddress())) {
        qCWarning(dcZigbeeNetwork()) << "Not adding node to the system since already added" << node;
        return;
    }

    m_database->saveNode(node);
    addNodeInternally(node);
}

void ZigbeeNetwork::addUnitializedNode(ZigbeeNode *node)
{
    if (m_uninitializedNodes.contains(node)) {
        qCWarning(dcZigbeeNetwork()) << "The uninitialized node" << node << "has already been added.";
        return;
    }

    connect(node, &ZigbeeNode::stateChanged, this, &ZigbeeNetwork::onNodeStateChanged);
    connect(node, &ZigbeeNode::nodeInitializationFailed, this, [this, node](){
        qCWarning(dcZigbeeNetwork()) << "The initialization procedure for" << node << "failed. Please retry to add this node by restarting the init procedure.";
        m_uninitializedNodes.removeAll(node);
        node->deleteLater();
    });

    m_uninitializedNodes.append(node);
}

void ZigbeeNetwork::removeNode(ZigbeeNode *node)
{
    qCDebug(dcZigbeeNetwork()) << "Remove node" << node;
    removeNodeInternally(node);
    m_database->removeNode(node);
}

void ZigbeeNetwork::removeUninitializedNode(ZigbeeNode *node)
{
    qCDebug(dcZigbeeNetwork()) << "Remove uninitialized node" << node;
    m_uninitializedNodes.removeAll(node);
    node->deleteLater();
}

void ZigbeeNetwork::setState(ZigbeeNetwork::State state)
{
    if (m_state == state)
        return;

    qCDebug(dcZigbeeNetwork()) << "State changed" << state;
    m_state = state;

    if (state == StateRunning) {
        saveNetwork();
        qCDebug(dcZigbeeNetwork()) << this;
    }
    emit stateChanged(m_state);
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
    return m_extendedPanId != 0 && m_channel != 0 && m_coordinatorNode;
}

void ZigbeeNetwork::handleNodeIndication(ZigbeeNode *node, const Zigbee::ApsdeDataIndication indication)
{
    node->handleDataIndication(indication);
}

ZigbeeNetworkReply *ZigbeeNetwork::createNetworkReply(const ZigbeeNetworkRequest &request)
{
    ZigbeeNetworkReply *reply = new ZigbeeNetworkReply(request, this);
    // Make sure the reply will be deleted
    connect(reply, &ZigbeeNetworkReply::finished, reply, &ZigbeeNetworkReply::deleteLater, Qt::QueuedConnection);
    return reply;
}

void ZigbeeNetwork::setReplyResponseError(ZigbeeNetworkReply *reply, Zigbee::ZigbeeApsStatus zigbeeApsStatus)
{
    if (zigbeeApsStatus == Zigbee::ZigbeeApsStatusSuccess) {
        // The request has been sent successfully to the device
        finishNetworkReply(reply);
    } else {
        // There has been an error while transporting the request to the device
        // Note: if the APS status is >= 0xc1, it has to interpreted as NWK layer error
        if (zigbeeApsStatus >= 0xc1) {
            reply->m_zigbeeNwkStatus = static_cast<Zigbee::ZigbeeNwkLayerStatus>(static_cast<quint8>(zigbeeApsStatus));
            finishNetworkReply(reply, ZigbeeNetworkReply::ErrorZigbeeNwkStatusError);
        } else {
            reply->m_zigbeeApsStatus = zigbeeApsStatus;
            finishNetworkReply(reply, ZigbeeNetworkReply::ErrorZigbeeApsStatusError);
        }
    }
}

void ZigbeeNetwork::finishNetworkReply(ZigbeeNetworkReply *reply, ZigbeeNetworkReply::Error error)
{
    reply->m_error = error;
    switch(reply->error()) {
    case ZigbeeNetworkReply::ErrorNoError:
        qCDebug(dcZigbeeNetwork()) << "Network request sent successfully to device" << reply->request();
        break;
    case ZigbeeNetworkReply::ErrorZigbeeApsStatusError:
        qCWarning(dcZigbeeNetwork()) << "Failed to send request to device" << reply->request() << reply->error() << reply->zigbeeApsStatus();
        break;
    case ZigbeeNetworkReply::ErrorZigbeeNwkStatusError:
        qCWarning(dcZigbeeNetwork()) << "Failed to send request to device" << reply->request() << reply->error() << reply->zigbeeNwkStatus();
        break;
    default:
        qCWarning(dcZigbeeNetwork()) << "Failed to send request to device" << reply->request() << reply->error();
        break;
    }

    // Stop the timer
    reply->m_timer->stop();

    // Finish the reply
    reply->finished();
}

void ZigbeeNetwork::startWaitingReply(ZigbeeNetworkReply *reply)
{
    reply->m_timer->start();
}

void ZigbeeNetwork::onNodeStateChanged(ZigbeeNode::State state)
{
    ZigbeeNode *node = qobject_cast<ZigbeeNode *>(sender());
    if (state == ZigbeeNode::StateInitialized && m_uninitializedNodes.contains(node)) {
        m_uninitializedNodes.removeAll(node);
        // Disconnect this slot since we don't need it any more
        disconnect(node, &ZigbeeNode::stateChanged, this, &ZigbeeNetwork::onNodeStateChanged);
        addNode(node);
    }
}

void ZigbeeNetwork::onNodeClusterAttributeChanged(ZigbeeCluster *cluster, const ZigbeeClusterAttribute &attribute)
{
    m_database->saveAttribute(cluster, attribute);
}

QDebug operator<<(QDebug debug, ZigbeeNetwork *network)
{
    debug.nospace().noquote() << "ZigbeeNetwork (" << ZigbeeUtils::convertUint16ToHexString(network->panId())
                              << ", Channel " << network->channel()
                              << ")" << "\n";
    foreach (ZigbeeNode *node, network->nodes()) {
        debug.nospace().noquote() << " ---> " << node << "\n";
        debug.nospace().noquote() << "  " << node->nodeDescriptor();
        debug.nospace().noquote() << "  " << node->powerDescriptor();
        debug.nospace().noquote() << "  Endpoints: " << node->endpoints().count() << "\n";
        foreach (ZigbeeNodeEndpoint *endpoint, node->endpoints()) {
            debug.nospace().noquote() << "    - " << endpoint << "\n";
            debug.nospace().noquote() << "      Input clusters:" << "\n";
            foreach (ZigbeeCluster *cluster, endpoint->inputClusters()) {
                debug.nospace().noquote() << "      - " << cluster << "\n";
                foreach (const ZigbeeClusterAttribute &attribute, cluster->attributes()) {
                    debug.nospace().noquote() << "        - " << attribute << "\n";
                }
            }
            debug.nospace().noquote() << "      Output clusters:" << "\n";
            foreach (ZigbeeCluster *cluster, endpoint->outputClusters()) {
                debug.nospace().noquote() << "      - " << cluster << "\n";
                foreach (const ZigbeeClusterAttribute &attribute, cluster->attributes()) {
                    debug.nospace().noquote() << "        - " << attribute << "\n";
                }
            }
        }
    }

    return debug.space();
}
