#ifndef ZIGBEEBRIDGECONTROLLER_H
#define ZIGBEEBRIDGECONTROLLER_H

#include <QObject>
#include <QQueue>

#include "zigbee.h"
#include "interface/zigbeeinterface.h"
#include "interface/zigbeeinterfacereply.h"
#include "interface/zigbeeinterfacerequest.h"
#include "interface/zigbeeinterfacemessage.h"

class ZigbeeBridgeController : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeBridgeController(const QString &serialPort, QObject *parent = nullptr);

    bool available() const;

private:
    ZigbeeInterface *m_interface = nullptr;
    ZigbeeInterfaceReply *m_currentReply = nullptr;

    QQueue<ZigbeeInterfaceReply *> m_replyQueue;

    void sendMessage(ZigbeeInterfaceReply *reply);

signals:
    void messageReceived(const ZigbeeInterfaceMessage &message);

private slots:
    void onMessageReceived(const ZigbeeInterfaceMessage &message);
    void onReplyTimeout();

public slots:
    ZigbeeInterfaceReply *sendRequest(const ZigbeeInterfaceRequest &request);


};

#endif // ZIGBEEBRIDGECONTROLLER_H
