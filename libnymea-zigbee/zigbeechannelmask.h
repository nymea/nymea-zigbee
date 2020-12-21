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

    ZigbeeChannelMask &operator=(const ZigbeeChannelMask &other);
    bool operator==(const ZigbeeChannelMask &other) const;
    bool operator!=(const ZigbeeChannelMask &other) const;

private:
    quint32 m_channelMask = 0;

};

QDebug operator<<(QDebug debug, const ZigbeeChannelMask &channelMaks);

#endif // ZIGBEECHANNELMASK_H
