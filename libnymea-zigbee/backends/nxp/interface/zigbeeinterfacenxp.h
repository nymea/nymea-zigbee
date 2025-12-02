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

#ifndef ZIGBEEINTERFACENXP_H
#define ZIGBEEINTERFACENXP_H

#include <QObject>
#include <QTimer>
#include <QSerialPort>

class ZigbeeInterfaceNxp : public QObject
{
    Q_OBJECT

public:
    enum ProtocolByte {
        ProtocolByteEnd = 0xC0,
        ProtocolByteEsc = 0xDB,
        ProtocolByteTransposedEnd = 0xDC,
        ProtocolByteTransposedEsc = 0xDD
    };
    Q_ENUM(ProtocolByte)

    explicit ZigbeeInterfaceNxp(QObject *parent = nullptr);
    ~ZigbeeInterfaceNxp();

    bool available() const;
    QString serialPort() const;

private:
    QTimer *m_reconnectTimer = nullptr;
    QSerialPort *m_serialPort = nullptr;
    bool m_available = false;
    QByteArray m_dataBuffer;

    quint8 calculateCrc(const QByteArray &data);
    QByteArray unescapeData(const QByteArray &data);
    QByteArray escapeData(const QByteArray &data);

    void setAvailable(bool available);

signals:
    void availableChanged(bool available);
    void packageReceived(const QByteArray &package);

private slots:
    void onReconnectTimeout();
    void onReadyRead();
    void onError(const QSerialPort::SerialPortError &error);

public slots:
    void sendPackage(const QByteArray &package);
    bool enable(const QString &serialPort = "/dev/ttyS0", qint32 baudrate = 115200);
    void reconnectController();
    void disable();

};

#endif // ZIGBEEINTERFACENXP_H
