/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
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

#ifndef ZIGBEEINTERFACE_H
#define ZIGBEEINTERFACE_H

#include <QObject>
#include <QTimer>
#include <QSerialPort>

#include "../zigbee.h"
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
