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

#include "zigbeeclustercolorcontrol.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"

#include <QDataStream>

ZigbeeClusterColorControl::ZigbeeClusterColorControl(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, ZigbeeCluster::Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, Zigbee::ClusterIdColorControl, direction, parent)
{

}

/*! Send the move to \a hue command with the given \a direction and \a transitionTime. The transition time has the unit 1/10 seconds. */
ZigbeeClusterReply *ZigbeeClusterColorControl::commandMoveToHue(quint8 hue, ZigbeeClusterColorControl::MoveDirection direction, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << hue << static_cast<quint8>(direction) << transitionTime;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandMoveToHue, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandMoveHue(ZigbeeClusterColorControl::MoveMode moveMode, quint8 rate)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(moveMode) << rate;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandMoveHue, payload);
}

/*! Send the step hue command with the given \a stepMode, \a stepSize and \a transitionTime. The transition time has the unit 1/10 seconds. */
ZigbeeClusterReply *ZigbeeClusterColorControl::commandStepHue(ZigbeeClusterColorControl::StepMode stepMode, quint8 stepSize, quint8 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(stepMode) << stepSize << transitionTime;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandStepHue, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandMoveToSaturation(quint8 saturation, ZigbeeClusterColorControl::MoveDirection direction, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << saturation << static_cast<quint8>(direction) << transitionTime;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandMoveToSaturation, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandMoveSaturation(ZigbeeClusterColorControl::MoveMode moveMode, quint8 rate)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(moveMode) << rate;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandMoveSaturation, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandStepSaturation(ZigbeeClusterColorControl::StepMode stepMode, quint8 stepSize, quint8 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(stepMode) << stepSize << transitionTime;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandStepSaturation, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandMoveToHueAndSaturation(quint8 hue, quint8 saturation, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << hue << saturation << transitionTime;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandMoveToHueAndSaturation, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandMoveToColor(quint16 colorX, quint16 colorY, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << colorX << colorY << transitionTime;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandMoveToColor, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandMoveColor(quint16 colorXRate, quint16 colorYRate)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << colorXRate << colorYRate;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandMoveColor, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandStepColor(quint16 stepX, quint16 stepY, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << stepX << stepY << transitionTime;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandStepColor, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandMoveToColorTemperature(quint16 colorTemperatureMireds, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << colorTemperatureMireds << transitionTime;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandMoveToColorTemperature, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandEnhancedMoveToHue(quint16 enhancedHue, ZigbeeClusterColorControl::MoveDirection direction, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << enhancedHue << static_cast<quint8>(direction) << transitionTime;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandEnhancedMoveToHue, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandEnhancedMoveHue(ZigbeeClusterColorControl::MoveMode moveMode, quint16 rate)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(moveMode) << rate;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandEnhancedMoveHue, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandEnhancedStepHue(ZigbeeClusterColorControl::StepMode stepMode, quint16 stepSize, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(stepMode) << stepSize << transitionTime;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandEnhancedStepHue, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandEnhancedMoveToHueAndSaturation(quint16 enhancedHue, quint8 saturation, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << enhancedHue << saturation << transitionTime;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandEnhancedMoveToHueAndSaturation, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandColorLoopSet(ColorLoopUpdateFlags updateFlag, ZigbeeClusterColorControl::ColorLoopAction action, ZigbeeClusterColorControl::ColorLoopDirection direction, quint16 time, quint16 startHue)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(updateFlag);
    stream << static_cast<quint8>(action);
    stream << static_cast<quint8>(direction);
    stream << time << startHue;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandColorLoopSet, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandStopMoveStep()
{
    return executeClusterCommand(ZigbeeClusterColorControl::CommandStopMoveStep);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandMoveColorTemperature(ZigbeeClusterColorControl::MoveMode moveMode, quint16 rate, quint16 minColorTemperature, quint16 maxColorTemperature)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(moveMode) << rate << minColorTemperature << maxColorTemperature;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandMoveColorTemperature, payload);
}

ZigbeeClusterReply *ZigbeeClusterColorControl::commandStepColorTemperature(ZigbeeClusterColorControl::StepMode stepMode, quint16 stepSize, quint16 transitionTime, quint16 minColorTemperature, quint16 maxColorTemperature)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(stepMode) << stepSize << transitionTime << minColorTemperature << maxColorTemperature;
    return executeClusterCommand(ZigbeeClusterColorControl::CommandStepColorTemperature, payload);
}

void ZigbeeClusterColorControl::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    qCDebug(dcZigbeeCluster()) << "Attribute changed" << m_node << m_endpoint << this << static_cast<Attribute>(attribute.id()) << attribute.dataType();
    if (hasAttribute(attribute.id())) {
        m_attributes[attribute.id()] = attribute;
        emit attributeChanged(attribute);
    } else {
        m_attributes.insert(attribute.id(), attribute);
        emit attributeChanged(attribute);
    }


}

void ZigbeeClusterColorControl::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    qCDebug(dcZigbeeCluster()) << "Processing cluster frame" << m_node << m_endpoint << this << frame;

    // Increase the tsn for continuouse id increasing on both sides
    m_transactionSequenceNumber = frame.header.transactionSequenceNumber;
}


