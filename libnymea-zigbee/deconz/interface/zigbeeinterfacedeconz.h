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

#ifndef ZIGBEEINTERFACEDECONZ_H
#define ZIGBEEINTERFACEDECONZ_H

#include <QObject>
#include <QTimer>
#include <QSerialPort>

class ZigbeeInterfaceDeconz : public QObject
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

    explicit ZigbeeInterfaceDeconz(QObject *parent = nullptr);
    ~ZigbeeInterfaceDeconz();

    bool available() const;
    QString serialPort() const;

private:
    QTimer *m_reconnectTimer = nullptr;
    QSerialPort *m_serialPort = nullptr;
    bool m_available = false;
    QByteArray m_dataBuffer;

    quint16 calculateCrc(const QByteArray &data);
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

#endif // ZIGBEEINTERFACEDECONZ_H
