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

QString ZigbeeNetwork::serialPortName() const
{
    return m_serialPortName;
}

void ZigbeeNetwork::setSerialPortName(const QString &serialPortName)
{
    m_serialPortName = serialPortName;
}

qint32 ZigbeeNetwork::serialBaudrate() const
{
    return m_serialBaudrate;
}

void ZigbeeNetwork::setSerialBaudrate(qint32 baudrate)
{
    m_serialBaudrate = baudrate;
}

quint64 ZigbeeNetwork::extendedPanId() const
{
    return m_extendedPanId;
}

void ZigbeeNetwork::setExtendedPanId(quint64 extendedPanId)
{
    m_extendedPanId = extendedPanId;
}

uint ZigbeeNetwork::channel() const
{
    return m_channel;
}

void ZigbeeNetwork::setChannel(uint channel)
{
    m_channel = channel;
}

ZigbeeSecurityConfiguration ZigbeeNetwork::securityConfiguration() const
{
    return m_securityConfiguration;
}

void ZigbeeNetwork::setSecurityConfiguration(const ZigbeeSecurityConfiguration &securityConfiguration)
{
    m_securityConfiguration = securityConfiguration;
}

QList<ZigbeeNode *> ZigbeeNetwork::nodes() const
{
    return m_nodes;
}

ZigbeeNode *ZigbeeNetwork::getZigbeeNode(quint16 shortAddress)
{
    foreach (ZigbeeNode *node, m_nodes) {
        if (node->shortAddress() == shortAddress) {
            return node;
        }
    }

    return nullptr;
}

ZigbeeNode *ZigbeeNetwork::getZigbeeNode(ZigbeeAddress address)
{
    foreach (ZigbeeNode *node, m_nodes) {
        if (node->extendedAddress() == address) {
            return node;
        }
    }

    return nullptr;
}

void ZigbeeNetwork::addNode(ZigbeeNode *node)
{
    if (m_nodes.contains(node)) {
        qCWarning(dcZigbeeNetwork()) << "The node" << node << "has already been added.";
        return;
    }

    m_nodes.append(node);
    emit nodeAdded(node);
}

void ZigbeeNetwork::removeNode(ZigbeeNode *node)
{
    if (!m_nodes.contains(node)) {
        qCWarning(dcZigbeeNetwork()) << "Try to remove node" << node << "but not in the node list.";
        return;
    }

    m_nodes.removeAll(node);
    emit nodeRemoved(node);
}

void ZigbeeNetwork::setState(ZigbeeNetwork::State state)
{
    if (m_state == state)
        return;

    qCDebug(dcZigbeeNetwork()) << "State changed" << state;
    m_state = state;
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
