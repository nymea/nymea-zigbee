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

#ifndef ZIGBEENODEENDPOINTDECONZ_H
#define ZIGBEENODEENDPOINTDECONZ_H

#include <QObject>
#include "zigbeenodeendpoint.h"

class ZigbeeNodeDeconz;
class ZigbeeNetworkDeconz;

class ZigbeeNodeEndpointDeconz : public ZigbeeNodeEndpoint
{
    Q_OBJECT

    friend class ZigbeeNodeDeconz;

public:
    explicit ZigbeeNodeEndpointDeconz(ZigbeeNetworkDeconz *network, ZigbeeNode *node, quint8 endpointId, QObject *parent = nullptr);

    ZigbeeNetworkReply *readAttribute(ZigbeeCluster *cluster, QList<quint16> attributes) override;
    ZigbeeNetworkReply *configureReporting(ZigbeeCluster *cluster, QList<ZigbeeClusterReportConfigurationRecord> reportConfigurations) override;

protected:
    // Cluster commands
    void setClusterAttribute(Zigbee::ClusterId clusterId, const ZigbeeClusterAttribute &attribute = ZigbeeClusterAttribute()) override;

private:
    ZigbeeNetworkDeconz *m_network = nullptr;
    ZigbeeNode *m_node = nullptr;

signals:

};

#endif // ZIGBEENODEENDPOINTDECONZ_H
