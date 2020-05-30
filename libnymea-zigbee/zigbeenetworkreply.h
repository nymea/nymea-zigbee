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

#ifndef ZIGBEENETWORKREPLY_H
#define ZIGBEENETWORKREPLY_H

#include <QObject>

#include "zigbee.h"
#include "zigbeenetworkrequest.h"

class ZigbeeNetworkReply : public QObject
{
    Q_OBJECT

    friend class ZigbeeNetwork;
    friend class ZigbeeNodeEndpoint;

public:
    enum Error {
        ErrorNoError,
        ErrorZigbeeApsStatusError,
        ErrorZigbeeNwkStatusError,
        ErrorInterfaceError,
        ErrorNetworkOffline
    };
    Q_ENUM(Error)

    Error error() const;

    ZigbeeNetworkRequest request() const;
    Zigbee::ZigbeeApsStatus zigbeeApsStatus() const;
    Zigbee::ZigbeeNwkLayerStatus zigbeeNwkStatus() const;

private:
    explicit ZigbeeNetworkReply(const ZigbeeNetworkRequest &request, QObject *parent = nullptr);
    ZigbeeNetworkRequest m_request;

    Error m_error = ErrorNoError;
    Zigbee::ZigbeeApsStatus m_zigbeeApsStatus = Zigbee::ZigbeeApsStatusSuccess;
    Zigbee::ZigbeeNwkLayerStatus m_zigbeeNwkStatus = Zigbee::ZigbeeNwkLayerStatusSuccess;

signals:
    void finished();

};

#endif // ZIGBEENETWORKREPLY_H
