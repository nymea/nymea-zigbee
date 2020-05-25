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
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QDataStream>

quint8 ZigbeeClusterLibrary::buildFrameControlByte(const ZigbeeClusterLibrary::FrameControl &frameControl)
{
    quint8 byte = 0x00;

    // Bit 0-1
    byte |= frameControl.frameType;

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

QList<ZigbeeClusterLibrary::ReadAttributeStatusRecord> ZigbeeClusterLibrary::parseAttributeStatusRecords(const QByteArray &payload)
{
    // Read attribute status records
    QList<ReadAttributeStatusRecord> attributeStatusRecords;

    qCDebug(dcZigbeeClusterLibrary()) << "Parse attribute status records from" << ZigbeeUtils::convertByteArrayToHexString(payload);

    QDataStream stream(payload);
    stream.setByteOrder(QDataStream::LittleEndian);
    quint16 attributeId; quint8 statusInt; quint8 dataTypeInt; quint16 numberOfElenemts; quint8 elementType;
    QByteArray data;

    while (!stream.atEnd()) {
        // Reset variables
        attributeId = 0; statusInt = 0; dataTypeInt = 0; numberOfElenemts = 0; elementType = 0;
        data.clear();

        // Read attribute id and status
        stream >> attributeId >> statusInt;
        Zigbee::ZigbeeStatus status = static_cast<Zigbee::ZigbeeStatus>(statusInt);

        qCDebug(dcZigbeeClusterLibrary()) << "Parse:" << ZigbeeUtils::convertUint16ToHexString(attributeId) << status;

        if (status != Zigbee::ZigbeeStatusSuccess) {
            qCWarning(dcZigbeeCluster()) << "Attribute status record" << ZigbeeUtils::convertUint16ToHexString(attributeId) << "finished with error" << status;
            // If not success, we are done and can continue with the next status record
            continue;
        } else {
            stream >> dataTypeInt;
            Zigbee::DataType dataType = static_cast<Zigbee::DataType>(dataTypeInt);

            qCDebug(dcZigbeeClusterLibrary()) << "Parse:" << dataType;

            // Parse data depending on the type
            if (dataType == Zigbee::Array || dataType == Zigbee::Set || dataType == Zigbee::Bag) {
                stream >> elementType >> numberOfElenemts;
                qCDebug(dcZigbeeClusterLibrary()) << "Parse (array, set, bag): Element type" << ZigbeeUtils::convertByteToHexString(elementType) << "Number of elements:" << numberOfElenemts;
                if (numberOfElenemts == 0xffff) {
                    qCWarning(dcZigbeeCluster()) << "ZigbeeStatusRecord contains invalid elements" << ZigbeeUtils::convertUint16ToHexString(attributeId) << dataType;
                    continue;
                } else {
                    for (int i = 0; i < numberOfElenemts; i++) {
                        quint8 element = 0;
                        stream >> element;
                        data.append(element);
                    }
                }
            } else if (dataType == Zigbee::Structure) {
                stream >> numberOfElenemts;
                qCDebug(dcZigbeeClusterLibrary()) << "Parse (structure)" << "Number of elements:" << numberOfElenemts;
                if (numberOfElenemts == 0xffff) {
                    qCWarning(dcZigbeeCluster()) << "ZigbeeStatusRecord contains invalid elements" << ZigbeeUtils::convertUint16ToHexString(attributeId) << dataType;
                    continue;
                } else {
                    stream >> elementType;
                    qCDebug(dcZigbeeClusterLibrary()) << "Parse (structure)" << "Element type:" << ZigbeeUtils::convertByteToHexString(elementType);
                    for (int i = 0; i < numberOfElenemts; i++) {
                        quint8 element = 0;
                        stream >> element;
                        //qCDebug(dcZigbeeClusterLibrary()) << "Parse (structure)" << "Element value:" << ZigbeeUtils::convertByteToHexString(element);
                        data.append(element);
                    }
                }
            } else {
                // Normal data type
                quint8 length = 0;
                stream >> length;
                qCDebug(dcZigbeeClusterLibrary()) << "Parse (normal data type)" << "Number of elements:" << length;
                for (int i = 0; i < length; i++) {
                    quint8 element = 0;
                    stream >> element;
                    data.append(element);
                }
            }

            ReadAttributeStatusRecord attributeRecord;
            attributeRecord.attributeId = attributeId;
            attributeRecord.attributeStatus = status;
            attributeRecord.dataType = dataType;
            attributeRecord.data = data;
            qCDebug(dcZigbeeClusterLibrary()) << attributeRecord;
            attributeStatusRecords.append(attributeRecord);
        }
    }

    return attributeStatusRecords;
}

ZigbeeClusterLibrary::Frame ZigbeeClusterLibrary::parseFrameData(const QByteArray &frameData)
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
    header.command = static_cast<Command>(commandByte);
    offset += 1;

    Frame frame;
    frame.header = header;
    frame.payload = frameData.right(frameData.length() - offset);
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
    debug.nospace() << "Frame(";
    debug.nospace() << frame.header;
    debug.nospace() << ZigbeeUtils::convertByteArrayToHexString(frame.payload) << ")";
    return debug.space();
}

QDebug operator<<(QDebug debug, const ZigbeeClusterLibrary::ReadAttributeStatusRecord &attributeStatusRecord)
{
    debug.nospace().noquote() << "ReadAttributeStatusRecord("
                              << ZigbeeUtils::convertUint16ToHexString(attributeStatusRecord.attributeId) << ", "
                              << attributeStatusRecord.attributeStatus << ", "
                              << attributeStatusRecord.dataType << ", "
                              << attributeStatusRecord.data
                              << ")";

    return debug.space();
}
