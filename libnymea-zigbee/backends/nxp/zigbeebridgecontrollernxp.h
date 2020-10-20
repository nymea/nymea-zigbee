#ifndef ZIGBEEBRIDGECONTROLLERNXP_H
#define ZIGBEEBRIDGECONTROLLERNXP_H

#include <QHash>
#include <QTimer>
#include <QQueue>
#include <QObject>

#include "zigbee.h"
#include "zigbeeaddress.h"
#include "zigbeenetworkkey.h"
#include "zigbeenetworkrequest.h"
#include "zigbeebridgecontroller.h"
#include "interface/zigbeeinterfacenxp.h"
#include "interface/zigbeeinterfacenxpreply.h"

class ZigbeeBridgeControllerNxp : public ZigbeeBridgeController
{
    Q_OBJECT

    friend class ZigbeeNetworkNxp;

public:
    explicit ZigbeeBridgeControllerNxp(QObject *parent = nullptr);
    ~ZigbeeBridgeControllerNxp() override;

    enum ControllerState {
        ControllerStateRunning = 0x00,
        ControllerStateBooting = 0x01,
        ControllerStateStarting = 0x02,
        ControllerStateRunningUninitialized = 0x03,
        ControllerStateNotRunning = 0x04
    };
    Q_ENUM(ControllerState)

    ControllerState controllerState() const;
    void refreshControllerState();

    // Controllere requests
    ZigbeeInterfaceNxpReply *requestVersion();
    ZigbeeInterfaceNxpReply *requestControllerState();
    ZigbeeInterfaceNxpReply *requestSoftResetController();
    ZigbeeInterfaceNxpReply *requestFactoryResetController();

    // Configure network
    ZigbeeInterfaceNxpReply *requestSetPanId(quint64 panId);
    ZigbeeInterfaceNxpReply *requestSetChannelMask(quint32 channelMask);
    ZigbeeInterfaceNxpReply *requestSetSecurityKey(Nxp::KeyType keyType, const ZigbeeNetworkKey &key);

    // Network commands
    ZigbeeInterfaceNxpReply *requestStartNetwork();
    ZigbeeInterfaceNxpReply *requestNetworkState();
    ZigbeeInterfaceNxpReply *requestSetPermitJoinCoordinator(quint8 duration);

    // APS
    ZigbeeInterfaceNxpReply *requestSendRequest(const ZigbeeNetworkRequest &request);


signals:
    void controllerStateChanged(ControllerState controllerState);
    void interfaceNotificationReceived(Nxp::Notification notification, const QByteArray &data);

private:
    ZigbeeInterfaceNxp *m_interface = nullptr;
    ControllerState m_controllerState = ControllerStateNotRunning;
    quint8 m_sequenceNumber = 0;

    ZigbeeInterfaceNxpReply *m_currentReply = nullptr;
    QQueue<ZigbeeInterfaceNxpReply *> m_replyQueue;
    ZigbeeInterfaceNxpReply *createReply(Nxp::Command command, quint8 sequenceNumber, const QString &requestName, const QByteArray &requestData, QObject *parent);

    void bumpSequenceNumber();

    ZigbeeInterfaceNxpReply *requestEnqueueSendDataGroup(quint8 requestId, quint16 groupAddress, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius = 0);
    ZigbeeInterfaceNxpReply *requestEnqueueSendDataShortAddress(quint8 requestId, quint16 shortAddress, quint8 destinationEndpoint, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius = 0);
    ZigbeeInterfaceNxpReply *requestEnqueueSendDataIeeeAddress(quint8 requestId, ZigbeeAddress ieeeAddress, quint8 destinationEndpoint, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius = 0);


private slots:
    void onInterfaceAvailableChanged(bool available);
    void onInterfacePackageReceived(const QByteArray &package);

    void sendNextRequest();

public slots:
    bool enable(const QString &serialPort, qint32 baudrate);
    void disable();

};

#endif // ZIGBEEBRIDGECONTROLLERNXP_H
