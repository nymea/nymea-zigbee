#ifndef ZIGBEEINTERFACEREQUEST_H
#define ZIGBEEINTERFACEREQUEST_H

#include "zigbeeinterfacemessage.h"

class ZigbeeInterfaceRequest
{
public:
    ZigbeeInterfaceRequest();
    ZigbeeInterfaceRequest(const ZigbeeInterfaceMessage &message);

    void setDescription(const QString &description);
    QString description() const;

    void setMessage(const ZigbeeInterfaceMessage &message);
    ZigbeeInterfaceMessage message() const;

    bool expectsAdditionalMessage() const;

    void setExpectedAdditionalMessageType(const Zigbee::InterfaceMessageType &messageType);
    Zigbee::InterfaceMessageType expectedAdditionalMessageType() const;

    void setTimoutIntervall(const int &timeoutIntervall);
    int timeoutIntervall() const;

private:
    QString m_description;
    ZigbeeInterfaceMessage m_message;
    Zigbee::InterfaceMessageType m_expectedAdditionalMessageType = Zigbee::MessageTypeNone;
    bool m_expectsAdditionalMessage = false;
    int m_timeoutIntervall = 500;

};

#endif // ZIGBEEINTERFACEREQUEST_H
