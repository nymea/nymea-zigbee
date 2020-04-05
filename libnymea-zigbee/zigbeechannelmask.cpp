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

#include "zigbeechannelmask.h"
#include "zigbeeutils.h"

ZigbeeChannelMask::ZigbeeChannelMask()
{

}

ZigbeeChannelMask::ZigbeeChannelMask(quint32 channelMask) :
    m_channelMask(channelMask)
{

}

ZigbeeChannelMask::ZigbeeChannelMask(Zigbee::ZigbeeChannels channels)
{
    m_channelMask = static_cast<quint32>(channels);
}

quint32 ZigbeeChannelMask::toUInt32() const
{
    return m_channelMask;
}

Zigbee::ZigbeeChannels ZigbeeChannelMask::channels() const
{
    return static_cast<Zigbee::ZigbeeChannels>(m_channelMask);
}

bool ZigbeeChannelMask::isSet(Zigbee::ZigbeeChannel channel) const
{
    return channels().testFlag(channel);
}

void ZigbeeChannelMask::setChannel(Zigbee::ZigbeeChannel channel)
{
    // Set channel bit
    m_channelMask |= 1 << channel;
}

void ZigbeeChannelMask::unsetChannel(Zigbee::ZigbeeChannel channel)
{
    // Clear channel bit
    m_channelMask  &= ~(1 << channel);
}

ZigbeeChannelMask &ZigbeeChannelMask::operator=(const ZigbeeChannelMask &other)
{
    m_channelMask = other.toUInt32();
    return *this;
}

bool ZigbeeChannelMask::operator==(const ZigbeeChannelMask &other) const
{
    return m_channelMask == other.toUInt32();
}

bool ZigbeeChannelMask::operator!=(const ZigbeeChannelMask &other) const
{
    return !operator==(other);
}

QDebug operator<<(QDebug debug, const ZigbeeChannelMask &channelMaks)
{
    debug.nospace() << "ChannelMask(" << ZigbeeUtils::convertUint32ToHexString(channelMaks.toUInt32());
    debug.nospace() << ", [";
    for (int i = 11; i <= 25; i++) {
        if (channelMaks.isSet(static_cast<Zigbee::ZigbeeChannel>(i))) {
            if (i < 25) {
                debug.nospace() << i << ", ";
            } else {
                debug.nospace() << i;
            }
        }
    }

    debug.nospace() << "])";
    return debug.space();
}
