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

#ifndef ZIGBEEMANAGER_H
#define ZIGBEEMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "zigbeenetwork.h"


class ZigbeeNetworkManager : public QObject
{
    Q_OBJECT

public:
    enum BackendType {
        BackendTypeNxp
    };
    Q_ENUM(BackendType)

    explicit ZigbeeNetworkManager(const QString &serialPortName, qint32 baudrate, BackendType backendType, QObject *parent = nullptr);

    QString serialPortName() const;
    qint32 baudrate() const;
    BackendType backendType() const;

    ZigbeeNetwork *network() const;

private:
    QString m_serialPortName;
    qint32 m_baudrate;
    BackendType m_backendType = BackendTypeNxp;
    ZigbeeNetwork *m_network = nullptr;

private slots:

};

#endif // ZIGBEEMANAGER_H
