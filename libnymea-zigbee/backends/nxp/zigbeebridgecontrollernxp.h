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
public:
    explicit ZigbeeBridgeControllerNxp(QObject *parent = nullptr);
    ~ZigbeeBridgeControllerNxp() override;

    // Controllere requests
    ZigbeeInterfaceNxpReply *requestVersion();

signals:

private:
    ZigbeeInterfaceNxp *m_interface = nullptr;
    quint8 m_sequenceNumber = 0;

    ZigbeeInterfaceNxpReply *createReply(Nxp::Command command, const QString &requestName, const QByteArray &requestData, QObject *parent);

private slots:
    void onInterfaceAvailableChanged(bool available);
    void onInterfacePackageReceived(const QByteArray &package);

public slots:
    bool enable(const QString &serialPort, qint32 baudrate);
    void disable();

};

#endif // ZIGBEEBRIDGECONTROLLERNXP_H
