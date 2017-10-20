#ifndef ZIGBEEMANAGER_H
#define ZIGBEEMANAGER_H

#include <QObject>

#include "zigbeenode.h"
#include "zigbeebridgecontroller.h"

class ZigbeeNetworkManager : public ZigbeeNode
{
    Q_OBJECT
public:
    explicit ZigbeeNetworkManager(const QString &serialPort = "/dev/ttyS0", QObject *parent = nullptr);

    QString serialPort() const;
    void setSerialPort(const QString &serialPort);

    QString controllerVersion() const;

    void reset();

private:
    ZigbeeBridgeController *m_controller = nullptr;
    QString m_serialPort;
    QString m_controllerVersion;
    quint64 m_extendedPanId;

    quint64 generateRandomPanId();

    // Controller methods
    void resetController();
    void erasePersistentData();
    void sendDataManagerAvailableResponse();
    void getVersion();
    void setExtendedPanId(const quint64 &panId);
    void setChannelMask(const quint32 &channelMask);
    void setDeviceType(const NodeType &deviceType);
    void startNetwork();
    void startScan();
    void permitJoining(quint16 targetAddress = 0xfffc, const quint8 advertisingIntervall = 254);
    void getPermitJoiningStatus();
    void requestNodeDescription(const quint16 &shortAddress);
    void requestSimpleNodeDescription(const quint16 &shortAddress, const quint8 &endpoint = 1);

signals:

private slots:
    void onMessageReceived(const ZigbeeInterfaceMessage &message);

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

    void onRequestNodeDescriptionFinished();
    void onRequestSimpleNodeDescriptionFinished();

    // Controler notifications
    void processLoggingMessage(const ZigbeeInterfaceMessage &message);
    void processFactoryNewRestart(const ZigbeeInterfaceMessage &message);
    void processNodeClusterList(const ZigbeeInterfaceMessage &message);
    void processNodeAttributeList(const ZigbeeInterfaceMessage &message);
    void processNodeCommandIdList(const ZigbeeInterfaceMessage &message);
    void processDeviceAnnounce(const ZigbeeInterfaceMessage &message);
};

#endif // ZIGBEEMANAGER_H
