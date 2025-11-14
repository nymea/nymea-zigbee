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

#include "zigbeeinterfacemessage.h"
#include "zigbeeutils.h"

ZigbeeInterfaceMessage::ZigbeeInterfaceMessage()
{

}

ZigbeeInterfaceMessage::ZigbeeInterfaceMessage(const Zigbee::InterfaceMessageType &messageType, const QByteArray &data):
    m_messageType(messageType),
    m_data(data)
{

}

Zigbee::InterfaceMessageType ZigbeeInterfaceMessage::messageType() const
{
    return m_messageType;
}

void ZigbeeInterfaceMessage::setMessageType(const Zigbee::InterfaceMessageType &messageType)
{
    m_messageType = messageType;
}

QByteArray ZigbeeInterfaceMessage::data() const
{
    return m_data;
}

void ZigbeeInterfaceMessage::setData(const QByteArray &data)
{
    m_data = data;
}

bool ZigbeeInterfaceMessage::isValid() const
{
    return m_messageType != Zigbee::MessageTypeNone;
}

QDebug operator<<(QDebug dbg, const ZigbeeInterfaceMessage &message)
{
    dbg.nospace().noquote() << ZigbeeUtils::messageTypeToString(message.messageType()) << "(" << ZigbeeUtils::convertUint16ToHexString(static_cast<quint16>(message.messageType())) << ")" << " | " <<  ZigbeeUtils::convertByteArrayToHexString(message.data());
    return dbg.space();
}
