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

#include "zigbeeclusteronoff.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"

#include <QDataStream>

ZigbeeClusterOnOff::ZigbeeClusterOnOff(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, Zigbee::ClusterIdOnOff, direction, parent)
{

}

ZigbeeClusterReply *ZigbeeClusterOnOff::commandOff()
{
    ZigbeeNetworkRequest request = createGeneralRequest();

    // Build ZCL frame

    // Note: for basic commands the frame control files has to be zero accoring to spec ZCL 2.4.1.1
    ZigbeeClusterLibrary::FrameControl frameControl;
    frameControl.frameType = ZigbeeClusterLibrary::FrameTypeClusterSpecific;
    frameControl.manufacturerSpecific = false;
    frameControl.direction = ZigbeeClusterLibrary::DirectionClientToServer;
    frameControl.disableDefaultResponse = false;

    // ZCL header
    ZigbeeClusterLibrary::Header header;
    header.frameControl = frameControl;
    header.command = ZigbeeClusterOnOff::CommandOff;
    header.transactionSequenceNumber = m_transactionSequenceNumber++;

    // there is no ZCL payload

    // Put them together
    ZigbeeClusterLibrary::Frame frame;
    frame.header = header;

    request.setTxOptions(Zigbee::ZigbeeTxOptions(Zigbee::ZigbeeTxOptionAckTransmission));
    request.setAsdu(ZigbeeClusterLibrary::buildFrame(frame));

    ZigbeeClusterReply *zclReply = createClusterReply(request, frame);
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, this, [this, networkReply, zclReply](){
        if (!verifyNetworkError(zclReply, networkReply)) {
            qCWarning(dcZigbeeClusterLibrary()) << "Failed to send request"
                                                << m_node << networkReply->error()
                                                << networkReply->zigbeeApsStatus();
            finishZclReply(zclReply);
            return;
        }

        // The request was successfully sent to the device
        // Now check if the expected indication response received already
        if (zclReply->isComplete()) {
            finishZclReply(zclReply);
            return;
        }
    });

    return zclReply;
}

ZigbeeClusterReply *ZigbeeClusterOnOff::commandOn()
{
    ZigbeeNetworkRequest request = createGeneralRequest();

    // Build ZCL frame

    // Note: for basic commands the frame control files has to be zero accoring to spec ZCL 2.4.1.1
    ZigbeeClusterLibrary::FrameControl frameControl;
    frameControl.frameType = ZigbeeClusterLibrary::FrameTypeClusterSpecific;
    frameControl.manufacturerSpecific = false;
    frameControl.direction = ZigbeeClusterLibrary::DirectionClientToServer;
    frameControl.disableDefaultResponse = false;

    // ZCL header
    ZigbeeClusterLibrary::Header header;
    header.frameControl = frameControl;
    header.command = ZigbeeClusterOnOff::CommandOn;
    header.transactionSequenceNumber = m_transactionSequenceNumber++;

    // There is no ZCL payload

    // Put them together
    ZigbeeClusterLibrary::Frame frame;
    frame.header = header;

    request.setTxOptions(Zigbee::ZigbeeTxOptions(Zigbee::ZigbeeTxOptionAckTransmission));
    request.setAsdu(ZigbeeClusterLibrary::buildFrame(frame));

    ZigbeeClusterReply *zclReply = createClusterReply(request, frame);
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, this, [this, networkReply, zclReply](){
        if (!verifyNetworkError(zclReply, networkReply)) {
            qCWarning(dcZigbeeClusterLibrary()) << "Failed to send request"
                                                << m_node << networkReply->error()
                                                << networkReply->zigbeeApsStatus();
            finishZclReply(zclReply);
            return;
        }

        // The request was successfully sent to the device
        // Now check if the expected indication response received already
        if (zclReply->isComplete()) {
            finishZclReply(zclReply);
            return;
        }
    });

    return zclReply;
}

void ZigbeeClusterOnOff::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    if (hasAttribute(attribute.id())) {
        qCDebug(dcZigbeeCluster()) << "Update attribute" << this << static_cast<Attribute>(attribute.id()) << attribute.dataType();
        m_attributes[attribute.id()] = attribute;
        emit attributeChanged(attribute);
    } else {
        qCDebug(dcZigbeeCluster()) << "Add attribute" << this << static_cast<Attribute>(attribute.id()) << attribute.dataType();
        m_attributes.insert(attribute.id(), attribute);
        emit attributeChanged(attribute);
    }
}

void ZigbeeClusterOnOff::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    // Increase the tsn for continuouse id increasing on both sides
    m_transactionSequenceNumber = frame.header.transactionSequenceNumber;
    qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
}
