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

#include "zigbeenodedeconz.h"
#include "zigbeedeviceprofile.h"
#include "zigbeenetworkdeconz.h"
#include "loggingcategory.h"

#include <QDataStream>

ZigbeeNodeDeconz::ZigbeeNodeDeconz(ZigbeeNetworkDeconz *network, QObject *parent) :
    ZigbeeNode(parent),
    m_network(network)
{

}

void ZigbeeNodeDeconz::leaveNetworkRequest(bool rejoin, bool removeChildren)
{
    Q_UNUSED(rejoin)
    Q_UNUSED(removeChildren)
}

void ZigbeeNodeDeconz::setClusterAttributeReport(const ZigbeeClusterAttributeReport &report)
{
    Q_UNUSED(report)
}

void ZigbeeNodeDeconz::startInitialization()
{
    setState(StateInitializing);

    // Get the node descriptor
    ZigbeeNetworkRequest request;
    request.setRequestId(m_network->generateSequenceNumber());
    request.setDestinationAddressMode(Zigbee::DestinationAddressModeShortAddress);
    request.setDestinationShortAddress(shortAddress());
    request.setDestinationEndpoint(0); // ZDO
    request.setProfileId(Zigbee::ZigbeeProfileDevice); // ZDP
    request.setClusterId(ZigbeeDeviceProfile::NodeDescriptorRequest);
    request.setSourceEndpoint(0); // ZDO

    // Build ASDU
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << request.requestId() << request.destinationShortAddress();
    request.setAsdu(asdu);

    ZigbeeNetworkReply *reply = m_network->sendRequest(request);
    connect(reply, &ZigbeeNetworkReply::finished, this, [this, reply](){
        // TODO: check reply error

        ZigbeeDeviceProfileAdpu adpu = ZigbeeDeviceProfile::parseAdpu(reply->responseData());
        qCDebug(dcZigbeeNode()) << "Node descriptor request finished" << adpu;
        setNodeDescriptorRawData(reply->responseData());

        QDataStream stream(adpu.payload);
        stream.setByteOrder(QDataStream::LittleEndian);



    });

    /* Node initialisation steps (sequentially)
     * - Node descriptor
     * - Power descriptor
     * - Active endpoints
     * - for each endpoint do:
     *    - Simple descriptor request
     *    - for each endpoint
     *      - read basic cluster
     */


}

ZigbeeNodeEndpoint *ZigbeeNodeDeconz::createNodeEndpoint(quint8 endpointId, QObject *parent)
{
    Q_UNUSED(endpointId)
    Q_UNUSED(parent)
    return nullptr;
}
