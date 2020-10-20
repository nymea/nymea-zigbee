#ifndef ZIGBEENETWORKNXP_H
#define ZIGBEENETWORKNXP_H

#include <QObject>
#include <QTimer>

#include "zigbeenetwork.h"
#include "zigbeechannelmask.h"
#include "zcl/zigbeeclusterlibrary.h"
#include "zigbeebridgecontrollernxp.h"

class ZigbeeNetworkNxp : public ZigbeeNetwork
{
    Q_OBJECT
public:
    explicit ZigbeeNetworkNxp(QObject *parent = nullptr);

    ZigbeeBridgeController *bridgeController() const override;

    ZigbeeNetworkReply *sendRequest(const ZigbeeNetworkRequest &request) override;
    ZigbeeNetworkReply *setPermitJoin(quint16 shortAddress = Zigbee::BroadcastAddressAllRouters, quint8 duration = 0xfe);


private:
    ZigbeeBridgeControllerNxp *m_controller = nullptr;
    bool m_networkRunning = false;
    QHash<quint8, ZigbeeNetworkReply *> m_pendingReplies;

    QTimer *m_permitJoinRefreshTimer = nullptr;

    // ZDO
    void handleZigbeeDeviceProfileIndication(const Zigbee::ApsdeDataIndication &indication);

    // ZCL
    void handleZigbeeClusterLibraryIndication(const Zigbee::ApsdeDataIndication &indication);

private slots:
    void onControllerAvailableChanged(bool available);
    void onControllerStateChanged(ZigbeeBridgeControllerNxp::ControllerState controllerState);
    void onInterfaceNotificationReceived(Nxp::Notification notification, const QByteArray &payload);

    void onApsDataConfirmReceived(const Zigbee::ApsdeDataConfirm &confirm);
    void onApsDataIndicationReceived(const Zigbee::ApsdeDataIndication &indication);

    void onDeviceAnnounced(quint16 shortAddress, ZigbeeAddress ieeeAddress, quint8 macCapabilities);

protected:
    void setPermitJoiningInternal(bool permitJoining) override;

signals:

public slots:
    void startNetwork() override;
    void stopNetwork() override;
    void reset() override;
    void factoryResetNetwork() override;
};

#endif // ZIGBEENETWORKNXP_H
