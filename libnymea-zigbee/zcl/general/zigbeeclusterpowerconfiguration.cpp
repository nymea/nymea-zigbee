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

#include "zigbeeclusterpowerconfiguration.h"
#include "loggingcategory.h"

#include <QDataStream>

ZigbeeClusterPowerConfiguration::ZigbeeClusterPowerConfiguration(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdPowerConfiguration, direction, parent)
{

}

double ZigbeeClusterPowerConfiguration::batteryPercentage() const
{
    return m_batteryPercentage;
}

double ZigbeeClusterPowerConfiguration::batteryVoltage() const
{
    return m_batteryVoltage;
}

ZigbeeClusterPowerConfiguration::BatteryAlarmMask ZigbeeClusterPowerConfiguration::batteryAlarmState() const
{
    return m_batteryAlarmState;
}

void ZigbeeClusterPowerConfiguration::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    if (attribute.id() == AttributeBatteryVoltage) {
        bool ok;
        quint8 value = attribute.dataType().toUInt8(&ok);
        if (ok) {
            m_batteryVoltage = value / 10.0;
            qCDebug(dcZigbeeCluster()) << "PowerConfiguration battery voltage changed on" << m_node << m_endpoint << this << m_batteryVoltage << "V";
            emit batteryVoltageChanged(m_batteryVoltage);
        } else {
            qCWarning(dcZigbeeCluster()) << "Failed to parse battery voltage attribute data"  << m_node << m_endpoint << this << attribute;
        }
    } else if (attribute.id() == AttributeBatteryPercentageRemaining) {
        bool valueOk = false;
        quint8 value = attribute.dataType().toUInt8(&valueOk);
        if (valueOk) {
            m_batteryPercentage = value / 2.0;
            qCDebug(dcZigbeeCluster()) << "PowerConfiguration remaining battery percentage changed on" << m_node << m_endpoint << this << m_batteryPercentage << "%";
            emit batteryPercentageChanged(m_batteryPercentage);
        } else {
            qCWarning(dcZigbeeCluster()) << "Failed to parse battery percentage attribute data"  << m_node << m_endpoint << this << attribute;
        }
    } else if (attribute.id() == AttributeBatteryAlarmState) {
        bool ok;
        quint32 alarmState = attribute.dataType().toUInt32(&ok);
        if (ok) {
            m_batteryAlarmState = static_cast<BatteryAlarmMask>(alarmState);
            emit batteryAlarmStateChanged(m_batteryAlarmState);
        } else {
            qCWarning(dcZigbeeCluster()) << "Failed to parse attribute data" << m_node << m_endpoint << this << attribute;
        }
    }
}
