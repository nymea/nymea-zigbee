/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2022, nymea GmbH
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
