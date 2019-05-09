#ifndef ZIGBEEINTERFACE_H
#define ZIGBEEINTERFACE_H

#include <QObject>
#include <QTimer>
#include <QSerialPort>

#include "zigbee.h"
#include "zigbeeinterfacemessage.h"

class ZigbeeInterface : public QObject
{
    Q_OBJECT
public:
    enum ReadingState {
        WaitForStart,
        WaitForTypeMsb,
        WaitForTypeLsb,
        WaitForLenghtMsb,
        WaitForLengthLsb,
        WaitForCrc,
        WaitForData
    };
    Q_ENUM(ReadingState)

    explicit ZigbeeInterface(QObject *parent = nullptr);
    ~ZigbeeInterface();

    bool available() const;
    QString serialPort() const;

private:
    QTimer *m_reconnectTimer = nullptr;
    QSerialPort *m_serialPort = nullptr;
    QByteArray m_messageBuffer;
    bool m_available = false;

    // Message parsing
    ReadingState m_readingState = WaitForStart;
    quint8 m_crcValue;
    quint8 m_currentValue;
    quint16 m_messageTypeValue;
    quint16 m_lengthValue;
    QByteArray m_data;
    bool m_escapeDetected;

    quint8 calculateCrc(const quint16 &messageTypeValue, const quint16 &lenghtValue, const QByteArray &data);

    void streamByte(quint8 byte, bool specialCharacter = false);
    void setAvailable(bool available);
    void setReadingState(const ReadingState & state);

signals:
    void availableChanged(bool available);
    void messageReceived(const ZigbeeInterfaceMessage &message);

private slots:
    void onReconnectTimeout();
    void onReadyRead();
    void onError(const QSerialPort::SerialPortError &error);

public slots:
    bool enable(const QString &serialPort = "/dev/ttyS0", qint32 baudrate = 115200);
    void disable();

    void sendMessage(const ZigbeeInterfaceMessage &message);

};

#endif // ZIGBEEINTERFACE_H
