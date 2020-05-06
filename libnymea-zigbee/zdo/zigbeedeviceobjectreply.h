#ifndef ZIGBEEDEVICEOBJECTREPLY_H
#define ZIGBEEDEVICEOBJECTREPLY_H

#include <QObject>

class ZigbeeDeviceObjectReply : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeDeviceObjectReply(QObject *parent = nullptr);

signals:

};

#endif // ZIGBEEDEVICEOBJECTREPLY_H
