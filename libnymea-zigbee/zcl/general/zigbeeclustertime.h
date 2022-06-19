/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2022, nymea GmbH
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

#ifndef ZIGBEECLUSTERTIME_H
#define ZIGBEECLUSTERTIME_H

#include "zcl/zigbeecluster.h"
#include "zcl/zigbeeclusterreply.h"

#include <QObject>
#include <QDateTime>

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterTime: public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Attribute {
        AttributeTime = 0x0000,
        AttributeTimeStatus = 0x0001,
        AttributeTimeZone = 0x0002,
        AttributeDstStart = 0x003,
        AttributeDstEnd = 0x004,
        AttributeDstShift = 0x005,
        AttributeStandardTime = 0x006,
        AttributeLocalTime = 0x007,
        AttributeLastSetTime = 0x008,
        AttributeValidUnitTime = 0x009
    };
    Q_ENUM(Attribute)

    enum TimeStatus {
        TimeStatusNone = 0x00,
        TimeStatusMaster = 0x08,
        TimeStatusSynchronized = 0x04,
        TimeStatusMasterZoneDst = 0x02,
        TimeStatusSuperseding = 0x01
    };
    Q_DECLARE_FLAGS(TimeStatusFlags, TimeStatus)
    Q_FLAG(TimeStatusFlags)

    explicit ZigbeeClusterTime(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    QDateTime time() const;
    ZigbeeClusterReply *setTime(const QDateTime &time);

    TimeStatusFlags timeStatus() const;
    ZigbeeClusterReply *setTimeStatus(TimeStatusFlags timeStatus);

signals:
    void timeChanged(const QDateTime &time);
    void timeStatusChanged(TimeStatusFlags timeStatusFlags);

protected:
    void processDataIndication(ZigbeeClusterLibrary::Frame frame) override;

private:
    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

    QDateTime m_time;
    TimeStatusFlags m_timeStatus = TimeStatusNone;
};

#endif // ZIGBEECLUSTERTIME_H
