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

#include "zigbeeclustertime.h"

#include "zigbeeclusteronoff.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"
#include "zigbeeutils.h"


ZigbeeClusterTime::ZigbeeClusterTime(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent):
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdTime, direction, parent)
{

}

QDateTime ZigbeeClusterTime::time() const
{
    return m_time;
}

ZigbeeClusterReply *ZigbeeClusterTime::setTime(const QDateTime &time)
{
    ZigbeeDataType dataType(static_cast<quint32>(time.toMSecsSinceEpoch() / 1000));
    ZigbeeClusterLibrary::WriteAttributeRecord attribute;
    attribute.attributeId = ZigbeeClusterTime::AttributeTime;
    attribute.dataType = dataType.dataType();
    attribute.data = dataType.data();
    return writeAttributes({attribute});
}

ZigbeeClusterTime::TimeStatusFlags ZigbeeClusterTime::timeStatus() const
{
    return m_timeStatus;
}

ZigbeeClusterReply *ZigbeeClusterTime::setTimeStatus(TimeStatusFlags timeStatus)
{
    ZigbeeDataType dataType(static_cast<quint8>(timeStatus));
    ZigbeeClusterLibrary::WriteAttributeRecord attribute;
    attribute.attributeId = ZigbeeClusterTime::AttributeTimeStatus;
    attribute.dataType = dataType.dataType();
    attribute.data = dataType.data();
    return writeAttributes({attribute});
}

void ZigbeeClusterTime::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    switch (attribute.id()) {
    case AttributeTime: {
        qulonglong secsSinceEpoc = attribute.dataType().toUInt32();
        m_time = QDateTime::fromMSecsSinceEpoch(secsSinceEpoc * 1000);
        emit timeChanged(m_time);
        break;
    }
    case AttributeTimeStatus:
        m_timeStatus = static_cast<TimeStatusFlags>(attribute.dataType().toUInt8());
        emit timeStatusChanged(m_timeStatus);
        break;
    }
}

void ZigbeeClusterTime::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    // The time cluster has no specced commands. Silence the warning if out of spec commands come in here
    // by overrideing the method and printing just a debug instead.
    qCDebug(dcZigbeeCluster()) << "Time: Ignoring out of spec ZCL frame:" << m_node << m_endpoint << this << frame;
}
