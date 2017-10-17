#ifndef ZIGBEEMANAGER_H
#define ZIGBEEMANAGER_H

#include <QObject>

#include "zigbee.h"
#include "zigbeeinterface.h"
#include "zigbeeinterfacemessage.h"

class ZigbeeManager : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeManager(const QString &serialPort = "/dev/ttyS0", QObject *parent = nullptr);

    QString serialPort() const;
    void setSerialPort(const QString &serialPort);

private:
    ZigbeeInterface *m_interface;
    QString m_serialPort;

    // Controller methods
    void initController();

signals:

private slots:
    void onMessageReceived(const ZigbeeInterfaceMessage &message);

public slots:

};

#endif // ZIGBEEMANAGER_H
