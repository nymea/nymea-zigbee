#ifndef ZIGBEEDEVICEOBJECT_H
#define ZIGBEEDEVICEOBJECT_H

#include <QObject>

class ZigbeeNetwork;

class ZigbeeDeviceObject : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeDeviceObject(ZigbeeNetwork *network, QObject *parent = nullptr);

private:
    ZigbeeNetwork *m_network = nullptr;

signals:

};

#endif // ZIGBEEDEVICEOBJECT_H
