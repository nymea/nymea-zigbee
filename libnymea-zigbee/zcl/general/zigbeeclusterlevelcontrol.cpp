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
#include "zigbeeutils.h"

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

ZigbeeClusterReply *ZigbeeClusterLevelControl::commandStep(StepMode stepMode, quint8 stepSize, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(stepMode) << stepSize << transitionTime;
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

ZigbeeClusterReply *ZigbeeClusterLevelControl::commandStepWithOnOff(StepMode stepMode, quint8 stepSize, quint16 transitionTime)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(stepMode) << stepSize << transitionTime;
    return executeClusterCommand(ZigbeeClusterLevelControl::CommandStepWithOnOff, payload);
}

ZigbeeClusterReply *ZigbeeClusterLevelControl::commandStopWithOnOff()
{
    return executeClusterCommand(ZigbeeClusterLevelControl::CommandStopWithOnOff);
}

quint8 ZigbeeClusterLevelControl::currentLevel() const
{
    return m_currentLevel;
}

void ZigbeeClusterLevelControl::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    // Parse the information for convenience
    if (attribute.id() == AttributeCurrentLevel) {
        bool valueOk = false;
        quint8 value = attribute.dataType().toUInt8(&valueOk);
        if (valueOk) {
            m_currentLevel = value;
            qCDebug(dcZigbeeCluster()) << "CurrentLevel state changed on" << m_node << m_endpoint << this << m_currentLevel;
            emit currentLevelChanged(m_currentLevel);
        } else {
            qCWarning(dcZigbeeCluster()) << "Failed to parse attribute data"  << m_node << m_endpoint << this << attribute;
        }
    }
}

void ZigbeeClusterLevelControl::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    switch (m_direction) {
    case Client:
        // If the client cluster sends data to a server cluster (independent which), the command was executed on the device like button pressed
        if (frame.header.frameControl.direction == ZigbeeClusterLibrary::DirectionClientToServer) {
            // Read the payload which is
            Command command = static_cast<Command>(frame.header.command);
            emit commandSent(command, frame.payload, frame.header.transactionSequenceNumber);

            bool withOnOff = false;
            switch (command) {
            case CommandMoveToLevelWithOnOff:
            case CommandMoveToLevel: {
                QByteArray payload = frame.payload;
                QDataStream payloadStream(&payload, QIODevice::ReadOnly);
                payloadStream.setByteOrder(QDataStream::LittleEndian);
                quint8 level; quint16 transitionTime;
                payloadStream >> level >> transitionTime;
                withOnOff = command == CommandMoveToLevelWithOnOff;
                qCDebug(dcZigbeeCluster()).noquote().nospace() << "Command received from " << m_node << " " << m_endpoint << " " << this << " " << command << " withOnOff: " << withOnOff << " level: 0x" << QString::number(level, 16) << " transitionTime: 0x" << QString::number(transitionTime, 16);
                emit commandMoveToLevelSent(withOnOff, level, transitionTime, frame.header.transactionSequenceNumber);
                break;
            }
            case CommandStepWithOnOff:
            case CommandStep: {
                QByteArray payload = frame.payload;
                QDataStream payloadStream(&payload, QIODevice::ReadOnly);
                payloadStream.setByteOrder(QDataStream::LittleEndian);
                quint8 stepModeValue = 0; quint8 stepSize; quint16 transitionTime;
                payloadStream >> stepModeValue >> stepSize >> transitionTime;
                withOnOff = command == CommandMoveToLevelWithOnOff;
                qCDebug(dcZigbeeCluster()).noquote().nospace() << "Command received from " << m_node << " " << m_endpoint << " " << this << " " << command << " withOnOff: " << withOnOff << " stepModeValue: 0x" << QString::number(stepModeValue, 16) << " stepSize: 0x" << QString::number(stepSize, 16) << " transitionTime: 0x" << QString::number(transitionTime, 16);
                emit commandStepSent(withOnOff, static_cast<StepMode>(stepModeValue), stepSize, transitionTime, frame.header.transactionSequenceNumber);
                break;
            }
            case CommandMoveWithOnOff:
            case CommandMove: {
                QByteArray payload = frame.payload;
                QDataStream payloadStream(&payload, QIODevice::ReadOnly);
                payloadStream.setByteOrder(QDataStream::LittleEndian);
                quint8 moveModeValue = 0; quint8 rate;;
                payloadStream >> moveModeValue >> rate;
                withOnOff = command == CommandMoveToLevelWithOnOff;
                qCDebug(dcZigbeeCluster()).noquote().nospace() << "Command received from " << m_node << " " << m_endpoint << " " << this << " " << command << " withOnOff:" << withOnOff <<  " moveModeValue: 0x" << QString::number(moveModeValue, 16) << " rate: 0x" << QString::number(rate, 16);
                emit commandMoveSent(withOnOff, static_cast<MoveMode>(moveModeValue), rate, frame.header.transactionSequenceNumber);
                break;
            }
            case CommandStopWithOnOff:
            case CommandStop:
                withOnOff = command == CommandMoveToLevelWithOnOff;
                emit commandStopSent(withOnOff, frame.header.transactionSequenceNumber);
                break;
            default:
                qCDebug(dcZigbeeCluster()).noquote().nospace() << "Command received from " << m_node << " " << m_endpoint << " " << this << " " << command << " payload: 0x" << ZigbeeUtils::convertByteArrayToHexString(frame.payload);
                break;
            }

        }
        break;
    case Server:
        qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
        break;
    }
}
