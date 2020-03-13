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

#include "zigbeeutils.h"
#include "zigbeecluster.h"
#include "loggingcategory.h"

ZigbeeCluster::ZigbeeCluster(Zigbee::ClusterId clusterId, Direction direction, QObject *parent) :
    QObject(parent),
    m_clusterId(clusterId),
    m_direction(direction)
{
    qCDebug(dcZigbeeCluster()) << "Create cluster" << ZigbeeUtils::convertUint16ToHexString(clusterId) << direction;
}

ZigbeeCluster::Direction ZigbeeCluster::direction() const
{
    return m_direction;
}

Zigbee::ClusterId ZigbeeCluster::clusterId() const
{
    return m_clusterId;
}

QString ZigbeeCluster::clusterName() const
{
    return ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(m_clusterId));
}

QList<ZigbeeClusterAttribute> ZigbeeCluster::attributes() const
{
    return m_attributes.values();
}

bool ZigbeeCluster::hasAttribute(quint16 attributeId) const
{
    if (m_attributes.keys().isEmpty())
        return false;

    return m_attributes.keys().contains(attributeId);
}

ZigbeeClusterAttribute ZigbeeCluster::attribute(quint16 id)
{
    return m_attributes.value(id);
}

void ZigbeeCluster::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    if (hasAttribute(attribute.id())) { 
        qCDebug(dcZigbeeCluster()) << this << "update attribute" << attribute;
        m_attributes[attribute.id()] = attribute;
        emit attributeChanged(attribute);
    } else {
        qCDebug(dcZigbeeCluster()) << this << "add attribute" << attribute;
        m_attributes.insert(attribute.id(), attribute);
        emit attributeChanged(attribute);
    }
}

QDebug operator<<(QDebug debug, ZigbeeCluster *cluster)
{
    debug.nospace().noquote() << "ZigbeeCluster("
                              << ZigbeeUtils::convertUint16ToHexString(static_cast<quint16>(cluster->clusterId())) << ", "
                              << cluster->clusterName() << ", "
                              << cluster->direction()
                              << ")";

    return debug.space();
}
