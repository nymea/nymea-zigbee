// SPDX-License-Identifier: LGPL-3.0-or-later

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* nymea-zigbee
* Zigbee integration module for nymea
*
* Copyright (C) 2013 - 2024, nymea GmbH
* Copyright (C) 2024 - 2025, chargebyte austria GmbH
*
* This file is part of nymea-zigbee.
*
* nymea-zigbee is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3
* of the License, or (at your option) any later version.
*
* nymea-zigbee is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with nymea-zigbee. If not, see <https://www.gnu.org/licenses/>.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "zigbeeinterfaceti.h"
#include "zigbee.h"
#include "zigbeeutils.h"
#include "loggingcategory.h"

#include <QDataStream>

ZigbeeInterfaceTi::ZigbeeInterfaceTi(QObject *parent) : QObject(parent)
{
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);
    m_reconnectTimer->setInterval(5000);

    connect(m_reconnectTimer, &QTimer::timeout, this, &ZigbeeInterfaceTi::onReconnectTimeout);
}

ZigbeeInterfaceTi::~ZigbeeInterfaceTi()
{

}

bool ZigbeeInterfaceTi::available() const
{
    return m_available;
}

QString ZigbeeInterfaceTi::serialPort() const
{
    return m_serialPort->portName();
}

void ZigbeeInterfaceTi::sendMagicByte()
{
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream << static_cast<quint8>(0xef);
    m_serialPort->write(message);
}

void ZigbeeInterfaceTi::setDTR(bool dtr)
{
    m_serialPort->setDataTerminalReady(dtr);
}

void ZigbeeInterfaceTi::setRTS(bool rts)
{
    m_serialPort->setRequestToSend(rts);
}

quint8 ZigbeeInterfaceTi::calculateChecksum(const QByteArray &data)
{
    quint8 checksum = 0;
    for (int i = 0; i < data.length(); i++) {
        checksum ^= static_cast<quint8>(data.at(i));
    }
    return checksum;
}

void ZigbeeInterfaceTi::setAvailable(bool available)
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

void ZigbeeInterfaceTi::onReconnectTimeout()
{
    qCDebug(dcZigbeeInterface()) << "Reconnecting to serial port...";
    if (m_serialPort && !m_serialPort->isOpen()) {
        if (!m_serialPort->open(QSerialPort::ReadWrite)) {
            setAvailable(false);
            qCDebug(dcZigbeeInterface()) << "Interface reconnected failed" << m_serialPort->portName() << m_serialPort->baudRate();
            m_reconnectTimer->start();
        } else {
            qCDebug(dcZigbeeInterface()) << "Interface reconnected successfully on" << m_serialPort->portName() << m_serialPort->baudRate();
            m_serialPort->clear();
            setAvailable(true);
        }
    }
}

void ZigbeeInterfaceTi::onReadyRead()
{
    m_dataBuffer.append(m_serialPort->readAll());
    processBuffer();
}

void ZigbeeInterfaceTi::processBuffer()
{
    if (m_dataBuffer.isEmpty()) {
        return;
    }

    qCDebug(dcZigbeeInterfaceTraffic()) << "<--" << m_dataBuffer.toHex();
    // StartOfFrame
    if (static_cast<quint8>(m_dataBuffer.at(0)) != SOF) {
        qCWarning(dcZigbeeInterface()) << "Data doesn't start with StartOfFrame byte 0xfe. Discarding data...";
        m_dataBuffer.remove(0, 1);
        processBuffer();
        return;
    }

    // payload length
    quint8 payloadLength = static_cast<quint8>(m_dataBuffer[1]);
    // Packet must be SOF + payload length field + CMD0 + CMD1 + payload length + Checksum
    if (m_dataBuffer.length() < payloadLength + 5) {
        qCDebug(dcZigbeeInterface()) << "Not enough data in buffer....";
        return;
    }
    QByteArray packet = m_dataBuffer.left(5 + payloadLength);
    m_dataBuffer.remove(0, 5 + payloadLength);

    quint8 cmd0 = static_cast<quint8>(packet[2]);
    quint8 cmd1 = static_cast<quint8>(packet[3]);
    QByteArray payload = packet.mid(4, payloadLength);
    quint8 checksum = packet.at(4 + payloadLength);

    if (calculateChecksum(packet.mid(1, 3 + payloadLength)) != checksum) {
        qCWarning(dcZigbeeInterface()) << "Checksum mismatch!";
        processBuffer();
        return;
    }
//    qCDebug(dcZigbeeInterface()) << "packet received:" << payloadLength << cmd0 << command << payload.toHex(' ') << checksum;

    Ti::SubSystem subSystem = static_cast<Ti::SubSystem>(cmd0 & 0x1F);
    Ti::CommandType type = static_cast<Ti::CommandType>(cmd0 & 0xE0);

    emit packetReceived(subSystem, type, cmd1, payload);

    // In case there's more...
    processBuffer();

}

void ZigbeeInterfaceTi::onError(const QSerialPort::SerialPortError &error)
{
    if (error != QSerialPort::NoError && m_serialPort->isOpen()) {
        qCWarning(dcZigbeeInterface()) << "Serial port error:" << error << m_serialPort->errorString();
        reconnectController();
    }
}

void ZigbeeInterfaceTi::sendPacket(Ti::CommandType type, Ti::SubSystem subSystem, quint8 command, const QByteArray &payload)
{
    if (!m_available) {
        qCWarning(dcZigbeeInterface()) << "Can not send data. The interface is not available";
        return;
    }

    quint8 cmd0 = type | subSystem;

    // Build transport data
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << static_cast<quint8>(SOF);
    stream << static_cast<quint8>(payload.length());
    stream << cmd0;
    stream << static_cast<quint8>(command);
    for (int i = 0; i < payload.length(); i++) {
        stream << static_cast<quint8>(payload.at(i));
    }
    stream << calculateChecksum(data.right(data.length() - 1));

    // Send the data
    qCDebug(dcZigbeeInterfaceTraffic()) << "-->" << data.toHex();
    if (m_serialPort->write(data) < 0) {
        qCWarning(dcZigbeeInterface()) << "Could not stream byte" << ZigbeeUtils::convertByteArrayToHexString(data);
    }

    //m_serialPort->flush();
}

bool ZigbeeInterfaceTi::enable(const QString &serialPort, qint32 baudrate)
{
    qCDebug(dcZigbeeInterface()) << "Starting UART interface " << serialPort << baudrate;

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

    connect(m_serialPort, &QSerialPort::readyRead, this, &ZigbeeInterfaceTi::onReadyRead);
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &ZigbeeInterfaceTi::onError);
#else
    typedef void (QSerialPort::* errorSignal)(QSerialPort::SerialPortError);
    connect(m_serialPort, static_cast<errorSignal>(&QSerialPort::error), this, &ZigbeeInterfaceTi::onError);
#endif

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

void ZigbeeInterfaceTi::reconnectController()
{
    if (!m_serialPort)
        return;

    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }

    QString portName = m_serialPort->portName();
    int baudrate = m_serialPort->baudRate();
    setAvailable(false);

    delete m_serialPort;
    m_serialPort = nullptr;

    enable(portName, baudrate);
}

void ZigbeeInterfaceTi::disable()
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
