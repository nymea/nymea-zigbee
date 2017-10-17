#include "zigbeeinterfacemessage.h"

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

QDebug operator<<(QDebug dbg, const ZigbeeInterfaceMessage &message)
{
    dbg.nospace().noquote() << "InterfaceMessage(" << message.messageType() << Zigbee::convertByteArrayToHexString(message.data()) << ")";
    return dbg.space();
}
