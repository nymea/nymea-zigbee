/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2023, nymea GmbH
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

#include "zigbeeclusterwindowcovering.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeClusterWindowCovering::ZigbeeClusterWindowCovering(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdWindowCovering, direction, parent)
{

}

ZigbeeClusterReply *ZigbeeClusterWindowCovering::open()
{
    return executeClusterCommand(Command::CommandOpen);
}

ZigbeeClusterReply *ZigbeeClusterWindowCovering::close()
{
    return executeClusterCommand(Command::CommandClose);
}

ZigbeeClusterReply *ZigbeeClusterWindowCovering::stop()
{
    return executeClusterCommand(Command::CommandStop);
}

ZigbeeClusterReply *ZigbeeClusterWindowCovering::goToLiftValue(quint16 liftValue)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << liftValue;
    return executeClusterCommand(Command::CommandGoToLiftValue, payload);
}

ZigbeeClusterReply *ZigbeeClusterWindowCovering::goToLiftPercentage(quint8 liftPercentage)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << liftPercentage;
    return executeClusterCommand(Command::CommandGoToLiftPercentage, payload);
}

ZigbeeClusterReply *ZigbeeClusterWindowCovering::goToTiltValue(quint16 tiltValue)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << tiltValue;
    return executeClusterCommand(Command::CommandGoToTiltValue, payload);
}

ZigbeeClusterReply *ZigbeeClusterWindowCovering::goToTiltPercentage(quint8 tiltPercentage)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << tiltPercentage;
    return executeClusterCommand(Command::CommandGoToTiltPercentage, payload);
}

quint8 ZigbeeClusterWindowCovering::currentLiftPercentage() const
{
    return m_currentLiftPercentage;
}

quint8 ZigbeeClusterWindowCovering::currentTiltPercentage() const
{
    return m_currentTiltPercentage;
}

void ZigbeeClusterWindowCovering::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    switch (attribute.id()) {
    case AttributeCurrentPositionLiftPercentage:
        m_currentLiftPercentage = static_cast<quint8>(attribute.dataType().toUInt8());
        emit currentLiftPercentageChanged(m_currentLiftPercentage);
        break;
    case AttributeCurrentPositionTiltPercentage:
        m_currentTiltPercentage = static_cast<quint8>(attribute.dataType().toUInt8());
        emit currentTiltPercentageChanged(m_currentTiltPercentage);
        break;
    default:
        qCDebug(dcZigbeeCluster()) << "Unhandled WindowCovering cluster attribute change:" << static_cast<Attribute>(attribute.id());
    }
}
