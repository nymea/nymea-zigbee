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
