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

#include "zigbeeuartadapter.h"

ZigbeeUartAdapter::ZigbeeUartAdapter()
{

}

QString ZigbeeUartAdapter::name() const
{
    return m_name;
}

void ZigbeeUartAdapter::setName(const QString &name)
{
    m_name = name;
}

QString ZigbeeUartAdapter::description() const
{
    return m_description;
}

void ZigbeeUartAdapter::setDescription(const QString &description)
{
    m_description = description;
}

QString ZigbeeUartAdapter::serialPort() const
{
    return m_serialPort;
}

void ZigbeeUartAdapter::setSerialPort(const QString &serialPort)
{
    m_serialPort = serialPort;
}

QString ZigbeeUartAdapter::serialNumber() const
{
    return m_serialNumber;
}

void ZigbeeUartAdapter::setSerialNumber(const QString &serialNumber)
{
    m_serialNumber = serialNumber;
}

bool ZigbeeUartAdapter::hardwareRecognized() const
{
    return m_hardwareRecognized;
}

void ZigbeeUartAdapter::setHardwareRecognized(bool hardwareRecognized)
{
    m_hardwareRecognized = hardwareRecognized;
}

Zigbee::ZigbeeBackendType ZigbeeUartAdapter::zigbeeBackend() const
{
    return m_zigbeeBackend;
}

void ZigbeeUartAdapter::setBackendType(Zigbee::ZigbeeBackendType backendType)
{
    m_zigbeeBackend = backendType;
}

qint32 ZigbeeUartAdapter::baudRate() const
{
    return m_baudRate;
}

void ZigbeeUartAdapter::setBaudRate(qint32 baudRate)
{
    m_baudRate = baudRate;
}

QDebug operator<<(QDebug debug, const ZigbeeUartAdapter &adapter)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "ZigbeeUartAdapter(" << adapter.name() << " - " << adapter.description();
    debug.nospace() << ", " << adapter.serialPort();
    if (!adapter.serialNumber().isEmpty()) {
        debug.nospace() << ", " << adapter.serialNumber();
    }
    if (adapter.hardwareRecognized()) {
        debug.nospace() << ", Suggested backend: " << adapter.zigbeeBackend();
        debug.nospace() << ", " << adapter.baudRate();
    }

    debug.nospace() << ")";
    return debug;
}
