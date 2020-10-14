#ifndef ZIGBEENETWORKNXP_H
#define ZIGBEENETWORKNXP_H

#include <QObject>

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

private slots:
    void onControllerAvailableChanged(bool available);
    void onControllerStateChanged(ZigbeeBridgeControllerNxp::ControllerState controllerState);
    void onInterfaceNotificationReceived(Nxp::Notification notification, const QByteArray &payload);

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
