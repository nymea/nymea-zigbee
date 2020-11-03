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

#include "zigbeeclusteronoff.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"

#include <QDataStream>

ZigbeeClusterOnOff::ZigbeeClusterOnOff(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdOnOff, direction, parent)
{

}

ZigbeeClusterReply *ZigbeeClusterOnOff::commandOff()
{
    return executeClusterCommand(ZigbeeClusterOnOff::CommandOff);
}

ZigbeeClusterReply *ZigbeeClusterOnOff::commandOn()
{
    return executeClusterCommand(ZigbeeClusterOnOff::CommandOn);
}

ZigbeeClusterReply *ZigbeeClusterOnOff::commandToggle()
{
    return executeClusterCommand(ZigbeeClusterOnOff::CommandToggle);
}

ZigbeeClusterReply *ZigbeeClusterOnOff::commandOffWithEffect(ZigbeeClusterOnOff::Effect effect, quint8 effectVariant)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(effect) << effectVariant;
    return executeClusterCommand(ZigbeeClusterOnOff::CommandOffWithEffect, payload);
}

ZigbeeClusterReply *ZigbeeClusterOnOff::commandOnWithRecallGlobalScene()
{
    return executeClusterCommand(ZigbeeClusterOnOff::CommandOnWithRecallGlobalScene);
}

ZigbeeClusterReply *ZigbeeClusterOnOff::commandOnWithTimedOff(bool acceptOnlyWhenOn, quint16 onTime, quint16 offWaitTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(acceptOnlyWhenOn) << onTime << offWaitTime;
    return executeClusterCommand(ZigbeeClusterOnOff::CommandOnWithTimedOff, payload);
}

bool ZigbeeClusterOnOff::powered() const
{
    ZigbeeClusterAttribute onOffAttribute = attribute(ZigbeeClusterOnOff::AttributeOnOff);
    return onOffAttribute.dataType().toBool();
}

void ZigbeeClusterOnOff::setAttribute(const ZigbeeClusterAttribute &attribute)
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
    if (attribute.id() == AttributeOnOff) {
        bool valueOk = false;
        bool value = attribute.dataType().toBool(&valueOk);
        if (valueOk) {
            qCDebug(dcZigbeeCluster()) << "OnOff state changed on" << m_node << m_endpoint << this << value;
            emit powerChanged(value);
        } else {
            qCWarning(dcZigbeeCluster()) << "Failed to parse attribute data"  << m_node << m_endpoint << this << attribute;
        }
    }
}

void ZigbeeClusterOnOff::processDataIndication(ZigbeeClusterLibrary::Frame frame)
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
            switch (command) {
            case CommandOn:
                emit commandSent(CommandOn);
                break;
            case CommandOff:
                emit commandSent(CommandOff);
                break;
            case CommandToggle:
                emit commandSent(CommandToggle);
                break;
            default:
                qCWarning(dcZigbeeCluster()) << "Unhandled command sent from" << m_node << m_endpoint << this << command;
                break;
            }
        }
        break;
    case Server:
        qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
        break;
    }
}
