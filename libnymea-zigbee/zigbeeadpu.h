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
