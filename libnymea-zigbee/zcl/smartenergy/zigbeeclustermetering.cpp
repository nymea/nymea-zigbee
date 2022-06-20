#include "zigbeeclustermetering.h"

#include <QDataStream>
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeCluster)

ZigbeeClusterMetering::ZigbeeClusterMetering(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent):
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdMetering, direction, parent)
{

}

quint64 ZigbeeClusterMetering::currentSummationDelivered() const
{
    return m_currentSummationDelivered;
}

qint32 ZigbeeClusterMetering::instantaneousDemand() const
{
    return m_instantaneousDemand;
}

quint32 ZigbeeClusterMetering::multiplier() const
{
    return m_multiplier;
}

quint32 ZigbeeClusterMetering::divisor() const
{
    return m_divisor;
}

ZigbeeClusterReply *ZigbeeClusterMetering::readFormatting()
{
    ZigbeeClusterReply *readDivisorReply = readAttributes({ZigbeeClusterMetering::AttributeDivisor, ZigbeeClusterMetering::AttributeMultiplier});
    connect(readDivisorReply, &ZigbeeClusterReply::finished, this, [=](){
        if (readDivisorReply->error() != ZigbeeClusterReply::ErrorNoError) {
            qCWarning(dcZigbeeCluster()) << "Failed to read formatting." << readDivisorReply->error();
            return;
        }
    });
    return readDivisorReply;
}

void ZigbeeClusterMetering::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    switch (attribute.id()) {
    case AttributeCurrentSummationDelivered:
        m_currentSummationDelivered = attribute.dataType().toUInt64();
        emit currentSummationDeliveredChanged(m_currentSummationDelivered);
        break;
    case AttributeInstantaneousDemand:
        m_instantaneousDemand = attribute.dataType().toInt32();
        emit instantaneousDemandChanged(m_instantaneousDemand);
        break;
    case AttributeMultiplier:
        m_multiplier = attribute.dataType().toUInt32();
        break;
    case AttributeDivisor:
        m_divisor = attribute.dataType().toUInt32();
        break;
    default:
        qCWarning(dcZigbeeCluster()) << "Unhandled attribute change:" << attribute;
    }
}

void ZigbeeClusterMetering::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    switch (m_direction) {
    case Client:
        qCWarning(dcZigbeeCluster()) << "Metering: Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
        break;
    case Server: {
        ServerCommand command = static_cast<ServerCommand>(frame.header.command);
        switch (command) {
        case CommandDisplayMessage: {
            QDataStream stream(frame.payload);
            stream.setByteOrder(QDataStream::LittleEndian);
            quint32 messageId, time;
            quint16 durationInMinutes;
            quint8 messageControl;
            stream >> messageId >> messageControl >> time >> durationInMinutes;

            char messageData[2048]; // At max 2KB, realistically we'll only ever see < 80B in here as larger messages require both ends to negotiate on a larger PDU, however, if the backend supports it, it *may* happen.
            int messageLength = stream.readRawData(messageData, 2048);
            QByteArray message(messageData, messageLength);
            MessageTransmission messageTransmission = static_cast<MessageTransmission>((messageControl & 0xC0) >> 6);
            MessagePriority priority = static_cast<MessagePriority>((messageControl & 0x30) >> 4);
            bool confirmationRequired = (messageControl & 0x01) == 1;

            qCWarning(dcZigbeeCluster()) << "Display message received!" << messageId << messageControl << time << durationInMinutes << message;
            emit showMessage(messageId, message, time, durationInMinutes, messageTransmission, priority, confirmationRequired);
            break;
        }
        case ClientCommandCancelMessage: {
            QDataStream stream(frame.payload);
            stream.setByteOrder(QDataStream::LittleEndian);
            quint32 messageId;
            quint8 messageControl;
            stream >> messageId >> messageControl;
            MessageTransmission messageTransmission = static_cast<MessageTransmission>((messageControl & 0xC0) >> 6);
            MessagePriority priority = static_cast<MessagePriority>((messageControl & 0x30) >> 4);
            bool confirmationRequired = (messageControl & 0x01) == 1;
            qCDebug(dcZigbeeCluster()) << "Metering: Cancel message command received" << messageId << messageControl;
            emit cancelMessage(messageId, messageTransmission, priority, confirmationRequired);
            break;
        }
        default:
            qCDebug(dcZigbeeCluster()) << "Ignoring out of spec metering cluster ZCL indication:" << m_node << m_endpoint << this << frame;
        }
        break;
    }
    }
}
