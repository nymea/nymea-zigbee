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

#include "zigbeeadpu.h"

ZigbeeAdpu::ZigbeeAdpu(QObject *parent) : QObject(parent)
{

}

ZigbeeAdpu::FrameControl ZigbeeAdpu::frameControl() const
{
    return m_frameControl;
}

void ZigbeeAdpu::setFrameControl(ZigbeeAdpu::FrameControl frameControl)
{
    m_frameControl = frameControl;
}

quint8 ZigbeeAdpu::buildFrameControlByte(FrameControl frameControl)
{
    quint8 frameControlByte = 0;
    frameControlByte |= static_cast<quint8>(frameControl.frameType); // Bit 0 - 1
    frameControlByte |= static_cast<quint8>(frameControl.deliveryMode); // Bit 2 - 3
    if (frameControl.apsAckFormat)
        frameControlByte |= static_cast<quint8>(0x10); // Bit 4

    if (frameControl.security)
        frameControlByte |= static_cast<quint8>(0x20); // Bit 5

    if (frameControl.acknowledgementRequest)
        frameControlByte |= static_cast<quint8>(0x40); // Bit 6

    if (frameControl.extendedHeader)
        frameControlByte |= static_cast<quint8>(0x80); // Bit 7

    return frameControlByte;
}

ZigbeeAdpu::FrameControl ZigbeeAdpu::readFrameControlByte(quint8 frameControlByte)
{
    FrameControl frameControl;
    frameControl.frameType = static_cast<FrameType>(frameControlByte | 0x03);
    frameControl.deliveryMode = static_cast<DeliveryMode>(frameControlByte | 0x0C);
    frameControl.apsAckFormat = frameControlByte | 0x10;
    frameControl.security = frameControlByte | 0x20;
    frameControl.acknowledgementRequest = frameControlByte | 0x40;
    frameControl.extendedHeader = frameControlByte | 0x80;
    return frameControl;
}
