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

#ifndef ZIGBEECLUSTERILLUMINANCEMEASUREMENT_H
#define ZIGBEECLUSTERILLUMINANCEMEASUREMENT_H

#include <QObject>
#include "zcl/zigbeecluster.h"
#include "zcl/zigbeeclusterreply.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterIlluminanceMeasurement : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Attribute {
        AttributeMeasuredValue = 0x0000,
        AttributeMinMeasuredValue = 0x0001,
        AttributeMaxMeasuredValue = 0x0002,
        AttributeTolerance = 0x0003,
        AttributeLightSensorType = 0x0004
    };
    Q_ENUM(Attribute)

    enum LightSensorType {
        LightSensorTypePhotodiode = 0x00,
        LightSensorTypeCMOS = 0x01,
        LightSensorTypeUnknown = 0xff
    };
    Q_ENUM(LightSensorType)

    explicit ZigbeeClusterIlluminanceMeasurement(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    quint16 illuminance() const;

private:
    quint16 m_illuminance = 0;

    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

signals:
    void illuminanceChanged(quint16 illuminance);


};

#endif // ZIGBEECLUSTERILLUMINANCEMEASUREMENT_H
