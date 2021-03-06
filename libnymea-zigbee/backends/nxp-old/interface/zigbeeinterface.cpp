/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea-zigbee.
* This project including source code and documentation is protected by copyright law, and
* remains the property of nymea GmbH. All rights, including reproduction, publication,
* editing and translation, are reserved. The use of this project is subject to the terms of a
* license agreement to be concluded with nymea GmbH in accordance with the terms
* of use of nymea GmbH, available under https://nymea.io/license
*
* GNU Lesser General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the terms of the GNU
* Lesser General Public License as published by the Free Software Foundation; version 3.
* this project is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License along with this project.
* If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under contact@nymea.io
* or see our FAQ/Licensing Information on https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "zigbeeinterface.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

ZigbeeInterface::ZigbeeInterface(QObject *parent) :
    QObject(parent)
{
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);
    m_reconnectTimer->setInterval(5000);

    connect(m_reconnectTimer, &QTimer::timeout, this, &ZigbeeInterface::onReconnectTimeout);
}

ZigbeeInterface::~ZigbeeInterface()
{
    qCDebug(dcZigbeeInterface()) << "Destroy interface";
    disable();
}

bool ZigbeeInterface::available() const
{
    if (!m_serialPort)
        return false;

    return m_serialPort->isOpen();
}

QString ZigbeeInterface::serialPort() const
{
    return m_serialPort->portName();
}

quint8 ZigbeeInterface::calculateCrc(const quint16 &messageTypeValue, const quint16 &lenghtValue, const QByteArray &data)
{
    quint8 crc = 0;

    crc ^= (messageTypeValue >> 8) & 0xff;
    crc ^= (messageTypeValue >> 0) & 0xff;

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
        qCDebug(dcZigbeeInterfaceTraffic()) << "[out]" << ZigbeeUtils::convertByteToHexString(0x02);
        if (m_serialPort->write(QByteArray::fromRawData("\x02", 1)) < 0) {
            qCWarning(dcZigbeeInterface()) << "Could not stream ESC byte" << ZigbeeUtils::convertByteArrayToHexString(QByteArray::fromRawData("\x02", 1));
        }

        byte ^= 0x10;
    }

    qCDebug(dcZigbeeInterfaceTraffic()) << "[out]" << ZigbeeUtils::convertByteToHexString(byte);
    if (m_serialPort->write(QByteArray(1, static_cast<char>(byte))) < 0) {
        qCWarning(dcZigbeeInterface()) << "Could not stream byte" << ZigbeeUtils::convertByteToHexString(byte);
    }

    m_serialPort->flush();
}

void ZigbeeInterface::setAvailable(bool available)
{
    if (m_available == available)
        return;

    m_available = available;
    emit availableChanged(m_available);
}

void ZigbeeInterface::setReadingState(const ZigbeeInterface::ReadingState &state)
{
    if (m_readingState == state)
        return;

    m_readingState = state;
    qCDebug(dcZigbeeInterfaceTraffic()) << m_readingState;
}

void ZigbeeInterface::onReconnectTimeout()
{
    if (m_serialPort && !m_serialPort->isOpen()) {
        if (!m_serialPort->open(QSerialPort::ReadWrite)) {
            setAvailable(false);
            m_reconnectTimer->start();
        } else {
            qCDebug(dcZigbeeInterface()) << "The controller showed up again.";
            setAvailable(true);
        }
    }
}

void ZigbeeInterface::onReadyRead()
{
    QByteArray data = m_serialPort->readAll();

    // Parse serial data and built InterfaceMessage
    for (int i = 0; i < data.length(); i++) {
        quint8 byte = static_cast<quint8>(data.at(i));

        qCDebug(dcZigbeeInterfaceTraffic()) << "[ in]" << ZigbeeUtils::convertByteToHexString(byte);

        switch (byte) {
        case 0x01:
            m_crcValue = 0;
            m_messageTypeValue = 0;
            m_lengthValue = 0;
            m_escapeDetected = false;
            m_data.clear();
            if (!m_unhandledBuffer.isEmpty()) {
                qCDebug(dcZigbeeInterfaceTraffic()) << "Controller debug message:" << QString::fromUtf8(m_unhandledBuffer);
                m_unhandledBuffer.clear();
            }
            setReadingState(WaitForTypeMsb);
            break;
        case 0x02:
            m_escapeDetected = true;
            break;
        case 0x03: {
            Zigbee::InterfaceMessageType messageType = static_cast<Zigbee::InterfaceMessageType>(m_messageTypeValue);

            // Check message sanity
            quint8 crc = calculateCrc(m_messageTypeValue, m_lengthValue, m_data);
            if (crc != m_crcValue) {
                qCWarning(dcZigbeeInterface()) << "Received message end: Invalid CRC value" << crc << "!=" << m_crcValue;
            } else if (m_data.count() != m_lengthValue) {
                qCWarning(dcZigbeeInterface()) << "Received message end: Invalid data length of message" << m_data.count() << "!=" << m_lengthValue;
            } else {
                // We got a valid message
                ZigbeeInterfaceMessage message(messageType, m_data);
                qCDebug(dcZigbeeInterface()) << "<--" << message << "|" << "crc:" << ZigbeeUtils::convertByteToHexString(m_crcValue) << ", length:" << ZigbeeUtils::convertUint16ToHexString(m_lengthValue);
                emit messageReceived(message);

                // Clear all information for the next byte
                m_crcValue = 0;
                m_messageTypeValue = 0;
                m_lengthValue = 0;
                m_escapeDetected = false;
                m_data.clear();
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

            // Read data bytes depending on the reading state
            switch (m_readingState) {
            case WaitForStart:
                qCDebug(dcZigbeeInterfaceTraffic()) << "Wait for start but reviced data:" << byte;
                m_unhandledBuffer.append(static_cast<char>(byte));
                break;
            case WaitForTypeMsb:
                m_messageTypeValue = byte;
                m_messageTypeValue <<= 8;
                setReadingState(WaitForTypeLsb);
                break;
            case WaitForTypeLsb:
                m_messageTypeValue |= byte;
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
            }

            break;
        }
    }
}

void ZigbeeInterface::onError(const QSerialPort::SerialPortError &error)
{
    if (error != QSerialPort::NoError && m_serialPort->isOpen()) {
        qCWarning(dcZigbeeInterface()) << "Serial port error:" << error << m_serialPort->errorString();
        m_reconnectTimer->start();
        m_serialPort->close();
        setAvailable(false);
    }
}

bool ZigbeeInterface::enable(const QString &serialPort, qint32 baudrate)
{
    if (m_serialPort) {
        delete m_serialPort;
        m_serialPort = nullptr;
    }

    setAvailable(false);

    m_serialPort = new QSerialPort(serialPort, this);
    m_serialPort->setBaudRate(baudrate);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    connect(m_serialPort, &QSerialPort::readyRead, this, &ZigbeeInterface::onReadyRead);
    connect(m_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(onError(QSerialPort::SerialPortError)));

    if (!m_serialPort->open(QSerialPort::ReadWrite)) {
        qCWarning(dcZigbeeInterface()) << "Could not open serial port" << serialPort << baudrate;
        m_reconnectTimer->start();
        return false;
    }

    qCDebug(dcZigbeeInterface()) << "Interface enabled successfully on" << serialPort;
    setAvailable(true);
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
    setAvailable(false);
    qCDebug(dcZigbeeInterface()) << "Interface disabled";
}

void ZigbeeInterface::sendMessage(const ZigbeeInterfaceMessage &message)
{
    if (!available()) {
        qCWarning(dcZigbeeInterface()) << "Could not send message. The serial port is not available.";
        return;
    }

    quint16 messageTypeValue = static_cast<quint16>(message.messageType());
    quint16 lengthValue = static_cast<quint16>(message.data().count());
    quint8 crcValue = calculateCrc(messageTypeValue, lengthValue, message.data());

    qCDebug(dcZigbeeInterface()) << "-->" << message << "|" << "crc:" << ZigbeeUtils::convertByteToHexString(crcValue) << ", length:" << ZigbeeUtils::convertUint16ToHexString(lengthValue);

    streamByte(0x01, true);
    streamByte((messageTypeValue >> 8) & 0xff);
    streamByte((messageTypeValue >> 0) & 0xff);
    streamByte((lengthValue >> 8) & 0xff);
    streamByte((lengthValue >> 0) & 0xff);
    streamByte(crcValue);

    for (int i = 0; i < message.data().count(); i++) {
        streamByte(static_cast<quint8>(message.data().at(i)));
    }
    streamByte(0x03, true);
}
