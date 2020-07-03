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

#include "zigbeeinterfacedeconz.h"
#include "zigbee.h"
#include "zigbeeutils.h"
#include "loggingcategory.h"

#include <QDataStream>

// SLIP: https://tools.ietf.org/html/rfc1055

ZigbeeInterfaceDeconz::ZigbeeInterfaceDeconz(QObject *parent) : QObject(parent)
{
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);
    m_reconnectTimer->setInterval(5000);

    connect(m_reconnectTimer, &QTimer::timeout, this, &ZigbeeInterfaceDeconz::onReconnectTimeout);
}

ZigbeeInterfaceDeconz::~ZigbeeInterfaceDeconz()
{

}

quint16 ZigbeeInterfaceDeconz::calculateCrc(const QByteArray &data)
{
    quint16 crc = 0;
    for (int i = 0; i < data.length(); i++) {
        crc += static_cast<quint8>(data.at(i));
    }
    quint8 crc0 = (~crc + 1) & 0xFF;
    quint8 crc1 = ((~crc + 1) >> 8) & 0xFF;
    return (static_cast<quint16>(crc1 << 8) | static_cast<quint16>(crc0));
}

QByteArray ZigbeeInterfaceDeconz::unescapeData(const QByteArray &data)
{
    QByteArray deserializedData;
    // Parse serial data and built InterfaceMessage
    bool escaped = false;
    for (int i = 0; i < data.length(); i++) {
        quint8 byte = static_cast<quint8>(data.at(i));

        if (escaped) {
            if (byte == ProtocolByteTransposedEnd) {
                deserializedData.append(static_cast<char>(ProtocolByteEnd));
            } else if (byte == ProtocolByteTransposedEsc) {
                deserializedData.append(static_cast<char>(ProtocolByteEsc));
            } else {
                qCWarning(dcZigbeeInterfaceTraffic()) << "Error while deserialing data. Escape character received but the escaped character was not recognized.";
                return QByteArray();
            }

            escaped = false;
            continue;
        }

        // If escape byte, the next byte has to be a modified byte
        if (byte == ProtocolByteEsc) {
            escaped = true;
        } else {
            deserializedData.append(static_cast<char>(byte));
        }
    }

    return deserializedData;
}

QByteArray ZigbeeInterfaceDeconz::escapeData(const QByteArray &data)
{
    QByteArray serializedData;
    QDataStream stream(&serializedData, QIODevice::WriteOnly);

    for (int i = 0; i < data.length(); i++) {
        quint8 byte = static_cast<quint8>(data.at(i));
        switch (byte) {
        case ProtocolByteEnd:
            stream << static_cast<quint8>(ProtocolByteEsc);
            stream << static_cast<quint8>(ProtocolByteTransposedEnd);
            break;
        case ProtocolByteEsc:
            stream << static_cast<quint8>(ProtocolByteEsc);
            stream << static_cast<quint8>(ProtocolByteTransposedEsc);
            break;
        default:
            stream << byte;
            break;
        }
    }

    return serializedData;
}

void ZigbeeInterfaceDeconz::setAvailable(bool available)
{
    if (m_available == available)
        return;

    // Clear the data buffer in any case
    if (m_available) {
        m_dataBuffer.clear();
    }

    m_available = available;
    emit availableChanged(m_available);
}

void ZigbeeInterfaceDeconz::onReconnectTimeout()
{
    if (m_serialPort && !m_serialPort->isOpen()) {
        if (!m_serialPort->open(QSerialPort::ReadWrite)) {
            setAvailable(false);
            m_reconnectTimer->start();
        } else {
            qCDebug(dcZigbeeInterface()) << "Interface reconnected successfully on" << m_serialPort->portName() << m_serialPort->baudRate();
            m_serialPort->clear();
            setAvailable(true);
        }
    }
}

void ZigbeeInterfaceDeconz::onReadyRead()
{
    QByteArray data = m_serialPort->readAll();

    // Read each byte until we get END byte, then unescape the package
    for (int i = 0; i < data.length(); i++) {
        quint8 byte = static_cast<quint8>(data.at(i));
        if (byte == ProtocolByteEnd) {
            // If there is no data...continue since it might be a starting END byte
            if (m_dataBuffer.isEmpty())
                continue;

            qCDebug(dcZigbeeInterfaceTraffic()) << "<--" << ZigbeeUtils::convertByteArrayToHexString(m_dataBuffer);
            QByteArray frame = unescapeData(m_dataBuffer);
            if (frame.isNull()) {
                qCWarning(dcZigbeeInterface()) << "Received inconsistant message. Ignoring data" << ZigbeeUtils::convertByteArrayToHexString(m_dataBuffer);
            } else {
                QByteArray package = frame.left(frame.length() - 2);
                QByteArray checksumBytes = frame.right(2);
                QDataStream stream(&checksumBytes, QIODevice::ReadOnly);
                stream.setByteOrder(QDataStream::LittleEndian);
                quint16 receivedChecksum = 0;
                stream >> receivedChecksum;
                quint16 calculatedChecksum = calculateCrc(package);
                if (receivedChecksum != calculatedChecksum) {
                    qCWarning(dcZigbeeInterfaceTraffic()) << "Checksum verification failed for frame" << ZigbeeUtils::convertByteArrayToHexString(m_dataBuffer) << receivedChecksum << "!=" << calculatedChecksum;
                    m_dataBuffer.clear();
                    continue;
                }

                // Checksum verified, we got valid data
                qCDebug(dcZigbeeInterface()) << "Received frame" << ZigbeeUtils::convertByteArrayToHexString(frame);
                emit packageReceived(package);
            }
            m_dataBuffer.clear();
        } else {
            m_dataBuffer.append(data.at(i));
        }
    }
}

void ZigbeeInterfaceDeconz::onError(const QSerialPort::SerialPortError &error)
{
    if (error != QSerialPort::NoError && m_serialPort->isOpen()) {
        qCWarning(dcZigbeeInterface()) << "Serial port error:" << error << m_serialPort->errorString();
        m_reconnectTimer->start();
        m_serialPort->close();
        setAvailable(false);
    }
}

void ZigbeeInterfaceDeconz::sendPackage(const QByteArray &package)
{
    if (!m_available) {
        qCWarning(dcZigbeeInterface()) << "Can not send data. The interface is not available";
        return;
    }

    // Build the frame and escape the package data and crc
    quint16 checksum = calculateCrc(package);

    QByteArray frame = package;
    QDataStream frameStream(&frame, QIODevice::WriteOnly  | QIODevice::Append);
    frameStream.setByteOrder(QDataStream::LittleEndian);
    frameStream << checksum;

    qCDebug(dcZigbeeInterface()) << "Send frame" << ZigbeeUtils::convertByteArrayToHexString(frame);

    // Escape data according to SLIP for transfere
    QByteArray serializedData = escapeData(frame);

    // Build transport data
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << static_cast<quint8>(ProtocolByteEnd); // Start with SLIP END character
    for (int i = 0; i < serializedData.length(); i++)
        stream << static_cast<quint8>(serializedData.at(i));

    stream << static_cast<quint8>(ProtocolByteEnd); // End with SLIP END character

    // Send the data
    qCDebug(dcZigbeeInterfaceTraffic()) << "-->" << ZigbeeUtils::convertByteArrayToHexString(data);
    if (m_serialPort->write(data) < 0) {
        qCWarning(dcZigbeeInterface()) << "Could not stream byte" << ZigbeeUtils::convertByteArrayToHexString(data);
    }

    //m_serialPort->flush();
}

bool ZigbeeInterfaceDeconz::enable(const QString &serialPort, qint32 baudrate)
{
    qCDebug(dcZigbeeInterface()) << "Start UART interface " << serialPort << baudrate;

    if (m_serialPort) {
        delete m_serialPort;
        m_serialPort = nullptr;
    }

    m_serialPort = new QSerialPort(serialPort, this);
    m_serialPort->setBaudRate(baudrate);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    connect(m_serialPort, &QSerialPort::readyRead, this, &ZigbeeInterfaceDeconz::onReadyRead);
    connect(m_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(onError(QSerialPort::SerialPortError)));

    if (!m_serialPort->open(QSerialPort::ReadWrite)) {
        qCWarning(dcZigbeeInterface()) << "Could not open serial port" << serialPort << baudrate << m_serialPort->errorString();
        m_reconnectTimer->start();
        return false;
    }

    qCDebug(dcZigbeeInterface()) << "Interface enabled successfully on" << serialPort << baudrate;
    m_serialPort->clear();

    setAvailable(true);
    return true;
}

void ZigbeeInterfaceDeconz::reconnectController()
{
    if (!m_serialPort)
        return;

    if (m_serialPort->isOpen())
        m_serialPort->close();

    delete m_serialPort;
    m_serialPort = nullptr;
    setAvailable(false);
    m_reconnectTimer->start();
}

void ZigbeeInterfaceDeconz::disable()
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
