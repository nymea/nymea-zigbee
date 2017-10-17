#ifndef ZIGBEEMANAGER_H
#define ZIGBEEMANAGER_H

#include <QObject>

#include "zigbeeinterface.h"

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

signals:

public slots:

};

#endif // ZIGBEEMANAGER_H
