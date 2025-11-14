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

#ifndef ZIGBEECHANNELMASK_H
#define ZIGBEECHANNELMASK_H

#include <QDebug>
#include <QObject>

#include "zigbee.h"

class ZigbeeChannelMask
{
    Q_GADGET

public:
    enum ChannelConfiguration {
        ChannelConfigurationNoChannel = 0x00000000,
        ChannelConfigurationPrimaryLightLink = 0x02108800,
        ChannelConfigurationAllChannels = 0x07fff800
    };
    Q_ENUM(ChannelConfiguration)

    ZigbeeChannelMask();
    ZigbeeChannelMask(const ZigbeeChannelMask &other);
    ZigbeeChannelMask(quint32 channelMask);
    ZigbeeChannelMask(Zigbee::ZigbeeChannels channels);

    quint32 toUInt32() const;

    Zigbee::ZigbeeChannels channels() const;

    QList<int> channelArray() const;

    bool isSet(Zigbee::ZigbeeChannel channel) const;
    void setChannel(Zigbee::ZigbeeChannel channel);
    void unsetChannel(Zigbee::ZigbeeChannel channel);
    void clear();

    ZigbeeChannelMask &operator=(const ZigbeeChannelMask &other);
    bool operator==(const ZigbeeChannelMask &other) const;
    bool operator!=(const ZigbeeChannelMask &other) const;

private:
    quint32 m_channelMask = 0;

};

QDebug operator<<(QDebug debug, const ZigbeeChannelMask &channelMaks);

#endif // ZIGBEECHANNELMASK_H
