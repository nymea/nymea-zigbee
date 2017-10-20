#include "zigbeeinterfacemessage.h"
#include "zigbeeutils.h"

ZigbeeInterfaceMessage::ZigbeeInterfaceMessage()
{

}

ZigbeeInterfaceMessage::ZigbeeInterfaceMessage(const Zigbee::InterfaceMessageType &messageType, const QByteArray &data):
    m_messageType(messageType),
    m_data(data)
{

}

Zigbee::InterfaceMessageType ZigbeeInterfaceMessage::messageType() const
{
    return m_messageType;
}

void ZigbeeInterfaceMessage::setMessageType(const Zigbee::InterfaceMessageType &messageType)
{
    m_messageType = messageType;
}

QByteArray ZigbeeInterfaceMessage::data() const
{
    return m_data;
}

void ZigbeeInterfaceMessage::setData(const QByteArray &data)
{
    m_data = data;
}

bool ZigbeeInterfaceMessage::isValid() const
{
    return m_messageType != Zigbee::MessageTypeNone;
}

QDebug operator<<(QDebug dbg, const ZigbeeInterfaceMessage &message)
{
    dbg.nospace().noquote() << ZigbeeUtils::messageTypeToString(message.messageType()) << "(" << ZigbeeUtils::convertUint16ToHexString(static_cast<quint16>(message.messageType())) << ")" << " | " <<  ZigbeeUtils::convertByteArrayToHexString(message.data());
    return dbg.space();
}
