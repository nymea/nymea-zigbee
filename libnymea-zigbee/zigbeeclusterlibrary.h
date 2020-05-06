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

#ifndef ZIGBEECLUSTERLIBRARY_H
#define ZIGBEECLUSTERLIBRARY_H

#include <QObject>
#include <QDebug>

#include "zigbee.h"

class ZigbeeClusterLibrary
{
    Q_GADGET
public:
    /* General ZCL commans */
    enum Command {
        CommandReadAttributes = 0x00,
        CommandReadAttributesResponse = 0x01,
        CommandWriteAttributes = 0x02,
        CommandWriteAttributesUndivided = 0x03,
        CommandWriteAttributesResponse = 0x04,
        CommandWriteAttributesNoResponse = 0x05,
        CommandConfigureReporting = 0x06,
        CommandConfigureReportingResponse = 0x07,
        CommandReadReportingConfiguration = 0x08,
        CommandReadReportingConfigurationResponse = 0x09,
        CommandReportAttributes = 0x0a,
        CommandDefaultResponse = 0x0b,
        CommandDiscoverAttributes = 0x0c,
        CommandDiscoverAttributesResponse = 0x0d,
        CommandReadAttributesStructured = 0x0e,
        CommandWriteAttributesStructured = 0x0f,
        CommandWriteAttributesStructuredResponse = 0x10,
        CommandDiscoverCommandsReceived = 0x11,
        CommandDiscoverCommandsReceivedResponse = 0x12,
        CommandDiscoverCommandsGenerated = 0x13,
        CommandDiscoverCommandsGeneratedResponse = 0x14,
        CommandDiscoverAttributesExtended = 0x15,
        CommandDiscoverAttributesExtendedResponse = 0x16
    };
    Q_ENUM(Command)

    // Frame control field
    enum FrameType {
        FrameTypeGlobal = 0x00,
        FrameTypeClusterSpecific = 0x01
    };
    Q_ENUM(FrameType)

    enum Direction {
        DirectionClientToServer = 0x00,
        DirectionServerToClient = 0x01
    };
    Q_ENUM(Direction)

    typedef struct FrameControl {
        FrameType frameType = FrameTypeClusterSpecific;
        bool manufacturerSpecific = false;
        Direction direction = DirectionClientToServer;
        bool disableDefaultResponse = false;
    } FrameControl;

    typedef struct Header {
        FrameControl frameControl;
        quint16 manufacturerCode = 0;
        quint8 transactionSequenceNumber = 0;
        Command command;
    } ZclHeader;

    typedef struct Frame {
        Zigbee::ClusterId clusterId;
        Header header;
        QByteArray payload;
    } Frame;

    // General parse/build methods
    static quint8 buildFrameControlByte(const FrameControl &frameControl);
    static FrameControl parseFrameControlByte(quint8 frameControlByte);

    static QByteArray buildHeader(const Header &header);

    static Frame parseFrameData(Zigbee::ClusterId clusterId, const QByteArray &frameData);
    static QByteArray buildFrame(const Frame &frame);
};

QDebug operator<<(QDebug debug, const ZigbeeClusterLibrary::FrameControl &frameControl);
QDebug operator<<(QDebug debug, const ZigbeeClusterLibrary::Header &header);
QDebug operator<<(QDebug debug, const ZigbeeClusterLibrary::Frame &frame);


#endif // ZIGBEECLUSTERLIBRARY_H
