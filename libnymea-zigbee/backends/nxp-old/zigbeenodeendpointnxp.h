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

#ifndef ZIGBEENODEENDPOINTNXP_H
#define ZIGBEENODEENDPOINTNXP_H

#include <QObject>

#include "zigbeenodeendpoint.h"
#include "zigbeebridgecontrollernxp.h"

class ZigbeeNodeEndpointNxp : public ZigbeeNodeEndpoint
{
    Q_OBJECT

    friend class ZigbeeNodeNxp;
    friend class ZigbeeNetworkNxp;

public:
    explicit ZigbeeNodeEndpointNxp(ZigbeeBridgeControllerNxp *controller, ZigbeeNode *node, quint8 endpoint, QObject *parent = nullptr);

    ZigbeeNetworkReply *readAttribute(ZigbeeCluster *cluster, QList<quint16> attributes) override;
    ZigbeeNetworkReply *configureReporting(ZigbeeCluster *cluster, QList<ZigbeeClusterReportConfigurationRecord> reportConfigurations) override;

    ZigbeeNetworkReply *identify(quint16 seconds) override;
    ZigbeeNetworkReply *factoryReset() override;
    ZigbeeNetworkReply *bindGroup(Zigbee::ClusterId clusterId, quint16 destinationAddress, quint8 destinationEndpoint) override;
    ZigbeeNetworkReply *bindUnicast(Zigbee::ClusterId clusterId, const ZigbeeAddress &destinationAddress, quint8 destinationEndpoint) override;
    ZigbeeNetworkReply *sendOnOffClusterCommand(ZigbeeCluster::OnOffClusterCommand command) override;
    ZigbeeNetworkReply *addGroup(quint8 destinationEndpoint, quint16 groupAddress) override;

    // Level control
    ZigbeeNetworkReply *sendLevelCommand(ZigbeeCluster::LevelClusterCommand command, quint8 level, bool triggersOnOff, quint16 transitionTime) override;

    // Color control
    ZigbeeNetworkReply *sendMoveToColorTemperature(quint16 colourTemperature, quint16 transitionTime) override;
    ZigbeeNetworkReply *sendMoveToColor(double x, double y, quint16 transitionTime) override;
    ZigbeeNetworkReply *sendMoveToHueSaturation(quint8 hue, quint8 saturation, quint16 transitionTime) override;
    ZigbeeNetworkReply *sendMoveToHue(quint8 hue, quint16 transitionTime) override;
    ZigbeeNetworkReply *sendMoveToSaturation(quint8 saturation, quint16 transitionTime) override;


private:
    ZigbeeBridgeControllerNxp *m_controller = nullptr;

    void setClusterAttribute(Zigbee::ClusterId clusterId, const ZigbeeClusterAttribute &attribute = ZigbeeClusterAttribute()) override;
};

#endif // ZIGBEENODEENDPOINTNXP_H
