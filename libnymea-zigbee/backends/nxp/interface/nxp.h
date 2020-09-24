#ifndef NXP_H
#define NXP_H

#include <QObject>

class Nxp
{
    Q_GADGET
public:
    enum Command {
        CommandGetVersion = 0x00,
        CommandGetDeviceState = 0x01
    };
    Q_ENUM(Command)

    enum Notification {
        NotificationDeviceStatusChanged = 0x7D,
        NotificationDebugMessage = 0xFE
    };
    Q_ENUM(Notification)

    enum Status {
        StatusSuccess = 0x00,
        StatusProtocolError = 0x01,
        StatusUnknownCommand = 0x02,
        StatusInvalidCrc = 0x03
    };
    Q_ENUM(Status)

};

#endif // NXP_H
