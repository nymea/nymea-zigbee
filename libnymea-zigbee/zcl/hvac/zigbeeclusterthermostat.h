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

#ifndef ZIGBEECLUSTERTHERMOSTAT_H
#define ZIGBEECLUSTERTHERMOSTAT_H

#include "zcl/zigbeecluster.h"

class ZigbeeClusterThermostat : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:

    enum Attribute {
        AttributeLocalTemperature = 0x0000,
        AttributeOutdoorTemperature = 0x0001,
        AttributeOccupancy = 0x0002,
        AttributeAbsMinHeatSetpointLimit = 0x0003,
        AttributeAbsMaxHeatSetpointLimit = 0x0004,
        AttributeAbsMinCoolSetpointLimit = 0x0005,
        AttributeAbsMaxCoolSetpointLimit = 0x0006,
        AttributePICoolingDemand = 0x0007,
        AttributePIHeatingDemand = 0x0008,
        AttributeHVACSystemTypeConfiguration = 0x0009
    };
    Q_ENUM(Attribute)

    explicit ZigbeeClusterThermostat(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

protected:
    void processDataIndication(ZigbeeClusterLibrary::Frame frame) override;

};

#endif // ZIGBEECLUSTERTHERMOSTAT_H
