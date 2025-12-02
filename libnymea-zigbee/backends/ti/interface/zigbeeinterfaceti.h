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

#ifndef ZIGBEEINTERFACETI_H
#define ZIGBEEINTERFACETI_H

#include <QObject>
#include <QTimer>
#include <QSerialPort>
#include "zigbeeinterfacetireply.h"

#define SOF 0xFE

class ZigbeeInterfaceTi : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeInterfaceTi(QObject *parent = nullptr);
    ~ZigbeeInterfaceTi();

    bool available() const;
    QString serialPort() const;

    void sendMagicByte();
    void setDTR(bool dtr);
    void setRTS(bool rts);

    void sendPacket(Ti::CommandType type, Ti::SubSystem subSystem, quint8 command, const QByteArray &payload);

public slots:
    bool enable(const QString &serialPort = "/dev/ttyS0", qint32 baudrate = 38400);
    void reconnectController();
    void disable();

signals:
    void availableChanged(bool available);
    void packetReceived(Ti::SubSystem subSystem, Ti::CommandType type, quint8 command, const QByteArray &payload);

private slots:
    void onReconnectTimeout();
    void onReadyRead();
    void onError(const QSerialPort::SerialPortError &error);
    void processBuffer();

private:
    QTimer *m_reconnectTimer = nullptr;
    QSerialPort *m_serialPort = nullptr;
    bool m_available = false;
    QByteArray m_dataBuffer;

    quint8 calculateChecksum(const QByteArray &data);

    void setAvailable(bool available);
};

#endif // ZIGBEEINTERFACETI_H
