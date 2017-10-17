#ifndef ZIGBEEINTERFACEMESSAGE_H
#define ZIGBEEINTERFACEMESSAGE_H

#include <QObject>
#include <QDebug>

#include "zigbee.h"

class ZigbeeInterfaceMessage
{
public:
    ZigbeeInterfaceMessage();
    ZigbeeInterfaceMessage(const Zigbee::InterfaceMessageType &messageType, const QByteArray &data = QByteArray());

    Zigbee::InterfaceMessageType messageType() const;
    void setMessageType(const Zigbee::InterfaceMessageType &messageType);

    QByteArray data() const;
    void setData(const QByteArray &data);

private:
    Zigbee::InterfaceMessageType m_messageType;
    QByteArray m_data;

};

QDebug operator<<(QDebug dbg, const ZigbeeInterfaceMessage &message);

#endif // ZIGBEEINTERFACEMESSAGE_H
