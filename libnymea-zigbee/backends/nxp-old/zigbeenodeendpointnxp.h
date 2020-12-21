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
