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

#include "zigbeeclusterota.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeClusterOta::ZigbeeClusterOta(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdOtaUpgrade, direction, parent)
{

}

ZigbeeClusterReply *ZigbeeClusterOta::sendImageNotify(PayloadType payloadType, quint8 queryJitter, quint16 manufacturerCode, quint16 imageType, quint32 newFileVersion)
{
    if (queryJitter > 100) {
        queryJitter = 100;
    }
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(payloadType);
    stream << queryJitter;
    if (payloadType >= PayloadTypeQueryJitterAndManufacturerCode) {
        stream << manufacturerCode;
    }
    if (payloadType >= PayloadTypeQueryJitterAndManufacturerCodeAndImageType) {
        stream << imageType;
    }
    if (payloadType >= PayloadTypeQueryJitterAndManufacturerCodeAndImageTypeAndNewFileVersion) {
        stream << newFileVersion;
    }
    ZigbeeClusterReply *reply = executeClusterCommand(CommandImageNotify, payload, ZigbeeClusterLibrary::DirectionServerToClient);
    return reply;
}

ZigbeeClusterReply *ZigbeeClusterOta::sendQueryNextImageResponse(quint8 transactionSequenceNumber, StatusCode statusCode, quint16 manufacturerCode, quint16 imageType, quint32 fileVersion, quint32 imageSize)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(statusCode);
    if (statusCode == StatusCodeSuccess) {
        stream << manufacturerCode;
        stream << imageType;
        stream << fileVersion;
        stream << imageSize;
    }

    ZigbeeClusterReply *reply = sendClusterServerResponse(CommandQueryNextImageResponse, transactionSequenceNumber, payload);
    connect(reply, &ZigbeeClusterReply::finished, this, [reply](){
        if (reply->error() != ZigbeeClusterReply::ErrorNoError) {
            qCWarning(dcZigbeeCluster()) << "OTA: Error sending query next image response:" << reply->error();
            return;
        }
        qCDebug(dcZigbeeCluster()) << "OTA: Query image response sent successfully.";
    });
    return reply;
}

ZigbeeClusterReply *ZigbeeClusterOta::sendImageBlockResponse(quint8 transactionSequenceNumber, quint16 manufacturerCode, quint16 imageType, quint32 fileVersion, quint32 fileOffset, const QByteArray &imageData)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(StatusCodeSuccess);
    stream << manufacturerCode;
    stream << imageType;
    stream << fileVersion;
    stream << fileOffset;
    stream << static_cast<quint8>(imageData.length());
    stream.writeRawData(imageData.data(), imageData.length());

    ZigbeeClusterReply *reply = sendClusterServerResponse(CommandImageBlockResponse, transactionSequenceNumber, payload);
    connect(reply, &ZigbeeClusterReply::finished, this, [reply](){
        if (reply->error() != ZigbeeClusterReply::ErrorNoError) {
            qCWarning(dcZigbeeCluster()) << "OTA: Error sending image block response:" << reply->error();
            return;
        }
        qCDebug(dcZigbeeCluster()) << "OTA: Image block response sent successfully.";
    });
    return reply;
}

ZigbeeClusterReply *ZigbeeClusterOta::sendAbortImageBlockResponse(quint8 transactionSequenceNumber)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(StatusCodeAbort);
    ZigbeeClusterReply *reply = sendClusterServerResponse(CommandImageBlockResponse, transactionSequenceNumber, payload);
    connect(reply, &ZigbeeClusterReply::finished, this, [reply](){
        if (reply->error() != ZigbeeClusterReply::ErrorNoError) {
            qCWarning(dcZigbeeCluster()) << "OTA: Error sending abort image block response:" << reply->error();
            return;
        }
        qCDebug(dcZigbeeCluster()) << "OTA: Abort image block response sent successfully.";
    });
    return reply;

}

ZigbeeClusterReply *ZigbeeClusterOta::sendDelayImageBlockResponse(quint8 transactionSequenceNumber, const QDateTime &requestTime, quint16 minimumBlockPeriod)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(StatusCodeWaitForData);
    stream << static_cast<quint32>(requestTime.toMSecsSinceEpoch() / 1000);
    stream << minimumBlockPeriod;

    ZigbeeClusterReply *reply = sendClusterServerResponse(CommandImageBlockResponse, transactionSequenceNumber, payload);
    connect(reply, &ZigbeeClusterReply::finished, this, [reply](){
        if (reply->error() != ZigbeeClusterReply::ErrorNoError) {
            qCWarning(dcZigbeeCluster()) << "OTA: Error sending delay image block response:" << reply->error();
            return;
        }
        qCDebug(dcZigbeeCluster()) << "OTA: Delay image block response sent successfully.";
    });
    return reply;

}

ZigbeeClusterReply *ZigbeeClusterOta::sendUpgradeEndResponse(quint8 transactionSequenceNumber, quint16 manufacturerCode, quint16 imageType, quint32 fileVersion, quint32 serverTime, quint32 requestTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << manufacturerCode;
    stream << imageType;
    stream << fileVersion;
    stream << serverTime;
    stream << requestTime;

    ZigbeeClusterReply *reply = sendClusterServerResponse(CommandUpgradeEndResponse, transactionSequenceNumber, payload);
    connect(reply, &ZigbeeClusterReply::finished, this, [reply](){
        if (reply->error() != ZigbeeClusterReply::ErrorNoError) {
            qCWarning(dcZigbeeCluster()) << "OTA: Error sending image block response:" << reply->error();
            return;
        }
        qCDebug(dcZigbeeCluster()) << "OTA: Query image block data successfully.";
    });
    return reply;
}

ZigbeeClusterReply *ZigbeeClusterOta::sendAbortUpgradeEndResponse(quint8 transactionSequenceNumber)
{
    return sendDefaultResponse(transactionSequenceNumber, ZigbeeClusterLibrary::CommandDefaultResponse, ZigbeeClusterLibrary::StatusSuccess);
}

ZigbeeClusterOta::FileVersion ZigbeeClusterOta::parseFileVersion(quint32 fileVersionValue)
{
    FileVersion fileVersion;
    fileVersion.applicationRelease = static_cast<quint8>(fileVersionValue >> 24);
    fileVersion.applicationBuild = static_cast<quint8>(fileVersionValue >> 16);
    fileVersion.stackRelease = static_cast<quint8>(fileVersionValue >> 8);
    fileVersion.stackBuild = static_cast<quint8>(fileVersionValue);
    return fileVersion;
}

void ZigbeeClusterOta::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    qCDebug(dcZigbeeCluster()) << "OTA: Processing cluster frame" << m_node << m_endpoint << this << frame << frame.payload.toHex();

    switch (m_direction) {
    case Client:
        if (frame.header.frameControl.direction == ZigbeeClusterLibrary::DirectionClientToServer) {
            Command command = static_cast<Command>(frame.header.command);
            qCDebug(dcZigbeeCluster()) << "Received" << command << "from" << m_node << m_endpoint << this;
            switch (command) {
            case CommandQueryNextImageRequest: {
                // Print the image information
                quint8 fieldControl;
                quint16 manufacturerCode;
                quint16 imageType;
                quint32 currentVersion;
                quint16 hardwareVersion;

                QDataStream requestStream(&frame.payload, QIODevice::ReadOnly);
                requestStream.setByteOrder(QDataStream::LittleEndian);
                requestStream >> fieldControl >> manufacturerCode >> imageType >> currentVersion >> hardwareVersion;
                FileVersion currentFileVersion = parseFileVersion(currentVersion);
                qCDebug(dcZigbeeCluster()) << "OTA image request:" << (fieldControl == 0x00 ? "Hardware version not present" : "Hardware version present");
                qCDebug(dcZigbeeCluster()) << "OTA image request: Manufacturer code" << ZigbeeUtils::convertUint16ToHexString(manufacturerCode);
                qCDebug(dcZigbeeCluster()) << "OTA image request: Image type" << ZigbeeUtils::convertUint16ToHexString(imageType);
                qCDebug(dcZigbeeCluster()) << "OTA image request: Current file version" << ZigbeeUtils::convertUint32ToHexString(currentVersion) << currentFileVersion;
                qCDebug(dcZigbeeCluster()) << "OTA image request: Hardware version" << hardwareVersion;

                emit queryNextImageRequestReceived(frame.header.transactionSequenceNumber, manufacturerCode, imageType, currentVersion, hardwareVersion);
                break;
            }
            case CommandImageBlockRequest: {
                quint8 fieldControl;
                quint16 manufacturerCode;
                quint16 imageType;
                quint32 fileVersion;
                quint32 fileOffset;
                quint8 maximumDataSize;
                quint64 requestNodeAddress = 0;
                quint16 minimumBlockPerdiod = 0;

                QDataStream stream(frame.payload);
                stream.setByteOrder(QDataStream::LittleEndian);
                stream >> fieldControl >> manufacturerCode >> imageType >> fileVersion >> fileOffset >> maximumDataSize;
                if (fieldControl & 0x01) {
                    stream >> requestNodeAddress;
                }
                if (fieldControl & 0x02) {
                    stream >> minimumBlockPerdiod;
                }
                qCDebug(dcZigbeeCluster()) << "OTA: Image block request receved. FieldControl:" << fieldControl << "ManufacturerCode:" << manufacturerCode << "ImageType:" << imageType << "File version:" << fileVersion << "Offset:" << fileOffset << "Max size:" << maximumDataSize << "Request Address:" << ZigbeeAddress(requestNodeAddress) << "Min block period:" << minimumBlockPerdiod;
                emit imageBlockRequestReceived(frame.header.transactionSequenceNumber, manufacturerCode, imageType, fileVersion, fileOffset, maximumDataSize, ZigbeeAddress(requestNodeAddress), minimumBlockPerdiod);

                break;
            }
            case CommandUpgradeEndRequest: {
                quint8 status;
                quint16 manufacturerCode;
                quint16 imageType;
                quint32 fileVersion;
                QDataStream stream(frame.payload);
                stream.setByteOrder(QDataStream::LittleEndian);
                stream >> status >> manufacturerCode >> imageType >> fileVersion;
                emit upgradeEndRequestReceived(frame.header.transactionSequenceNumber, static_cast<StatusCode>(status), manufacturerCode, imageType, fileVersion);
                break;
            }
            default:
                qCWarning(dcZigbeeCluster()) << "Received unhandled command" << command << "from" << m_node << m_endpoint << this;
                break;
            }
        }
        break;
    case Server:
        if (frame.header.frameControl.direction == ZigbeeClusterLibrary::DirectionServerToClient) {
            Command command = static_cast<Command>(frame.header.command);
            qCDebug(dcZigbeeCluster()) << "Received" << command << "from" << m_node << m_endpoint << this;
        } else {
            qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
        }
        break;
    }
}


QDebug operator<<(QDebug debug, const ZigbeeClusterOta::FileVersion &fileVersion)
{
    debug.nospace().noquote() << "FileVersion(Application Release: " << fileVersion.applicationRelease
                              << ", Application Build: " << fileVersion.applicationBuild
                              << ", Stack Release: " << fileVersion.stackRelease
                              << ", Stack Build: " << fileVersion.stackBuild << ")";
    return debug.space();
}
