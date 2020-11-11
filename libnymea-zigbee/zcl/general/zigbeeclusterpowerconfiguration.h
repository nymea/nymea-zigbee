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

#ifndef ZIGBEECLUSTERPOWERCONFIGURATION_H
#define ZIGBEECLUSTERPOWERCONFIGURATION_H

#include <QObject>

#include "zcl/zigbeecluster.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterPowerConfiguration : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Attribute {
        // Mains information
        AttributeMainsVoltage = 0x0000,
        AttributeMainsFrequency = 0x0001,

        // Mains settings
        AttributeMainsAlarmMask = 0x0010,
        AttributeMainsVoltageMinThreshold = 0x0011,
        AttributeMainsVoltageMaxThreshold = 0x0012,
        AttributeMainsVoltageDwellTripPoint = 0x0013,

        // Battery information
        AttributeBatteryVoltage = 0x0020,
        AttributeBatteryPercentageRemaining = 0x0021,

        // Battery settings
        AttributeBatteryManufacturer = 0x0030,
        AttributeBatterySize = 0x0031,
        AttributeBatteryAHrRating = 0x0032,
        AttributeBatteryQuality = 0x0033,
        AttributeBatteryRatedVoltage = 0x0034,
        AttributeBatteryAlarmMask = 0x0035,
        AttributeBatteryVoltageMinThreshold = 0x0036,
        AttributeBatteryVoltageThreshold1 = 0x0037,
        AttributeBatteryVoltageThreshold2 = 0x0038,
        AttributeBatteryVoltageThreshold3 = 0x0039,
        AttributeBatteryPercentageMinThreshold = 0x003a,
        AttributeBatteryPercentageThreshold1 = 0x003b,
        AttributeBatteryPercentageThreshold2 = 0x003c,
        AttributeBatteryPercentageThreshold3 = 0x003d,
        AttributeBatteryAlarmState = 0x003e
    };
    Q_ENUM(Attribute)

    enum MainsAlarmMask {
        MainsAlarmMaskMainVoltageToLow = 0x01,
        MainsAlarmMaskMainVoltageToHigh = 0x02,
        MainsAlarmMaskMainPowerSupplyLost = 0x04
    };
    Q_ENUM(MainsAlarmMask)
    Q_DECLARE_FLAGS(MainsAlarmMaskFlag, MainsAlarmMask)

    explicit ZigbeeClusterPowerConfiguration(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

private:
    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

signals:
    void batteryPercentageChanged(double percentage);

};

Q_DECLARE_OPERATORS_FOR_FLAGS(ZigbeeClusterPowerConfiguration::MainsAlarmMaskFlag)


#endif // ZIGBEECLUSTERPOWERCONFIGURATION_H
