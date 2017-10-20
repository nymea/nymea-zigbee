#include "zigbeeinterfacerequest.h"

ZigbeeInterfaceRequest::ZigbeeInterfaceRequest()
{

}

ZigbeeInterfaceRequest::ZigbeeInterfaceRequest(const ZigbeeInterfaceMessage &message):
    m_message(message)
{

}

void ZigbeeInterfaceRequest::setDescription(const QString &description)
{
    m_description = description;
}

QString ZigbeeInterfaceRequest::description() const
{
    return m_description;
}

void ZigbeeInterfaceRequest::setMessage(const ZigbeeInterfaceMessage &message)
{
    m_message = message;
}

ZigbeeInterfaceMessage ZigbeeInterfaceRequest::message() const
{
    return m_message;
}

bool ZigbeeInterfaceRequest::expectsAdditionalMessage() const
{
    return m_expectsAdditionalMessage;
}

void ZigbeeInterfaceRequest::setExpectedAdditionalMessageType(const Zigbee::InterfaceMessageType &messageType)
{
    m_expectedAdditionalMessageType = messageType;
    m_expectsAdditionalMessage = true;
}

Zigbee::InterfaceMessageType ZigbeeInterfaceRequest::expectedAdditionalMessageType() const
{
    return m_expectedAdditionalMessageType;
}

void ZigbeeInterfaceRequest::setTimoutIntervall(const int &timeoutIntervall)
{
    m_timeoutIntervall = timeoutIntervall;
}

int ZigbeeInterfaceRequest::timeoutIntervall() const
{
    return m_timeoutIntervall;
}
