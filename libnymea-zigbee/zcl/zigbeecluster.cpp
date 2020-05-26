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

#include "zigbeeutils.h"
#include "zigbeenetwork.h"
#include "zigbeecluster.h"
#include "loggingcategory.h"
#include "zigbeenetworkreply.h"
#include "zigbeeclusterlibrary.h"
#include "zigbeenetworkrequest.h"

#include <QDataStream>

ZigbeeCluster::ZigbeeCluster(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Zigbee::ClusterId clusterId, Direction direction, QObject *parent) :
    QObject(parent),
    m_network(network),
    m_node(node),
    m_endpoint(endpoint),
    m_clusterId(clusterId),
    m_direction(direction)
{
    qCDebug(dcZigbeeCluster()) << "Create cluster" << ZigbeeUtils::convertUint16ToHexString(clusterId) << direction;
}

ZigbeeCluster::Direction ZigbeeCluster::direction() const
{
    return m_direction;
}

Zigbee::ClusterId ZigbeeCluster::clusterId() const
{
    return m_clusterId;
}

QString ZigbeeCluster::clusterName() const
{
    return ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(m_clusterId));
}

QList<ZigbeeClusterAttribute> ZigbeeCluster::attributes() const
{
    return m_attributes.values();
}

bool ZigbeeCluster::hasAttribute(quint16 attributeId) const
{
    if (m_attributes.keys().isEmpty())
        return false;

    return m_attributes.keys().contains(attributeId);
}

ZigbeeClusterAttribute ZigbeeCluster::attribute(quint16 attributeId)
{
    return m_attributes.value(attributeId);
}

void ZigbeeCluster::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    if (hasAttribute(attribute.id())) {
        qCDebug(dcZigbeeCluster()) << this << "update attribute" << attribute;
        m_attributes[attribute.id()] = attribute;
        emit attributeChanged(attribute);
    } else {
        qCDebug(dcZigbeeCluster()) << this << "add attribute" << attribute;
        m_attributes.insert(attribute.id(), attribute);
        emit attributeChanged(attribute);
    }
}

ZigbeeClusterReply *ZigbeeCluster::readAttributes(QList<quint16> attributes)
{
    qCDebug(dcZigbeeClusterLibrary()) << "Read attributes from" << m_node << m_endpoint << this << attributes;

    // Build the request
    ZigbeeNetworkRequest request = createGeneralRequest();

    // Build ZCL frame

    // Note: for basic commands the frame control files has to be zero accoring to spec ZCL 2.4.1.1
    ZigbeeClusterLibrary::FrameControl frameControl;
    frameControl.frameType = ZigbeeClusterLibrary::FrameTypeGlobal;
    frameControl.manufacturerSpecific = false;
    if (m_direction == Direction::Input) {
        frameControl.direction = ZigbeeClusterLibrary::DirectionClientToServer;
    } else {
        frameControl.direction = ZigbeeClusterLibrary::DirectionServerToClient;
    }
    frameControl.disableDefaultResponse = true;

    // ZCL header
    ZigbeeClusterLibrary::Header header;
    header.frameControl = frameControl;
    header.command = ZigbeeClusterLibrary::CommandReadAttributes;
    header.transactionSequenceNumber = m_transactionSequenceNumber++;

    // ZCL payload
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    foreach (quint16 attribute, attributes) {
        stream << attribute;
    }

    // Put them together
    ZigbeeClusterLibrary::Frame frame;
    frame.header = header;
    frame.payload = payload;

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

ZigbeeClusterReply *ZigbeeCluster::createClusterReply(const ZigbeeNetworkRequest &request, ZigbeeClusterLibrary::Frame frame)
{
    ZigbeeClusterReply *zclReply = new ZigbeeClusterReply(request, frame, this);
    connect(zclReply, &ZigbeeClusterReply::finished, zclReply, &ZigbeeClusterReply::deleteLater);
    zclReply->m_transactionSequenceNumber = frame.header.transactionSequenceNumber;
    m_pendingReplies.insert(zclReply->transactionSequenceNumber(), zclReply);
    return zclReply;
}

ZigbeeNetworkRequest ZigbeeCluster::createGeneralRequest()
{
    // Build the request
    ZigbeeNetworkRequest request;
    request.setRequestId(m_network->generateSequenceNumber());
    request.setDestinationAddressMode(Zigbee::DestinationAddressModeShortAddress);
    request.setDestinationShortAddress(m_node->shortAddress());
    request.setProfileId(Zigbee::ZigbeeProfileHomeAutomation); // Note: in Zigbee 3.0 this is the Application Profile (0x0104)
    request.setClusterId(m_clusterId);
    request.setSourceEndpoint(0x01);
    request.setDestinationEndpoint(m_endpoint->endpointId());
    request.setRadius(10);
    request.setTxOptions(Zigbee::ZigbeeTxOptions(Zigbee::ZigbeeTxOptionAckTransmission));
    return request;
}

bool ZigbeeCluster::verifyNetworkError(ZigbeeClusterReply *zclReply, ZigbeeNetworkReply *networkReply)
{
    bool success = false;
    switch (networkReply->error()) {
    case ZigbeeNetworkReply::ErrorNoError:
        // The request has been transported successfully to he destination, now
        // wait for the expected indication or check if we already recieved it
        zclReply->m_apsConfirmReceived = true;
        zclReply->m_zigbeeApsStatus = networkReply->zigbeeApsStatus();
        success = true;
        break;
    case ZigbeeNetworkReply::ErrorInterfaceError:
        zclReply->m_error = ZigbeeClusterReply::ErrorInterfaceError;
        break;
    case ZigbeeNetworkReply::ErrorNetworkOffline:
        zclReply->m_error = ZigbeeClusterReply::ErrorNetworkOffline;
        break;
    case ZigbeeNetworkReply::ErrorZigbeeApsStatusError:
        zclReply->m_error = ZigbeeClusterReply::ErrorZigbeeApsStatusError;
        zclReply->m_apsConfirmReceived = true;
        zclReply->m_zigbeeApsStatus = networkReply->zigbeeApsStatus();
        break;
    }

    return success;
}

void ZigbeeCluster::finishZclReply(ZigbeeClusterReply *zclReply)
{
    m_pendingReplies.remove(zclReply->transactionSequenceNumber());
    zclReply->finished();
}

void ZigbeeCluster::processApsDataIndication(QByteArray payload)
{
    ZigbeeClusterLibrary::Frame frame = ZigbeeClusterLibrary::parseFrameData(payload);
    qCDebug(dcZigbeeClusterLibrary()) << this << "received data indication" << frame;

    if (m_pendingReplies.contains(frame.header.transactionSequenceNumber)) {
        ZigbeeClusterReply *reply = m_pendingReplies.value(frame.header.transactionSequenceNumber);
        reply->m_responseData = payload;
        reply->m_responseFrame = frame;
        reply->m_zclIndicationReceived = true;

        if (reply->isComplete())
            finishZclReply(reply);

        return;
    }

    // FIXME: increase transaction sequence number
    qCWarning(dcZigbeeNode()) << m_node << m_endpoint << this << "Unhandled ZCL indication" << ZigbeeUtils::convertByteArrayToHexString(payload);
}

QDebug operator<<(QDebug debug, ZigbeeCluster *cluster)
{
    debug.nospace().noquote() << "ZigbeeCluster("
                              << ZigbeeUtils::convertUint16ToHexString(static_cast<quint16>(cluster->clusterId())) << ", "
                              << cluster->clusterName() << ", "
                              << cluster->direction()
                              << ")";
    return debug.space();
}

QDebug operator<<(QDebug debug, const ZigbeeClusterAttributeReport &attributeReport)
{
    debug.nospace().noquote() << "AttributeReport("
                              << attributeReport.clusterId << ", "
                              << attributeReport.attributeId << ", "
                              << attributeReport.attributeStatus << ", "
                              << attributeReport.dataType << ", "
                              << attributeReport.data << ", "
                              << ")";

    return debug.space();
}

