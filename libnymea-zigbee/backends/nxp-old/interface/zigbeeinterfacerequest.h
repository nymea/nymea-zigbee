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

#ifndef ZIGBEEINTERFACEREQUEST_H
#define ZIGBEEINTERFACEREQUEST_H

#include "zigbeeinterfacemessage.h"

class ZigbeeInterfaceRequest
{
public:
    ZigbeeInterfaceRequest();
    ZigbeeInterfaceRequest(const ZigbeeInterfaceMessage &message);

    void setDescription(const QString &description);
    QString description() const;

    void setMessage(const ZigbeeInterfaceMessage &message);
    ZigbeeInterfaceMessage message() const;

    bool expectsAdditionalMessage() const;

    void setExpectedAdditionalMessageType(const Zigbee::InterfaceMessageType &messageType);
    Zigbee::InterfaceMessageType expectedAdditionalMessageType() const;

    void setTimoutIntervall(const int &timeoutIntervall);
    int timeoutIntervall() const;

private:
    QString m_description;
    ZigbeeInterfaceMessage m_message;
    Zigbee::InterfaceMessageType m_expectedAdditionalMessageType = Zigbee::MessageTypeNone;
    bool m_expectsAdditionalMessage = false;
    int m_timeoutIntervall = 500;

};

#endif // ZIGBEEINTERFACEREQUEST_H
