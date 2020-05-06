#ifndef ZIGBEEDEVICEPROFILEHANDLER_H
#define ZIGBEEDEVICEPROFILEHANDLER_H

#include <QObject>

class ZigbeeDeviceProfileHandler : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeDeviceProfileHandler(QObject *parent = nullptr);

signals:

};

#endif // ZIGBEEDEVICEPROFILEHANDLER_H
