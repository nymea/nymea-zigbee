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

#ifndef ZIGBEEINTERFACEMESSAGE_H
#define ZIGBEEINTERFACEMESSAGE_H

#include <QObject>
#include <QDebug>

#include "zigbee.h"

class ZigbeeInterfaceMessage
{
public:
    ZigbeeInterfaceMessage();
    ZigbeeInterfaceMessage(const Zigbee::InterfaceMessageType &messageType, const QByteArray &data = QByteArray());

    Zigbee::InterfaceMessageType messageType() const;
    void setMessageType(const Zigbee::InterfaceMessageType &messageType);

    QByteArray data() const;
    void setData(const QByteArray &data);

    bool isValid() const;

private:
    Zigbee::InterfaceMessageType m_messageType = Zigbee::MessageTypeNone;
    QByteArray m_data;

};

QDebug operator<<(QDebug dbg, const ZigbeeInterfaceMessage &message);

#endif // ZIGBEEINTERFACEMESSAGE_H
