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

#ifndef ZIGBEEUARTADAPTER_H
#define ZIGBEEUARTADAPTER_H

#include <QString>
#include <QDebug>

#include "zigbee.h"

class ZigbeeUartAdapter
{
public:
    explicit ZigbeeUartAdapter();

    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &description);

    QString serialPort() const;
    void setSerialPort(const QString &serialPort);

    QString serialNumber() const;
    void setSerialNumber(const QString &serialNumber);

    bool hardwareRecognized() const;
    void setHardwareRecognized(bool hardwareRecognized);

    Zigbee::ZigbeeBackendType zigbeeBackend() const;
    void setBackendType(Zigbee::ZigbeeBackendType backendType);

    qint32 baudRate() const;
    void setBaudRate(qint32 baudRate);

private:
    QString m_name;
    QString m_description;
    QString m_serialPort;
    QString m_serialNumber;

    bool m_hardwareRecognized = false;
    Zigbee::ZigbeeBackendType m_zigbeeBackend = Zigbee::ZigbeeBackendTypeDeconz;
    qint32 m_baudRate = 38400;
};

Q_DECLARE_METATYPE(ZigbeeUartAdapter)

QDebug operator<<(QDebug debug, const ZigbeeUartAdapter &adapter);


#endif // ZIGBEEADAPTER_H
