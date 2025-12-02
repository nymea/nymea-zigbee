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

#include "zigbeeclustertemperaturemeasurement.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"

ZigbeeClusterTemperatureMeasurement::ZigbeeClusterTemperatureMeasurement(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdTemperatureMeasurement, direction, parent)
{

}

ZigbeeClusterReply *ZigbeeClusterTemperatureMeasurement::readTemperature()
{
    ZigbeeClusterReply *readTempReply = readAttributes({ZigbeeClusterTemperatureMeasurement::AttributeMeasuredValue});
    connect(readTempReply, &ZigbeeClusterReply::finished, this, [=](){
        if (readTempReply->error() != ZigbeeClusterReply::ErrorNoError) {
            qCWarning(dcZigbeeCluster()) << "Failed to read min/max/temp values." << readTempReply->error();
            return;
        }
    });
    return readTempReply;
}

ZigbeeClusterReply *ZigbeeClusterTemperatureMeasurement::readMinMaxTemperature()
{
    ZigbeeClusterReply *readMinMaxTempReply = readAttributes({ZigbeeClusterTemperatureMeasurement::AttributeMeasuredValue, ZigbeeClusterTemperatureMeasurement::AttributeMinMeasuredValue, ZigbeeClusterTemperatureMeasurement::AttributeMaxMeasuredValue});
    connect(readMinMaxTempReply, &ZigbeeClusterReply::finished, this, [=](){
        if (readMinMaxTempReply->error() != ZigbeeClusterReply::ErrorNoError) {
            qCWarning(dcZigbeeCluster()) << "Failed to read min/max/temp values." << readMinMaxTempReply->error();
            return;
        }
    });
    return readMinMaxTempReply;
}

double ZigbeeClusterTemperatureMeasurement::temperature() const
{
    return m_temperature;
}

double ZigbeeClusterTemperatureMeasurement::minTemperature() const
{
    return m_minTemperature;
}

double ZigbeeClusterTemperatureMeasurement::maxTemperature() const
{
    return m_maxTemperature;
}

void ZigbeeClusterTemperatureMeasurement::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    // Parse the information for convenience
    if (attribute.id() == AttributeMeasuredValue) {
        bool valueOk = false;
        qint16 value = attribute.dataType().toInt16(&valueOk);
        if (valueOk) {
            if (value == static_cast<qint16>(0x8000)) {
                qCDebug(dcZigbeeCluster()) << m_node << m_endpoint << this << "received invalid measurement value. Not updating the attribute.";
                return;
            }

            m_temperature = value / 100.0;
            qCDebug(dcZigbeeCluster()) << "Temperature changed on" << m_node << m_endpoint << this << m_temperature << "°C";
            emit temperatureChanged(m_temperature);
        }
    }
    if (attribute.id() == AttributeMinMeasuredValue) {
        bool valueOk = false;
        qint16 value = attribute.dataType().toInt16(&valueOk);
        if (valueOk) {
            if (value == static_cast<qint16>(0x8000)) {
                qCDebug(dcZigbeeCluster()) << m_node << m_endpoint << this << "received invalid min measurement value. Not updating the attribute.";
                return;
            }

            m_temperature = value / 100.0;
            qCDebug(dcZigbeeCluster()) << "Temperature changed on" << m_node << m_endpoint << this << m_temperature << "°C";
            emit temperatureChanged(m_temperature);
        }
    }
}
