#ifndef ZIGBEEINTERFACE_H
#define ZIGBEEINTERFACE_H

#include <QObject>
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
    QSerialPort *m_serialPort;
    QByteArray m_messageBuffer;

    // Message parsing
    ReadingState m_readingState;
    quint8 m_crcValue;
    quint8 m_currentValue;
    quint16 m_messageTypeValue;
    quint16 m_lengthValue;
    QByteArray m_data;
    bool m_escapeDetected;

    quint8 calculateCrc(const quint16 &messageTypeValue, const quint16 &lenghtValue, const QByteArray &data);

    void streamByte(quint8 byte, bool specialCharacter = false);

    void setReadingState(const ReadingState & state);

signals:
    void availableChanged(const bool &available);
    void messageReceived(const ZigbeeInterfaceMessage &message);

private slots:
    void onReadyRead();
    void onError(const QSerialPort::SerialPortError &error);

public slots:
    bool enable(const QString &serialPort = "/dev/ttyS0");
    void disable();

    void sendMessage(const ZigbeeInterfaceMessage &message);

};

#endif // ZIGBEEINTERFACE_H
