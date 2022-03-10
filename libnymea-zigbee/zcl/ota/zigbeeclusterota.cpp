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

ZigbeeClusterOta::FileVersion ZigbeeClusterOta::parseFileVersion(quint32 fileVersionValue)
{
    FileVersion fileVersion;
    fileVersion.applicationRelease = static_cast<quint8>(fileVersionValue & 0xFF000000);
    fileVersion.applicationBuild = static_cast<quint8>(fileVersionValue & 0x00FF0000);
    fileVersion.stackRelease = static_cast<quint8>(fileVersionValue & 0x0000FF00);
    fileVersion.stackBuild = static_cast<quint8>(fileVersionValue & 0x000000FF);
    return fileVersion;
}

void ZigbeeClusterOta::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    qCDebug(dcZigbeeCluster()) << "Processing cluster frame" << m_node << m_endpoint << this << frame;

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
                qCDebug(dcZigbeeCluster()) << "OTA image request:" << (fieldControl == 0x0000 ? "Hardware version not present" : "Hardware version present");
                qCDebug(dcZigbeeCluster()) << "OTA image request: Manufacturer code" << ZigbeeUtils::convertUint16ToHexString(manufacturerCode);
                qCDebug(dcZigbeeCluster()) << "OTA image request: Image type" << ZigbeeUtils::convertUint16ToHexString(imageType);
                qCDebug(dcZigbeeCluster()) << "OTA image request: Current file version" << ZigbeeUtils::convertUint32ToHexString(currentVersion) << parseFileVersion(currentVersion);
                qCDebug(dcZigbeeCluster()) << "OTA image request: Hardware version" << hardwareVersion;

                // Respond with no image available until we implement the entire cluster for OTA updates
                qCDebug(dcZigbeeCluster()) << "OTA mechanism not implemented yet. Tell the node there is no image available.";

                QByteArray payload;
                QDataStream stream(&payload, QIODevice::WriteOnly);
                stream.setByteOrder(QDataStream::LittleEndian);
                stream << static_cast<quint8>(StatuCodeNoImageAvailable);

                // Note: if there would be an image available, the response would be success, followed by manufacturer code, image type, file version of image and file size

                ZigbeeClusterReply *reply = sendClusterServerResponse(CommandQueryNextImageResponse, frame.header.transactionSequenceNumber, payload);
                connect(reply, &ZigbeeClusterReply::finished, this, [](){
                    qCDebug(dcZigbeeCluster()) << "OTA image request response for image query sent successfully to requested node.";
                });

                break;
            }
            default:
                qCWarning(dcZigbeeCluster()) << "Received command" << command << "which is not implemented yet from" << m_node << m_endpoint << this;
                break;
            }
        }
        break;
    case Server:
        qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
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
