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

#include "zigbeenetworkmanager.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include "nxp/zigbeenetworknxp.h"

#include <QDateTime>
#include <QDataStream>
#include <QSettings>

ZigbeeNetworkManager::ZigbeeNetworkManager(const QSerialPortInfo &serialPortInfo, QSerialPort::BaudRate baudrate, BackendType backendType, QObject *parent) :
    QObject(parent),
    m_serialPortInfo(serialPortInfo),
    m_baudrate(baudrate),
    m_backendType(backendType)
{
    switch (backendType) {
    case BackendTypeNxp:
        m_network = new ZigbeeNetworkNxp(this);
        m_network->setSerialPortName(m_serialPortInfo.systemLocation());
        m_network->setSerialBaudrate(static_cast<qint32>(baudrate));
        break;
    }
}

QSerialPortInfo ZigbeeNetworkManager::serialPortInfo() const
{
    return m_serialPortInfo;
}

QSerialPort::BaudRate ZigbeeNetworkManager::baudrate() const
{
    return m_baudrate;
}

ZigbeeNetworkManager::BackendType ZigbeeNetworkManager::backendType() const
{
    return m_backendType;
}

ZigbeeNetwork *ZigbeeNetworkManager::network() const
{
    return m_network;
}
