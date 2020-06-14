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

#include "zigbeeclusterlevelcontrol.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"

#include <QDataStream>

ZigbeeClusterLevelControl::ZigbeeClusterLevelControl(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, ZigbeeCluster::Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdLevelControl, direction, parent)
{

}

ZigbeeClusterReply *ZigbeeClusterLevelControl::commandMoveToLevel(quint8 level, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << level << transitionTime;
    return executeClusterCommand(ZigbeeClusterLevelControl::CommandMoveToLevel, payload);
}

ZigbeeClusterReply *ZigbeeClusterLevelControl::commandMove(ZigbeeClusterLevelControl::MoveMode moveMode, quint8 rate)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(moveMode) << rate;
    return executeClusterCommand(ZigbeeClusterLevelControl::CommandMove, payload);
}

ZigbeeClusterReply *ZigbeeClusterLevelControl::commandStep(ZigbeeClusterLevelControl::FadeMode fadeMode, quint8 stepSize, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(fadeMode) << stepSize << transitionTime;
    return executeClusterCommand(ZigbeeClusterLevelControl::CommandStep, payload);
}

ZigbeeClusterReply *ZigbeeClusterLevelControl::commandStop()
{
    return executeClusterCommand(ZigbeeClusterLevelControl::CommandStop);
}

ZigbeeClusterReply *ZigbeeClusterLevelControl::commandMoveToLevelWithOnOff(quint8 level, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << level << transitionTime;
    return executeClusterCommand(ZigbeeClusterLevelControl::CommandMoveToLevelWithOnOff, payload);
}

ZigbeeClusterReply *ZigbeeClusterLevelControl::commandMoveWithOnOff(ZigbeeClusterLevelControl::MoveMode moveMode, quint8 rate)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(moveMode) << rate;
    return executeClusterCommand(ZigbeeClusterLevelControl::CommandMoveWithOnOff, payload);
}

ZigbeeClusterReply *ZigbeeClusterLevelControl::commandStepWithOnOff(ZigbeeClusterLevelControl::FadeMode fadeMode, quint8 stepSize, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(fadeMode) << stepSize << transitionTime;
    return executeClusterCommand(ZigbeeClusterLevelControl::CommandStepWithOnOff, payload);
}

ZigbeeClusterReply *ZigbeeClusterLevelControl::commandStopWithOnOff()
{
    return executeClusterCommand(ZigbeeClusterLevelControl::CommandStopWithOnOff);
}

void ZigbeeClusterLevelControl::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    qCDebug(dcZigbeeCluster()) << "Update attribute" << m_node << m_endpoint << this << static_cast<Attribute>(attribute.id()) << attribute.dataType();
    if (hasAttribute(attribute.id())) {
        m_attributes[attribute.id()] = attribute;
        emit attributeChanged(attribute);
    } else {
        m_attributes.insert(attribute.id(), attribute);
        emit attributeChanged(attribute);
    }

    // Parse the information for convinience
    if (attribute.id() == AttributeCurrentLevel) {
        bool valueOk = false;
        quint8 value = attribute.dataType().toUInt8(&valueOk);
        if (valueOk) {
            qCDebug(dcZigbeeCluster()) << "CurrentLevel state changed on" << m_node << m_endpoint << this << value;
            emit currentLevelChanged(value);
        } else {
            qCWarning(dcZigbeeCluster()) << "Failed to parse attribute data"  << m_node << m_endpoint << this << attribute;
        }
    }
}

void ZigbeeClusterLevelControl::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    qCDebug(dcZigbeeCluster()) << "Processing cluster frame" << m_node << m_endpoint << this << frame;

    // Increase the tsn for continuouse id increasing on both sides
    m_transactionSequenceNumber = frame.header.transactionSequenceNumber;

    switch (m_direction) {
    case Client:
        // If the client cluster sends data to a server cluster (independent which), the command was executed on the device like button pressed
        if (frame.header.frameControl.direction == ZigbeeClusterLibrary::DirectionClientToServer) {
            // Read the payload which is
            Command command = static_cast<Command>(frame.header.command);
            qCDebug(dcZigbeeCluster()) << "Command sent from" << m_node << m_endpoint << this << command;
            emit commandSent(command, frame.payload);
        }
        break;
    case Server:
        qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
        break;
    }
}
