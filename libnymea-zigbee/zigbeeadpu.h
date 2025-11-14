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

#ifndef ZIGBEEADPU_H
#define ZIGBEEADPU_H

#include <QObject>


class ZigbeeAdpu : public QObject
{
    Q_OBJECT
public:
    // Note: zigbee Pro Specification 2.2.5.1 General APDU Frame Format

    /* Frame control */
    enum FrameType {
        FrameTypeData = 0x00,
        FrameTypeCommand = 0x01,
        FrameTypeAck = 0x02,
        FrameTypeInterPanAps = 0x03
    };
    Q_ENUM(FrameType)

    enum DeliveryMode {
        DeliveryModeNormalUnicast = 0x00,
        DeliveryModeBroadcast = 0x08,
        DeliveryModeGroupAddressing = 0x0C,
    };
    Q_ENUM(DeliveryMode)


    typedef struct FrameControl {
        FrameType frameType = FrameTypeData;
        DeliveryMode deliveryMode = DeliveryModeNormalUnicast;
        bool security = false;
        bool apsAckFormat = false;
        bool acknowledgementRequest = true;
        bool extendedHeader = false;
    } FrameControl;

    explicit ZigbeeAdpu(QObject *parent = nullptr);

    FrameControl frameControl() const;
    void setFrameControl(FrameControl frameControl);

    quint8 destinationEndpoint() const;
    void setDestinationEndpoint(quint8 destinationEndpoint);

    quint16 groupAddress() const;
    void setGroupAddress(quint16 groupAddress);

    quint16 clusterId() const;
    void setClusterId(quint16 clusterId);

private:
    FrameControl m_frameControl;
    quint8 m_destinationEndpoint;

    quint8 buildFrameControlByte(FrameControl frameControl);
    FrameControl readFrameControlByte(quint8 frameControlByte);

};

#endif // ZIGBEEADPU_H
