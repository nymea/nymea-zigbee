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

#include "zigbeeclusteridentify.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"

#include <QDataStream>

ZigbeeClusterIdentify::ZigbeeClusterIdentify(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, ZigbeeCluster::Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, Zigbee::ClusterIdIdentify, direction, parent)
{

}

ZigbeeClusterReply *ZigbeeClusterIdentify::identify(quint16 seconds)
{
    // Note: the identify time unit is 0.5 seconds
    QByteArray payload = ZigbeeDataType(seconds * 2).data();
    return executeClusterCommand(ZigbeeClusterIdentify::CommandIdentify, payload);
}

ZigbeeClusterReply *ZigbeeClusterIdentify::identifyQuery()
{
    return executeClusterCommand(ZigbeeClusterIdentify::CommandIdentifyQuery);
}

ZigbeeClusterReply *ZigbeeClusterIdentify::triggerEffect(ZigbeeClusterIdentify::Effect effect, quint8 effectVariant)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(effect);
    stream << static_cast<quint8>(effectVariant);

    return executeClusterCommand(ZigbeeClusterIdentify::CommandTriggerEffect, payload);
}

void ZigbeeClusterIdentify::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    qCDebug(dcZigbeeCluster()) << "Update attribute" << m_node << m_endpoint << this << static_cast<Attribute>(attribute.id()) << attribute.dataType();
    if (hasAttribute(attribute.id())) {
        m_attributes[attribute.id()] = attribute;
        emit attributeChanged(attribute);
    } else {
        m_attributes.insert(attribute.id(), attribute);
        emit attributeChanged(attribute);
    }
}

void ZigbeeClusterIdentify::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    qCDebug(dcZigbeeCluster()) << "Processing cluster frame" << m_node << m_endpoint << this << frame;

    // TODO: implement identify query response

    // Increase the tsn for continuouse id increasing on both sides
    m_transactionSequenceNumber = frame.header.transactionSequenceNumber;

    qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
}
