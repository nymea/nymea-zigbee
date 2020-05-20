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

#include "zigbeedeviceobject.h"
#include "zigbeenetwork.h"
#include "loggingcategory.h"

#include <QDataStream>

ZigbeeDeviceObject::ZigbeeDeviceObject(ZigbeeNetwork *network, ZigbeeNode *node, QObject *parent) :
    QObject(parent),
    m_network(network),
    m_node(node)
{

}

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::requestNodeDescriptor()
{    
    qCDebug(dcZigbeeDeviceObject()) << "Request node descriptor from" << m_node;

    // Build APS request
    ZigbeeNetworkRequest request = buildZdoRequest(ZigbeeDeviceProfile::NodeDescriptorRequest);

    // Generate a new transaction sequence number for this device object
    quint8 transactionSequenceNumber = m_transactionSequenceNumber++;

    // Build ZDO frame
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << transactionSequenceNumber << m_node->shortAddress();

    // Set the ZDO frame as APS request payload
    request.setAsdu(asdu);

    // Create the device object reply and wait for the response indication
    ZigbeeDeviceObjectReply *zdoReply = createZigbeeDeviceObjectReply(request, transactionSequenceNumber);

    // Send the request, on finished read the confirm information
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, this, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
            qCWarning(dcZigbeeDeviceObject()) << "Failed to send request"
                                              << static_cast<ZigbeeDeviceProfile::ZdoCommand>(networkReply->request().clusterId())
                                              << m_node << networkReply->error()
                                              << networkReply->zigbeeApsStatus();
            finishZdoReply(zdoReply);
            return;
        }

        // The request was successfully sent to the device
        // Now check if the expected indication response received already
        if (zdoReply->isComplete()) {
            finishZdoReply(zdoReply);
            return;
        }

        // We received the confirmation but not yet the indication
    });

    return zdoReply;
}

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::requestPowerDescriptor()
{
    qCDebug(dcZigbeeDeviceObject()) << "Request power descriptor from" << m_node;

    // Build APS request
    ZigbeeNetworkRequest request = buildZdoRequest(ZigbeeDeviceProfile::PowerDescriptorRequest);

    // Generate a new transaction sequence number for this device object
    quint8 transactionSequenceNumber = m_transactionSequenceNumber++;

    // Build ZDO frame
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << transactionSequenceNumber << m_node->shortAddress();

    // Set the ZDO frame as APS request payload
    request.setAsdu(asdu);

    // Create the device object reply and wait for the response indication
    ZigbeeDeviceObjectReply *zdoReply = createZigbeeDeviceObjectReply(request, transactionSequenceNumber);

    // Send the request, on finished read the confirm information
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, this, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
            qCWarning(dcZigbeeDeviceObject()) << "Failed to send request"
                                              << static_cast<ZigbeeDeviceProfile::ZdoCommand>(networkReply->request().clusterId())
                                              << m_node << networkReply->error()
                                              << networkReply->zigbeeApsStatus();
            finishZdoReply(zdoReply);
            return;
        }

        // The request was successfully sent to the device
        // Now check if the expected indication response received already
        if (zdoReply->isComplete()) {
            finishZdoReply(zdoReply);
            return;
        }
        // We received the confirmation but not yet the indication
    });

    return zdoReply;
}

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::requestActiveEndpoints()
{
    qCDebug(dcZigbeeDeviceObject()) << "Request active endpoints from" << m_node;

    // Build APS request
    ZigbeeNetworkRequest request = buildZdoRequest(ZigbeeDeviceProfile::ActiveEndpointsRequest);

    // Generate a new transaction sequence number for this device object
    quint8 transactionSequenceNumber = m_transactionSequenceNumber++;

    // Build ZDO frame
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << transactionSequenceNumber << m_node->shortAddress();

    // Set the ZDO frame as APS request payload
    request.setAsdu(asdu);

    // Create the device object reply and wait for the response indication
    ZigbeeDeviceObjectReply *zdoReply = createZigbeeDeviceObjectReply(request, transactionSequenceNumber);

    // Send the request, on finished read the confirm information
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, this, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
            qCWarning(dcZigbeeDeviceObject()) << "Failed to send request"
                                              << static_cast<ZigbeeDeviceProfile::ZdoCommand>(networkReply->request().clusterId())
                                              << m_node << networkReply->error()
                                              << networkReply->zigbeeApsStatus();
            finishZdoReply(zdoReply);
            return;
        }

        // The request was successfully sent to the device
        // Now check if the expected indication response received already
        if (zdoReply->isComplete()) {
            finishZdoReply(zdoReply);
            return;
        }
        // We received the confirmation but not yet the indication
    });

    return zdoReply;
}

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::requestSimpleDescriptor(quint8 endpointId)
{
    qCDebug(dcZigbeeDeviceObject()) << "Request simple descriptor from" << m_node << "endpoint" << endpointId;

    // Build APS request
    ZigbeeNetworkRequest request = buildZdoRequest(ZigbeeDeviceProfile::SimpleDescriptorRequest);

    // Generate a new transaction sequence number for this device object
    quint8 transactionSequenceNumber = m_transactionSequenceNumber++;

    // Build ZDO frame
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << transactionSequenceNumber << request.destinationShortAddress() << endpointId;

    // Set the ZDO frame as APS request payload
    request.setAsdu(asdu);

    // Create the device object reply and wait for the response indication
    ZigbeeDeviceObjectReply *zdoReply = createZigbeeDeviceObjectReply(request, transactionSequenceNumber);

    // Send the request, on finished read the confirm information
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, this, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
            qCWarning(dcZigbeeDeviceObject()) << "Failed to send request"
                                              << static_cast<ZigbeeDeviceProfile::ZdoCommand>(networkReply->request().clusterId())
                                              << m_node << networkReply->error()
                                              << networkReply->zigbeeApsStatus();
            finishZdoReply(zdoReply);
            return;
        }

        // The request was successfully sent to the device
        // Now check if the expected indication response received already
        if (zdoReply->isComplete()) {
            finishZdoReply(zdoReply);
            return;
        }
        // We received the confirmation but not yet the indication
    });

    return zdoReply;
}

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::requestMgmtLeaveNetwork(bool rejoin, bool removeChildren)
{
    qCDebug(dcZigbeeDeviceObject()) << "Request management leave network from" << m_node << "rejoin" << rejoin << "remove children" << removeChildren;

    // Build APS request
    ZigbeeNetworkRequest request = buildZdoRequest(ZigbeeDeviceProfile::MgmtLeaveRequest);

    // Generate a new transaction sequence number for this device object
    quint8 transactionSequenceNumber = m_transactionSequenceNumber++;

    // Build ZDO frame
    quint8 leaveFlag = 0;
    if (rejoin) {
        leaveFlag |= 0x01;
    }

    if (removeChildren) {
        leaveFlag |= 0x02;
    }

    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << transactionSequenceNumber << m_node->extendedAddress().toUInt64() << leaveFlag;

    // Set the ZDO frame as APS request payload
    request.setAsdu(asdu);

    // Create the device object reply and wait for the response indication
    ZigbeeDeviceObjectReply *zdoReply = createZigbeeDeviceObjectReply(request, transactionSequenceNumber);

    // Send the request, on finished read the confirm information
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, this, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
            qCWarning(dcZigbeeDeviceObject()) << "Failed to send request"
                                              << static_cast<ZigbeeDeviceProfile::ZdoCommand>(networkReply->request().clusterId())
                                              << m_node << networkReply->error()
                                              << networkReply->zigbeeApsStatus();
            finishZdoReply(zdoReply);
            return;
        }

        // The request was successfully sent to the device
        // Now check if the expected indication response received already
        if (zdoReply->isComplete()) {
            finishZdoReply(zdoReply);
            return;
        }
        // We received the confirmation but not yet the indication
    });

    return zdoReply;
}

ZigbeeNetworkRequest ZigbeeDeviceObject::buildZdoRequest(quint16 zdoRequest)
{
    ZigbeeNetworkRequest request;
    request.setRequestId(m_network->generateSequenceNumber());
    request.setDestinationAddressMode(Zigbee::DestinationAddressModeShortAddress);
    request.setDestinationShortAddress(m_node->shortAddress());
    request.setDestinationEndpoint(0); // ZDO
    request.setProfileId(Zigbee::ZigbeeProfileDevice); // ZDP
    request.setClusterId(zdoRequest);
    request.setSourceEndpoint(0); // ZDO
    return request;
}

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::createZigbeeDeviceObjectReply(const ZigbeeNetworkRequest &request, quint8 transactionSequenceNumber)
{
    ZigbeeDeviceObjectReply *zdoReply = new ZigbeeDeviceObjectReply(request, this);
    connect(zdoReply, &ZigbeeDeviceObjectReply::finished, zdoReply, &ZigbeeDeviceObjectReply::deleteLater);
    zdoReply->m_expectedResponse =  static_cast<ZigbeeDeviceProfile::ZdoCommand>(request.clusterId() | 0x8000);
    zdoReply->m_transactionSequenceNumber = transactionSequenceNumber;
    m_pendingReplies.insert(transactionSequenceNumber, zdoReply);
    return zdoReply;
}

bool ZigbeeDeviceObject::verifyNetworkError(ZigbeeDeviceObjectReply *zdoReply, ZigbeeNetworkReply *networkReply)
{
    bool success = false;
    switch (networkReply->error()) {
    case ZigbeeNetworkReply::ErrorNoError:
        // The request has been transported successfully to he destination, now
        // wait for the expected indication or check if we already recieved it
        zdoReply->m_apsConfirmReceived = true;
        zdoReply->m_zigbeeApsStatus = networkReply->zigbeeApsStatus();
        success = true;
        break;
    case ZigbeeNetworkReply::ErrorInterfaceError:
        zdoReply->m_error = ZigbeeDeviceObjectReply::ErrorInterfaceError;
        break;
    case ZigbeeNetworkReply::ErrorNetworkOffline:
        zdoReply->m_error = ZigbeeDeviceObjectReply::ErrorNetworkOffline;
        break;
    case ZigbeeNetworkReply::ErrorZigbeeApsStatusError:
        zdoReply->m_error = ZigbeeDeviceObjectReply::ErrorZigbeeApsStatusError;
        zdoReply->m_apsConfirmReceived = true;
        zdoReply->m_zigbeeApsStatus = networkReply->zigbeeApsStatus();
        break;
    }

    return success;
}

void ZigbeeDeviceObject::finishZdoReply(ZigbeeDeviceObjectReply *zdoReply)
{
    m_pendingReplies.remove(zdoReply->transactionSequenceNumber());
    zdoReply->finished();
}

void ZigbeeDeviceObject::processApsDataIndication(quint8 destinationEndpoint, quint8 sourceEndpoint, quint16 clusterId, QByteArray payload, quint8 lqi, qint8 rssi)
{
    Q_UNUSED(destinationEndpoint)
    Q_UNUSED(sourceEndpoint)
    Q_UNUSED(clusterId)
    Q_UNUSED(lqi)
    Q_UNUSED(rssi)

    // Check if we have a waiting ZDO reply for this data
    ZigbeeDeviceProfile::Adpu asdu = ZigbeeDeviceProfile::parseAdpu(payload);
    ZigbeeDeviceObjectReply *zdoReply = m_pendingReplies.value(asdu.transactionSequenceNumber);
    if (zdoReply && clusterId == (zdoReply->request().clusterId() | 0x8000)) {
        zdoReply->m_responseData = payload;
        zdoReply->m_responseAdpu = asdu;
        zdoReply->m_zdpIndicationReceived = true;
        if (zdoReply->isComplete()) {
            finishZdoReply(zdoReply);
        }
        return;
    }

    qCWarning(dcZigbeeDeviceObject()) << m_node << "unhandled ZDO indication";
}
