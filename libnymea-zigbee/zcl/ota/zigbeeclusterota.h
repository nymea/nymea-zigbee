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

#ifndef ZIGBEECLUSTEROTA_H
#define ZIGBEECLUSTEROTA_H

#include <QObject>

#include "zcl/zigbeecluster.h"
#include "zcl/zigbeeclusterreply.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterOta : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Attribute {
        AttributeUpgradeServerId = 0x0000,
        AttributeFileOffset = 0x0001,
        AttributeCurrentFileVersion = 0x0002,
        AttributeCurrentZigbeeStackVersion = 0x0003,
        AttributeDownloadedFileVersion = 0x0004,
        AttributeDownloadedZigbeeStackVersion = 0x0005,
        AttributeImageUpgradeStatus = 0x0006,
        AttributeManufacturerId = 0x0007,
        AttributeImageTypeId = 0x0008,
        AttributeMinimumBockPeriod = 0x0009,
        AttributeImageStamp = 0x000a
    };
    Q_ENUM(Attribute)

    enum Command {
        CommandImageNotify = 0x00,
        CommandQueryNextImageRequest = 0x01,
        CommandQueryNextImageResponse = 0x02,
        CommandImageBlockRequest = 0x03,
        CommandImagePageRequest = 0x04,
        CommandImageBlockResponse = 0x05,
        CommandUpgradeEndRequest = 0x06,
        CommandUpgradeEndResponse = 0x07,
        CommandQueryDeviceSpecificFileRequest = 0x08,
        CommandQueryDeviceSpecificFileResponse = 0x09
    };
    Q_ENUM(Command)

    enum StatusCode {
        StatuCodeSuccess = 0x00,
        StatuCodeAbort = 0x95,
        StatuCodeNotAuthorized = 0x7E,
        StatuCodeInvalidImage = 0x96,
        StatuCodeWaitForData = 0x97,
        StatuCodeNoImageAvailable = 0x98,
        StatuCodeMalformedCommand = 0x80,
        StatuCodeUnsupportedClusterCommand = 0x81,
        StatuCodeRequireMoreImage = 0x99
    };
    Q_ENUM(StatusCode)

    typedef struct FileVersion {
        quint8 applicationRelease;
        quint8 applicationBuild;
        quint8 stackRelease;
        quint8 stackBuild;
    } FileVersion;

    explicit ZigbeeClusterOta(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

protected:
    void processDataIndication(ZigbeeClusterLibrary::Frame frame) override;

private:
    FileVersion parseFileVersion(quint32 fileVersionValue);
};

QDebug operator<<(QDebug debug, const ZigbeeClusterOta::FileVersion &fileVersion);


#endif // ZIGBEECLUSTEROTA_H
