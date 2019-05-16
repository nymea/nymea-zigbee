#ifndef ZIGBEEBRIDGECONTROLLER_H
#define ZIGBEEBRIDGECONTROLLER_H

#include <QObject>
#include <QQueue>

#include "zigbee.h"
#include "zigbeenode.h"
#include "interface/zigbeeinterface.h"
#include "interface/zigbeeinterfacereply.h"
#include "interface/zigbeeinterfacerequest.h"
#include "interface/zigbeeinterfacemessage.h"

class ZigbeeBridgeController : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeBridgeController(QObject *parent = nullptr);
    ~ZigbeeBridgeController();

    bool available() const;

    // Direct commands
    ZigbeeInterfaceReply *commandResetController();
    ZigbeeInterfaceReply *commandSoftResetController();
    ZigbeeInterfaceReply *commandErasePersistantData();
    ZigbeeInterfaceReply *commandGetVersion();
    ZigbeeInterfaceReply *commandSetExtendedPanId(quint64 extendedPanId);
    ZigbeeInterfaceReply *commandSetChannelMask(quint32 channelMask = 0x07fff800);
    ZigbeeInterfaceReply *commandSetNodeType(ZigbeeNode::NodeType nodeType);
    ZigbeeInterfaceReply *commandStartNetwork();
    ZigbeeInterfaceReply *commandStartScan();
    ZigbeeInterfaceReply *commandPermitJoin(quint16 targetAddress = 0xfffc, const quint8 advertisingIntervall = 180, bool tcSignificance = false);
    ZigbeeInterfaceReply *commandGetPermitJoinStatus();
    ZigbeeInterfaceReply *commandRequestLinkQuality(quint16 shortAddress);
    ZigbeeInterfaceReply *commandEnableWhiteList();
    ZigbeeInterfaceReply *commandInitiateTouchLink();
    ZigbeeInterfaceReply *commandTouchLinkFactoryReset();
    ZigbeeInterfaceReply *commandNetworkAddressRequest(quint16 targetAddress, quint64 extendedAddress);
    ZigbeeInterfaceReply *commandSetSecurityStateAndKey(quint8 keyState, quint8 keySequence, quint8 keyType, const QString &key);
    ZigbeeInterfaceReply *commandAuthenticateDevice(const ZigbeeAddress &ieeeAddress, const QString &key);
    ZigbeeInterfaceReply *commandNodeDescriptorRequest(quint16 shortAddress);
    ZigbeeInterfaceReply *commandSimpleDescriptorRequest(quint16 shortAddress, quint8 endpoint);
    ZigbeeInterfaceReply *commandPowerDescriptorRequest(quint16 shortAddress);
    ZigbeeInterfaceReply *commandUserDescriptorRequest(quint16 shortAddress, quint16 address);

private:
    ZigbeeInterface *m_interface = nullptr;
    ZigbeeInterfaceReply *m_currentReply = nullptr;

    QQueue<ZigbeeInterfaceReply *> m_replyQueue;

    void sendMessage(ZigbeeInterfaceReply *reply);

signals:
    void availableChanged(bool available);
    void messageReceived(const ZigbeeInterfaceMessage &message);

private slots:
    void onMessageReceived(const ZigbeeInterfaceMessage &message);
    void onReplyTimeout();

public slots:
    bool enable(const QString &serialPort, qint32 baudrate);
    void disable();

    ZigbeeInterfaceReply *sendRequest(const ZigbeeInterfaceRequest &request);

};

#endif // ZIGBEEBRIDGECONTROLLER_H
