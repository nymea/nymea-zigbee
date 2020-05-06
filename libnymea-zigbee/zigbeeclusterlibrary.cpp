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

#include "zigbeeclusterlibrary.h"
#include "zigbeeutils.h"

#include <QDataStream>

quint8 ZigbeeClusterLibrary::buildFrameControlByte(const ZigbeeClusterLibrary::FrameControl &frameControl)
{
    quint8 byte = 0x00;

    // Bit 0-1
    byte |= FrameTypeClusterSpecific;

    // Bit 2
    if (frameControl.manufacturerSpecific)
        byte |= 0x01 << 2;

    // Bit 3
    if (frameControl.direction == DirectionServerToClient)
        byte |= 0x01 << 3;

    // Bit 4
    if (frameControl.disableDefaultResponse)
        byte |= 0x01 << 4;

    return byte;
}

ZigbeeClusterLibrary::FrameControl ZigbeeClusterLibrary::parseFrameControlByte(quint8 frameControlByte)
{
    FrameControl frameControl;

    if (ZigbeeUtils::checkBitUint8(frameControlByte, 0)) {
        frameControl.frameType = FrameTypeClusterSpecific;
    } else {
        frameControl.frameType = FrameTypeGlobal;
    }

    frameControl.manufacturerSpecific = ZigbeeUtils::checkBitUint8(frameControlByte, 2);

    if (ZigbeeUtils::checkBitUint8(frameControlByte, 3)) {
        frameControl.direction = DirectionServerToClient;
    } else {
        frameControl.direction = DirectionClientToServer;
    }

    frameControl.disableDefaultResponse = ZigbeeUtils::checkBitUint8(frameControlByte, 4);

    return frameControl;
}

QByteArray ZigbeeClusterLibrary::buildHeader(const ZigbeeClusterLibrary::Header &header)
{
    QByteArray headerData;
    QDataStream stream(&headerData, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << buildFrameControlByte(header.frameControl);

    // Include manufacturer only if the frame control indicates manufacturer specific
    if (header.frameControl.manufacturerSpecific) {
        stream << header.manufacturerCode;
    }

    stream << header.transactionSequenceNumber;
    stream << static_cast<quint8>(header.command);

    return headerData;
}

ZigbeeClusterLibrary::Frame ZigbeeClusterLibrary::parseFrameData(Zigbee::ClusterId clusterId, const QByteArray &frameData)
{
    QDataStream stream(frameData);
    stream.setByteOrder(QDataStream::LittleEndian);

    // Read the header and then the payload
    quint8 offset = 0;
    quint8 frameControlByte = 0;
    quint8 commandByte = 0;

    Header header;
    stream >> frameControlByte;
    offset += 1;

    header.frameControl = parseFrameControlByte(frameControlByte);
    if (header.frameControl.manufacturerSpecific) {
        stream >> header.manufacturerCode;
        offset += 2;
    }

    stream >> header.transactionSequenceNumber;
    offset += 1;

    stream >> commandByte;
    offset += 1;

    header.command = static_cast<Command>(commandByte);
    offset += 1;

    Frame frame;
    frame.clusterId = clusterId;
    frame.header = header;
    frame.payload = frameData.right(frameData.length() - offset - 1);
    return frame;
}

QByteArray ZigbeeClusterLibrary::buildFrame(const ZigbeeClusterLibrary::Frame &frame)
{
    return buildHeader(frame.header) + frame.payload;
}

QDebug operator<<(QDebug debug, const ZigbeeClusterLibrary::FrameControl &frameControl)
{
    debug.nospace() << "FrameControl(";
    if (frameControl.frameType == ZigbeeClusterLibrary::FrameTypeGlobal) {
        debug.nospace() << "Frame Type: Global" << ", ";
    } else {
        debug.nospace() << "Frame Type: Cluster specific" << ", ";
    }

    debug.nospace() << "Manufacturer specific: " << (frameControl.manufacturerSpecific ? "1" : "0") << ", ";
    debug.nospace() << "Direction: ";
    if (frameControl.direction == ZigbeeClusterLibrary::DirectionClientToServer) {
        debug.nospace() << "Client to server, ";
    } else {
        debug.nospace() << "Server to client, ";
    }

    debug.nospace() << "Disable default response: " << (frameControl.disableDefaultResponse ? "1" : "0") << ")";
    return debug.space();
}

QDebug operator<<(QDebug debug, const ZigbeeClusterLibrary::Header &header)
{
    debug.nospace() << "Header(";
    debug.nospace() << header.frameControl;
    if (header.frameControl.manufacturerSpecific) {
        debug.nospace() << "Manufacturer code: " << ZigbeeUtils::convertUint16ToHexString(header.manufacturerCode) << ", ";
    }
    debug.nospace() << "TSN:" << header.transactionSequenceNumber << ", ";
    debug.nospace() << header.command << ")";
    return debug.space();
}

QDebug operator<<(QDebug debug, const ZigbeeClusterLibrary::Frame &frame)
{
    debug.nospace() << "Zigbee Cluster Library Frame(" << frame.clusterId << ", ";
    debug.nospace() << frame.header;
    debug.nospace() << ZigbeeUtils::convertByteArrayToHexString(frame.payload) << ")";
    return debug.space();
}
