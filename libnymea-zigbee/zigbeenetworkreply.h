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

    friend class ZigbeeNodeEndpoint;

public:
    enum Error {
        ErrorNoError,
        ErrorZigbeeStatusError,
        ErrorNetworkOffline,
        ErrorUnknown
    };
    Q_ENUM(Error)

    Error error() const;
    ZigbeeNetworkRequest request() const;
    Zigbee::ZigbeeStatus zigbeeStatus() const;

private:
    explicit ZigbeeNetworkReply(const ZigbeeNetworkRequest &request, QObject *parent = nullptr);
    ZigbeeNetworkRequest m_request;

    bool m_finished = false;
    Error m_error = ErrorNoError;
    Zigbee::ZigbeeStatus m_zigbeeStatus = Zigbee::ZigbeeStatusSuccess;

signals:
    void finished();

};

#endif // ZIGBEENETWORKREPLY_H
