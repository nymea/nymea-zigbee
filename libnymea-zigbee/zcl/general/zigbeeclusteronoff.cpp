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

ZigbeeClusterReply *ZigbeeClusterOnOff::commandToggle()
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
    header.command = ZigbeeClusterOnOff::CommandToggle;
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
    qCDebug(dcZigbeeCluster()) << "Update attribute" << m_node << m_endpoint << this << static_cast<Attribute>(attribute.id()) << attribute.dataType();
    if (hasAttribute(attribute.id())) {
        m_attributes[attribute.id()] = attribute;
        emit attributeChanged(attribute);
    } else {
        m_attributes.insert(attribute.id(), attribute);
        emit attributeChanged(attribute);
    }

    // Parse the information for convinience
    if (attribute.id() == AttributeOnOff) {
        bool valueOk = false;
        bool value = attribute.dataType().toBool(&valueOk);
        if (valueOk) {
            qCDebug(dcZigbeeCluster()) << "OnOff state changed on" << m_node << m_endpoint << this << value;
            emit powerChanged(value);
        }
    }
}

void ZigbeeClusterOnOff::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    qCDebug(dcZigbeeCluster()) << "Processing cluster frame" << m_node << m_endpoint << this << frame;

    // Increase the tsn for continuouse id increasing on both sides
    m_transactionSequenceNumber = frame.header.transactionSequenceNumber;

    switch (m_direction) {
    case Client:
        // If the client cluster sends data to a server cluster (independent which), the command was executed on the device like button pressed
        if (frame.header.frameControl.direction == ZigbeeClusterLibrary::DirectionClientToServer) {
            // Read the payload which is
            Command command = static_cast<Command>(frame.header.command);
            qCDebug(dcZigbeeCluster()) << "Command sent from" << m_node << m_endpoint << this << command;
            switch (command) {
            case CommandOn:
                emit commandSent(CommandOn);
                break;
            case CommandOff:
                emit commandSent(CommandOff);
                break;
            case CommandToggle:
                emit commandSent(CommandToggle);
                break;
            default:
                qCWarning(dcZigbeeCluster()) << "Unhandled command sent from" << m_node << m_endpoint << this << command;
                break;
            }
            return;
        }
        break;
    case Server:
        // keep it unhandled if not parsed yet in order to warn about the handled indication
        break;
    }

    qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
}
