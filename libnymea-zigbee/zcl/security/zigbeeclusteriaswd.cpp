// SPDX-License-Identifier: LGPL-3.0-or-later

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* nymea-zigbee
* Zigbee integration module for nymea
*
* Copyright (C) 2013 - 2024, nymea GmbH
* Copyright (C) 2024 - 2025, chargebyte austria GmbH
*
* This file is part of nymea-zigbee.
*
* nymea-zigbee is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3
* of the License, or (at your option) any later version.
*
* nymea-zigbee is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with nymea-zigbee. If not, see <https://www.gnu.org/licenses/>.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "zigbeeclusteriaswd.h"

#include "loggingcategory.h"

#include <QDataStream>

ZigbeeClusterIasWd::ZigbeeClusterIasWd(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent):
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdIasWd, direction, parent)
{

}

ZigbeeClusterReply *ZigbeeClusterIasWd::startWarning(WarningMode warningMode, bool strobeEnabled, SirenLevel sirenLevel, quint16 duration, quint8 strobeDutyCycle, StrobeLevel strobeLevel)
{
    QByteArray payload;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QDataStream stream(&payload, QDataStream::WriteOnly);
#else
    QDataStream stream(&payload, QIODevice::WriteOnly);
#endif
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(warningMode | (strobeEnabled ? 0x04 : 0x00) | sirenLevel);
    stream << duration;
    stream << strobeDutyCycle;
    stream << static_cast<quint8>(strobeLevel);
    qCDebug(dcZigbeeCluster) << "Sending payload:" << payload.toHex();
    qCDebug(dcZigbeeCluster) << "1:" << static_cast<quint8>(warningMode | (strobeEnabled ? 0x04 : 0x00) | sirenLevel);
    ZigbeeClusterReply *reply = executeClusterCommand(ServerCommandStartWarning, payload);
    return reply;
}

ZigbeeClusterReply *ZigbeeClusterIasWd::squawk(SquawkMode squawkMode, bool strobeEnabled, SquawkLevel squawkLevel)
{
    QByteArray payload;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QDataStream stream(&payload, QDataStream::WriteOnly);
#else
    QDataStream stream(&payload, QIODevice::WriteOnly);
#endif
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(squawkMode | (strobeEnabled ? 0x08 : 0x00) | squawkLevel);
    qCDebug(dcZigbeeCluster) << "Sending payload:" << payload.toHex();
    ZigbeeClusterReply *reply = executeClusterCommand(ServerCommandSquawk, payload);
    return reply;
}


void ZigbeeClusterIasWd::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    qCDebug(dcZigbeeCluster()) << "Update attribute" << m_node << m_endpoint << this << static_cast<Attribute>(attribute.id()) << attribute.dataType();
    ZigbeeCluster::setAttribute(attribute);

    if (attribute.id() == AttributeMaxDuration) {
        bool valueOk = false;
        quint8 value = attribute.dataType().toUInt16(&valueOk);
        if (valueOk) {
            m_maxDuration = value;
            qCDebug(dcZigbeeCluster()) << "IAS WD max duration changed on" << m_node << m_endpoint << this << m_maxDuration << "s";
            emit maxDurationChanged(m_maxDuration);
        } else {
            qCWarning(dcZigbeeCluster()) << "Failed to parse IAS WD max duration attribute data"  << m_node << m_endpoint << this << attribute;
        }
    }
}
