#include "zigbeeutils.h"
#include "zigbeenetwork.h"
#include "loggingcategory.h"

ZigbeeNetwork::ZigbeeNetwork(ControllerType controllerType, QObject *parent) :
    ZigbeeNode(parent),
    m_controllerType(controllerType)
{

}

ZigbeeNetwork::State ZigbeeNetwork::state() const
{
    return m_state;
}

ZigbeeNetwork::ControllerType ZigbeeNetwork::controlerType() const
{
    return m_controllerType;
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

uint ZigbeeNetwork::channel() const
{
    return m_channel;
}

void ZigbeeNetwork::setChannel(uint channel)
{
    if (m_channel == channel)
        return;

    m_channel = channel;
    emit channelChanged(m_channel);
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

QList<ZigbeeNode *> ZigbeeNetwork::nodes() const
{
    return m_nodes;
}

ZigbeeNode *ZigbeeNetwork::coordinatorNode() const
{
    return getZigbeeNode(0);
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

    node->setConnected(state() == StateRunning);

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

void ZigbeeNetwork::saveNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Save current network configuration to" << m_settingsFileName;
    QSettings settings(m_settingsFileName, QSettings::IniFormat, this);
    settings.beginGroup("Network");
    settings.setValue("panId", extendedPanId());
    settings.setValue("channel", channel());
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
    quint64 extendedPanId = static_cast<quint64>(settings.value("panId", 0).toULongLong());
    if (extendedPanId == 0) {
        extendedPanId = ZigbeeUtils::generateRandomPanId();
        qCDebug(dcZigbeeNetwork()) << "Create new PAN ID" << extendedPanId;
    }
    setExtendedPanId(extendedPanId);
    setChannel(settings.value("channel", 0).toUInt());
    settings.endGroup();

    // Load nodes
    settings.beginGroup("Nodes");
    foreach (const QString ieeeAddressString, settings.childGroups()) {
        settings.beginGroup(ieeeAddressString);

        ZigbeeNode *node = createNode();
        node->setExtendedAddress(ZigbeeAddress(ieeeAddressString));
        node->setShortAddress(static_cast<quint16>(settings.value("nwkAddress", 0).toUInt()));
        // TODO: load the rest of the node

        settings.beginGroup("inputCluster");
        foreach (const QString &clusterIdString, settings.childGroups()) {
           settings.beginGroup(clusterIdString);
           Zigbee::ClusterId clusterId = static_cast<Zigbee::ClusterId>(clusterIdString.toInt());

           foreach (const QString &attributeIdString, settings.childGroups()) {
               settings.beginGroup(attributeIdString);
               quint16 id = static_cast<quint16>(attributeIdString.toInt());
               Zigbee::DataType dataType = static_cast<Zigbee::DataType>(settings.value("dataType", 0).toInt());
               QByteArray data = settings.value("data").toByteArray();
               node->setClusterAttribute(clusterId, ZigbeeClusterAttribute(id, dataType, data));
               settings.endGroup(); // attributeId
           }
           settings.endGroup(); // clusterId
        }
        settings.endGroup(); // inputCluster

        // Output cluster
        settings.beginGroup("outputCluster");
        foreach (const QString &clusterIdString, settings.childGroups()) {
           settings.beginGroup(clusterIdString);
           Zigbee::ClusterId clusterId = static_cast<Zigbee::ClusterId>(clusterIdString.toInt());

           foreach (const QString &attributeIdString, settings.childGroups()) {
               settings.beginGroup(attributeIdString);
               quint16 id = static_cast<quint16>(attributeIdString.toInt());
               Zigbee::DataType dataType = static_cast<Zigbee::DataType>(settings.value("dataType", 0).toInt());
               QByteArray data = settings.value("data").toByteArray();
               node->setClusterAttribute(clusterId, ZigbeeClusterAttribute(id, dataType, data));
               settings.endGroup(); // attributeId
           }
           settings.endGroup(); // clusterId
        }
        settings.endGroup(); // inputCluster

        node->setState(StateInitialized);
        addNodeInternally(node);

        settings.endGroup(); // ieeeAddress
    }
    settings.endGroup(); // Nodes

    qCDebug(dcZigbeeNetwork()) << "Extended PAN ID:" << m_extendedPanId << ZigbeeUtils::convertUint64ToHexString(m_extendedPanId);
    qCDebug(dcZigbeeNetwork()) << "Channel" << m_channel;
    qCDebug(dcZigbeeNetwork()) << QStringLiteral("Nodes: (%1)").arg(m_nodes.count());
    foreach (ZigbeeNode *node, nodes()) {
        qCDebug(dcZigbeeNetwork()) << "  - " << node;
        qCDebug(dcZigbeeNetwork()) << "Output cluster:";
        foreach (ZigbeeCluster *cluster, node->outputClusters()) {
            qCDebug(dcZigbeeNetwork()) << "    " << cluster;
            foreach (const ZigbeeClusterAttribute &attribute, cluster->attributes()) {
                qCDebug(dcZigbeeNetwork()) << "        " << attribute;
            }
        }

        qCDebug(dcZigbeeNetwork()) << "Input cluster:";
        foreach (ZigbeeCluster *cluster, node->inputClusters()) {
            qCDebug(dcZigbeeNetwork()) << "    " << cluster;
            foreach (const ZigbeeClusterAttribute &attribute, cluster->attributes()) {
                qCDebug(dcZigbeeNetwork()) << "        " << attribute;
            }
        }
    }
}

void ZigbeeNetwork::clearSettings()
{
    qCDebug(dcZigbeeNetwork()) << "Clear network settings";

    // Reset network configurations
    m_extendedPanId = 0;
    m_channel = 0;
    m_securityConfiguration.clear();
    m_nodeType = ZigbeeNode::NodeTypeCoordinator;

    qCDebug(dcZigbeeNetwork()) << "Remove zigbee nodes from network";
    foreach (ZigbeeNode *node, m_nodes) {
        removeNode(node);
    }

    qCDebug(dcZigbeeNetwork()) << "Clear network settings" << m_settingsFileName;
    QSettings settings(m_settingsFileName, QSettings::IniFormat, this);
    settings.clear();
}

void ZigbeeNetwork::saveNode(ZigbeeNode *node)
{
    QSettings settings(m_settingsFileName, QSettings::IniFormat, this);
    settings.beginGroup("Nodes");

    // Clear settings for this node before storing it
    settings.beginGroup(node->extendedAddress().toString());
    settings.remove("");
    settings.endGroup();

    // Save this node
    settings.beginGroup(node->extendedAddress().toString());
    settings.setValue("nwkAddress", node->shortAddress());
    // TODO: save the rest of the node

    // Input clusters
    settings.beginGroup("inputCluster");
    foreach (ZigbeeCluster *cluster, node->inputClusters()) {
        settings.beginGroup(QString::number(static_cast<int>(cluster->clusterId())));
        foreach (const ZigbeeClusterAttribute &attribute, cluster->attributes()) {
            settings.beginGroup(QString::number(static_cast<int>(attribute.id())));
            settings.setValue("dataType", static_cast<int>(attribute.dataType()));
            settings.setValue("data", attribute.data());
            settings.endGroup(); // attributeId
        }
        settings.endGroup(); // clusterId
    }
    settings.endGroup(); // inputCluster


    // Output clusters
    settings.beginGroup("outputCluster");
    foreach (ZigbeeCluster *cluster, node->outputClusters()) {
        settings.beginGroup(QString::number(static_cast<int>(cluster->clusterId())));
        foreach (const ZigbeeClusterAttribute &attribute, cluster->attributes()) {
            settings.beginGroup(QString::number(static_cast<int>(attribute.id())));
            settings.setValue("dataType", static_cast<int>(attribute.dataType()));
            settings.setValue("data", attribute.data());
            settings.endGroup(); // attributeId
        }
        settings.endGroup(); // clusterId
    }
    settings.endGroup(); // inputCluster

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
    settings.endGroup();

    settings.endGroup(); // Nodes
}


void ZigbeeNetwork::addNode(ZigbeeNode *node)
{
    qCDebug(dcZigbeeNetwork()) << "Add node" << node;
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

ZigbeeNode *ZigbeeNetwork::createNode()
{
    ZigbeeNode *node = new ZigbeeNode(this);
    return node;
}

void ZigbeeNetwork::setState(ZigbeeNetwork::State state)
{
    if (m_state == state)
        return;

    qCDebug(dcZigbeeNetwork()) << "State changed" << state;
    m_state = state;
    emit stateChanged(m_state);

    if (state == StateRunning) saveNetwork();
}

void ZigbeeNetwork::setError(ZigbeeNetwork::Error error)
{
    if (m_error == error)
        return;

    if (m_error != ErrorNoError) qCDebug(dcZigbeeNetwork()) << "Error occured" << error;
    m_error = error;
    emit errorOccured(m_error);
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

