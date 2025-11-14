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

#include "zigbeeinterfacerequest.h"

ZigbeeInterfaceRequest::ZigbeeInterfaceRequest()
{

}

ZigbeeInterfaceRequest::ZigbeeInterfaceRequest(const ZigbeeInterfaceMessage &message):
    m_message(message)
{

}

void ZigbeeInterfaceRequest::setDescription(const QString &description)
{
    m_description = description;
}

QString ZigbeeInterfaceRequest::description() const
{
    return m_description;
}

void ZigbeeInterfaceRequest::setMessage(const ZigbeeInterfaceMessage &message)
{
    m_message = message;
}

ZigbeeInterfaceMessage ZigbeeInterfaceRequest::message() const
{
    return m_message;
}

bool ZigbeeInterfaceRequest::expectsAdditionalMessage() const
{
    return m_expectsAdditionalMessage;
}

void ZigbeeInterfaceRequest::setExpectedAdditionalMessageType(const Zigbee::InterfaceMessageType &messageType)
{
    m_expectedAdditionalMessageType = messageType;
    m_expectsAdditionalMessage = true;
}

Zigbee::InterfaceMessageType ZigbeeInterfaceRequest::expectedAdditionalMessageType() const
{
    return m_expectedAdditionalMessageType;
}

void ZigbeeInterfaceRequest::setTimoutIntervall(const int &timeoutIntervall)
{
    m_timeoutIntervall = timeoutIntervall;
}

int ZigbeeInterfaceRequest::timeoutIntervall() const
{
    return m_timeoutIntervall;
}
