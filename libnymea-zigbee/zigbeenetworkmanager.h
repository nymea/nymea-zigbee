#ifndef ZIGBEEMANAGER_H
#define ZIGBEEMANAGER_H

#include <QObject>

#include "zigbeenode.h"
#include "zigbeeaddress.h"
#include "zigbeebridgecontroller.h"

class ZigbeeNetworkManager : public ZigbeeNode
{
    Q_OBJECT
public:
    explicit ZigbeeNetworkManager(const int &channel = 0, const QString &serialPort = "/dev/ttyS0", qint32 baudrate = 115200, QObject *parent = nullptr);

    bool isAvailable() const;

    QString controllerVersion() const;
    QString serialPort() const;
    QList<ZigbeeNode *> nodeList() const;
    quint64 extendedPanId() const;

    bool networkRunning() const;

    // Controller methods
    void resetController();
    void erasePersistentData();
    void sendDataManagerAvailableResponse();
    void getVersion();
    void setExtendedPanId(const quint64 &panId);
    void setChannelMask(const quint32 &channelMask = 0x07fff800);
    void setDeviceType(const NodeType &deviceType);
    void startNetwork();
    void startScan();

    void permitJoining(quint16 targetAddress = 0xfffc, const quint8 advertisingIntervall = 180, bool tcSignificance = false);

    void requestLinkQuality();

    void getPermitJoiningStatus();
    void enableWhitelist();

    void initiateTouchLink();
    void touchLinkFactoryReset();

    void networkAddressRequest(quint16 targetAddress, quint64 extendedAddress);

    void requestMatchDescriptor(const quint16 &shortAddress, const Zigbee::ZigbeeProfile &profile);

    void setInitialSecurity(quint8 keyState, quint8 keySequence, quint8 keyType, const QString &key);

    void authenticateDevice(const ZigbeeAddress &ieeeAddress);

private:
    ZigbeeBridgeController *m_controller = nullptr;
    QString m_serialPort;
    QString m_controllerVersion;
    quint64 m_extendedPanId = 0;

    QList<ZigbeeNode *> m_nodeList;

    bool m_networkRunning = false;

    quint64 generateRandomPanId();

    void parseNetworkFormed(const QByteArray &data);

signals:
    void runningChanged(const bool &running);

private slots:
    void onMessageReceived(const ZigbeeInterfaceMessage &message);

    void loadNetwork();
    void saveNetwork();

    // Controller methods finished slots
    void onResetControllerFinished();
    void onErasePersistentDataFinished();
    void onGetVersionFinished();
    void onSetExtendedPanIdFinished();
    void onSetChannelMaskFinished();
    void onSetDeviceTypeFinished();
    void onStartNetworkFinished();
    void onStartScanFinished();
    void onGetPermitJoiningStatusFinished();
    void onPermitJoiningFinished();
    void onEnableWhitelistFinished();

    void onInitiateTouchLinkFinished();
    void onTouchLinkFactoryResetFinished();
    void onRequestLinkQualityFinished();

    void onRequestMatchDescriptorFinished();
    void onSetSecurityFinished();
    void onNetworkAddressRequestFinished();
    void onAuthenticateDeviceFinished();

    // Process controller notifications/messages
    void processLoggingMessage(const ZigbeeInterfaceMessage &message);
    void processFactoryNewRestart(const ZigbeeInterfaceMessage &message);
    void processNodeClusterList(const ZigbeeInterfaceMessage &message);
    void processNodeAttributeList(const ZigbeeInterfaceMessage &message);
    void processNodeCommandIdList(const ZigbeeInterfaceMessage &message);
    void processDeviceAnnounce(const ZigbeeInterfaceMessage &message);
    void processAttributeReport(const ZigbeeInterfaceMessage &message);
    void processLeaveIndication(const ZigbeeInterfaceMessage &message);
    void processRestartProvisioned(const ZigbeeInterfaceMessage &message);
};

#endif // ZIGBEEMANAGER_H
