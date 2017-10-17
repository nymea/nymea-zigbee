#include "zigbeeinterface.h"

#include <QDebug>

ZigbeeInterface::ZigbeeInterface(QObject *parent) :
    QObject(parent),
    m_serialPort(nullptr),
    m_readingState(WaitForStart)
{

}

bool ZigbeeInterface::available() const
{
    return m_serialPort->isOpen();
}

QString ZigbeeInterface::convertByteToHexString(const quint8 &byte)
{
    QString hexString;
    QString byteString = QString::number(byte, 16);
    if (byteString.count() == 1) {
        hexString = QString("0x0%1").arg(byteString);
    } else {
        hexString = QString("0x%1").arg(byteString);
    }
    return hexString.toStdString().data();
}

QString ZigbeeInterface::convertByteArrayToHexString(const QByteArray &byteArray)
{
    QString hexString;
    for (int i = 0; i < byteArray.count(); i++) {
        hexString.append(convertByteToHexString((quint8)byteArray.at(i)));
        if (i != byteArray.count() -1) {
            hexString.append(" ");
        }
    }
    return hexString.toStdString().data();
}

QString ZigbeeInterface::convertByte16ToHexString(const quint16 &byte)
{
    quint8 msbByte = (byte >> 8) & 0xff;
    quint8 lsbByte = (byte >> 0) & 0xff;

    return convertByteToHexString(msbByte) + convertByteToHexString(lsbByte).remove("0x");
}

quint8 ZigbeeInterface::calculateCrc(const quint16 &commandValue, const quint16 &lenghtValue, const QByteArray &data)
{
    quint8 crc = 0;

    crc ^= (commandValue >> 8) & 0xff;
    crc ^= (commandValue >> 0) & 0xff;

    crc ^= (lenghtValue >> 8) & 0xff;
    crc ^= (lenghtValue >> 0) & 0xff;

    for (int i = 0; i < lenghtValue; i++) {
        crc ^= quint8(data.at(i));
    }

    return crc;
}

void ZigbeeInterface::streamByte(quint8 byte, bool specialCharacter)
{
    if (!specialCharacter && byte < 0x10) {
        // Byte stuffing ESC char before stuffed data byte
        qDebug() << "[out]" << convertByteToHexString(0x02);
        if (m_serialPort->write(QByteArray::fromRawData("\x02", 1)) < 0) {
            qWarning() << "Could not stream ESC byte" << convertByteArrayToHexString(QByteArray::fromRawData("\x02", 1));
        }

        byte ^= 0x10;
    }
    qDebug() << "[out]" << convertByteToHexString(byte);
    if (m_serialPort->write(QByteArray(1, (char)byte)) < 0) {
        qWarning() << "Could not stream byte" << convertByteToHexString(byte);
    }
    m_serialPort->flush();
}

void ZigbeeInterface::setReadingState(const ZigbeeInterface::ReadingState &state)
{
    if (m_readingState == state)
        return;

    m_readingState = state;
    qDebug() << m_readingState;
}

void ZigbeeInterface::onReadyRead()
{
    QByteArray data = m_serialPort->readAll();

    // Parse serial data and built InterfaceMessage
    for (int i = 0; i < data.length(); i++) {
        quint8 byte = static_cast<quint8>(data.at(i));

        qDebug() << "[ in]" << convertByteToHexString(byte);

        switch (byte) {
        case 0x01:
            //qDebug() << "START message received";
            m_crcValue = 0;
            m_commandValue = 0;
            m_lengthValue = 0;
            m_escapeDetected = false;
            m_data.clear();

            setReadingState(WaitForTypeMsb);
            break;
        case 0x02:
            //qDebug() << "ESC char received";
            m_escapeDetected = true;
            break;
        case 0x03: {
            quint8 crc = calculateCrc(m_commandValue, m_lengthValue, m_data);
            if (crc != m_crcValue) {
                qWarning() << "Invalid CRC value" << crc << "!=" << m_crcValue;
            } else if (m_data.count() != m_lengthValue) {
                qWarning() << "ERROR: Invalid data length" << m_data.count() << "!=" << m_lengthValue;
            } else {
                qDebug() << "<--" << (Type)m_commandValue << convertByte16ToHexString(m_commandValue) << "CRC:" << convertByteToHexString(m_crcValue) << convertByte16ToHexString(m_lengthValue) << convertByteArrayToHexString(m_data);
                emit messageReceived((Type)m_commandValue, m_data);
            }
            setReadingState(WaitForStart);
            break;
        }
        default:

            // If the previous byte was an escape character, XOR the byte with 0x10 according documentation
            if (m_escapeDetected) {
                byte ^= 0x10;
                m_escapeDetected = false;
            }

            switch (m_readingState) {
            case WaitForStart:
                break;
            case WaitForTypeMsb:
                m_commandValue = byte;
                m_commandValue <<= 8;
                setReadingState(WaitForTypeLsb);
                break;
            case WaitForTypeLsb:
                m_commandValue |= byte;
                //qDebug() << "Command:" << convertByte16ToHexString(m_commandValue) << (Type)m_commandValue;
                setReadingState(WaitForLenghtMsb);
                break;
            case WaitForLenghtMsb:
                m_lengthValue = byte;
                m_lengthValue <<= 8;
                setReadingState(WaitForLengthLsb);
                break;
            case WaitForLengthLsb:
                m_lengthValue |= byte;
                setReadingState(WaitForCrc);
                break;
            case WaitForCrc:
                m_crcValue = byte;
                setReadingState(WaitForData);
                break;
            case WaitForData:
                m_data.append(static_cast<char>(byte));
                break;
            default:
                break;
            }

            break;
        }
    }
}

void ZigbeeInterface::onError(const QSerialPort::SerialPortError &error)
{
    qWarning() << "Serial port error:" << error << m_serialPort->errorString();
}

bool ZigbeeInterface::enable(const QString &serialPort)
{
    if (m_serialPort) {
        delete m_serialPort;
        m_serialPort = nullptr;
    }

    m_serialPort = new QSerialPort(serialPort, this);
    m_serialPort->setBaudRate(QSerialPort::Baud115200);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    connect(m_serialPort, &QSerialPort::readyRead, this, &ZigbeeInterface::onReadyRead);
    connect(m_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(onError(QSerialPort::SerialPortError)));

    if (!m_serialPort->open(QSerialPort::ReadWrite)) {
        qWarning() << "Could not open serial port" << serialPort;
        delete m_serialPort;
        m_serialPort = nullptr;
        return false;
    }

    qDebug() << "Interface enabled successfully on" << serialPort;
    return true;
}

void ZigbeeInterface::disable()
{
    if (!m_serialPort)
        return;

    if (m_serialPort->isOpen())
        m_serialPort->close();

    delete m_serialPort;
    m_serialPort = nullptr;
    qDebug() << "Interface disabled";
}

void ZigbeeInterface::sendCommand(const ZigbeeInterface::Type &commandType, const QByteArray &data)
{
    quint16 commandValue = static_cast<quint16>(commandType);
    quint16 lengthValue = static_cast<quint16>(data.count());
    quint8 crcValue = calculateCrc(commandValue, lengthValue, data);

    qDebug() << "-->" << commandType << convertByte16ToHexString(commandValue) << "CRC:" << convertByteToHexString(crcValue) << convertByte16ToHexString(lengthValue) << convertByteArrayToHexString(data);

    streamByte(0x01, true);
    streamByte((commandValue >> 8) & 0xff);
    streamByte((commandValue >> 0) & 0xff);
    streamByte((lengthValue >> 8) & 0xff);
    streamByte((lengthValue >> 0) & 0xff);
    streamByte(crcValue);

    for (int i = 0; i < data.count(); i++) {
        streamByte(data.at(i));
    }
    streamByte(0x03, true);
}
