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
#include "zigbeedeviceprofile.h"
#include "zigbeeutils.h"

#include <QDataStream>
#include <QPointer>

ZigbeeDeviceObject::ZigbeeDeviceObject(ZigbeeNetwork *network, ZigbeeNode *node, QObject *parent) :
    QObject(parent),
    m_network(network),
    m_node(node)
{

}

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::requestNetworkAddress()
{
    qCDebug(dcZigbeeDeviceObject()) << "Request network address from" << m_node;

    // Build APS request and make ieee address request
    ZigbeeNetworkRequest request;
    request.setRequestId(m_network->generateSequenceNumber());
    request.setDestinationAddressMode(Zigbee::DestinationAddressModeIeeeAddress);
    request.setDestinationIeeeAddress(m_node->extendedAddress());
    request.setDestinationEndpoint(0); // ZDO
    request.setProfileId(Zigbee::ZigbeeProfileDevice); // ZDP
    request.setClusterId(ZigbeeDeviceProfile::NetworkAddressRequest);
    request.setSourceEndpoint(0); // ZDO

    // Generate a new transaction sequence number for this device object
    quint8 transactionSequenceNumber = m_transactionSequenceNumber++;

    // Build ZDO frame
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << transactionSequenceNumber;
    stream << m_node->extendedAddress().toUInt64();
    stream << static_cast<quint8>(0); // Note: 0 = single device response, 1 = extended request
    stream << static_cast<quint8>(0); // Start index

    // Set the ZDO frame as APS request payload
    request.setAsdu(asdu);

    // Create the device object reply and wait for the response indication
    ZigbeeDeviceObjectReply *zdoReply = createZigbeeDeviceObjectReply(request, transactionSequenceNumber);

    // Send the request, on finished read the confirm information
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, zdoReply, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
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

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::requestIeeeAddress()
{
    qCDebug(dcZigbeeDeviceObject()) << "Request IEEE address from" << m_node;

    // Build APS request
    ZigbeeNetworkRequest request = buildZdoRequest(ZigbeeDeviceProfile::IeeeAddressRequest);

    // Generate a new transaction sequence number for this device object
    quint8 transactionSequenceNumber = m_transactionSequenceNumber++;

    // Build ZDO frame
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << transactionSequenceNumber << m_node->shortAddress();
    stream << static_cast<quint8>(0); // Note: 0 = single device response, 1 = extended request
    stream << static_cast<quint8>(0); // Start index

    // Set the ZDO frame as APS request payload
    request.setAsdu(asdu);

    // Create the device object reply and wait for the response indication
    ZigbeeDeviceObjectReply *zdoReply = createZigbeeDeviceObjectReply(request, transactionSequenceNumber);

    // Send the request, on finished read the confirm information
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, zdoReply, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
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
    connect(networkReply, &ZigbeeNetworkReply::finished, zdoReply, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
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
    connect(networkReply, &ZigbeeNetworkReply::finished, zdoReply, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
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
    connect(networkReply, &ZigbeeNetworkReply::finished, zdoReply, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
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
    connect(networkReply, &ZigbeeNetworkReply::finished, zdoReply, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
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

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::requestBindGroupAddress(quint8 sourceEndpointId, quint16 clusterId, quint16 destinationAddress)
{
    qCDebug(dcZigbeeDeviceObject()) << "Requesting bind group address from" << m_node << "endpoint" << ZigbeeUtils::convertByteToHexString(sourceEndpointId)
                                    << static_cast<ZigbeeClusterLibrary::ClusterId>(clusterId) << "to group" << ZigbeeUtils::convertUint16ToHexString(destinationAddress);

    // Build APS request
    ZigbeeNetworkRequest request = buildZdoRequest(ZigbeeDeviceProfile::BindRequest);

    // Generate a new transaction sequence number for this device object
    quint8 transactionSequenceNumber = m_transactionSequenceNumber++;

    // Build ZDO frame
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << transactionSequenceNumber;
    stream << m_node->extendedAddress().toUInt64();
    stream << sourceEndpointId;
    stream << clusterId;
    stream << static_cast<quint8>(Zigbee::DestinationAddressModeGroup);
    stream << destinationAddress;

    // Set the ZDO frame as APS request payload
    request.setAsdu(asdu);

    // Create the device object reply and wait for the response indication
    ZigbeeDeviceObjectReply *zdoReply = createZigbeeDeviceObjectReply(request, transactionSequenceNumber);

    // Send the request, on finished read the confirm information
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, zdoReply, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
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

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::requestBindIeeeAddress(quint8 sourceEndpointId, quint16 clusterId, const ZigbeeAddress &destinationIeeeAddress, quint8 destinationEndpointId)
{
    qCDebug(dcZigbeeDeviceObject()) << "Request bind IEEE address from" << m_node << "endpoint" << ZigbeeUtils::convertByteToHexString(sourceEndpointId)
                                    << static_cast<ZigbeeClusterLibrary::ClusterId>(clusterId) << "to" << destinationIeeeAddress.toString() << "endpoint"
                                    << ZigbeeUtils::convertByteToHexString(destinationEndpointId);

    // Build APS request
    ZigbeeNetworkRequest request = buildZdoRequest(ZigbeeDeviceProfile::BindRequest);

    // Generate a new transaction sequence number for this device object
    quint8 transactionSequenceNumber = m_transactionSequenceNumber++;

    // Build ZDO frame
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << transactionSequenceNumber;
    stream << m_node->extendedAddress().toUInt64();
    stream << sourceEndpointId;
    stream << clusterId;
    stream << static_cast<quint8>(Zigbee::DestinationAddressModeIeeeAddress);
    stream << destinationIeeeAddress.toUInt64();
    stream << destinationEndpointId;

    // Set the ZDO frame as APS request payload
    request.setAsdu(asdu);

    // Create the device object reply and wait for the response indication
    ZigbeeDeviceObjectReply *zdoReply = createZigbeeDeviceObjectReply(request, transactionSequenceNumber);

    // Send the request, on finished read the confirm information
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, zdoReply, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
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

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::requestUnbind(const ZigbeeDeviceProfile::BindingTableListRecord &bindingRecord)
{
    qCDebug(dcZigbeeDeviceObject()) << "Request unbind" << m_node << bindingRecord;

    // Build APS request
    ZigbeeNetworkRequest request = buildZdoRequest(ZigbeeDeviceProfile::UnbindRequest);

    // Generate a new transaction sequence number for this device object
    quint8 transactionSequenceNumber = m_transactionSequenceNumber++;

    // Build ZDO frame
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << transactionSequenceNumber;
    stream << bindingRecord.sourceAddress.toUInt64();
    stream << bindingRecord.sourceEndpoint;
    stream << bindingRecord.clusterId;
    stream << static_cast<quint8>(bindingRecord.destinationAddressMode);
    if (bindingRecord.destinationAddressMode == Zigbee::DestinationAddressModeGroup) {
        stream << bindingRecord.destinationAddressShort;
    } else {
        stream << bindingRecord.destinationAddress.toUInt64();
        stream << bindingRecord.destinationEndpoint;
    }

    // Set the ZDO frame as APS request payload
    request.setAsdu(asdu);

    // Create the device object reply and wait for the response indication
    ZigbeeDeviceObjectReply *zdoReply = createZigbeeDeviceObjectReply(request, transactionSequenceNumber);

    // Send the request, on finished read the confirm information
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, zdoReply, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
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

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::requestBindRegister(const ZigbeeAddress &ieeeAddress)
{
    qCDebug(dcZigbeeDeviceObject()) << "Request bind register" << m_node << ieeeAddress.toString();

    // Build APS request
    ZigbeeNetworkRequest request = buildZdoRequest(ZigbeeDeviceProfile::BindRegisterRequest);

    // Generate a new transaction sequence number for this device object
    quint8 transactionSequenceNumber = m_transactionSequenceNumber++;

    // Build ZDO frame
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << transactionSequenceNumber;
    stream << ieeeAddress.toUInt64();

    // Set the ZDO frame as APS request payload
    request.setAsdu(asdu);

    // Create the device object reply and wait for the response indication
    ZigbeeDeviceObjectReply *zdoReply = createZigbeeDeviceObjectReply(request, transactionSequenceNumber);

    // Send the request, on finished read the confirm information
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, zdoReply, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
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
    connect(networkReply, &ZigbeeNetworkReply::finished, zdoReply, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
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

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::requestMgmtLqi(quint8 startIndex)
{
    qCDebug(dcZigbeeDeviceObject()) << "Request lqi table from" << m_node << "start index" << startIndex;

    // Build APS request
    ZigbeeNetworkRequest request = buildZdoRequest(ZigbeeDeviceProfile::MgmtLqiRequest);

    // Generate a new transaction sequence number for this device object
    quint8 transactionSequenceNumber = m_transactionSequenceNumber++;

    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << transactionSequenceNumber << startIndex;

    // Set the ZDO frame as APS request payload
    request.setAsdu(asdu);

    // Create the device object reply and wait for the response indication
    ZigbeeDeviceObjectReply *zdoReply = createZigbeeDeviceObjectReply(request, transactionSequenceNumber);

    // Send the request, on finished read the confirm information
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, zdoReply, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
            finishZdoReply(zdoReply);
            return;
        }

        // The request was successfully sent to the device
        // Now check if the expected indication response received already
        if (zdoReply->isComplete()) {
            qCDebug(dcZigbeeDeviceObject()) << "Successfully received response for" << static_cast<ZigbeeDeviceProfile::ZdoCommand>(networkReply->request().clusterId());
            finishZdoReply(zdoReply);
            return;
        }
        // We received the confirmation but not yet the indication
    });

    return zdoReply;
}

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::requestMgmtBind(quint8 startIndex)
{
    qCDebug(dcZigbeeDeviceObject()) << "Request management bind table from" << m_node << "start index" << startIndex;

    // Build APS request
    ZigbeeNetworkRequest request = buildZdoRequest(ZigbeeDeviceProfile::MgmtBindRequest);

    // Generate a new transaction sequence number for this device object
    quint8 transactionSequenceNumber = m_transactionSequenceNumber++;

    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << transactionSequenceNumber << startIndex;

    // Set the ZDO frame as APS request payload
    request.setAsdu(asdu);

    // Create the device object reply and wait for the response indication
    ZigbeeDeviceObjectReply *zdoReply = createZigbeeDeviceObjectReply(request, transactionSequenceNumber);

    // Send the request, on finished read the confirm information
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, zdoReply, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
            finishZdoReply(zdoReply);
            return;
        }

        // The request was successfully sent to the device
        // Now check if the expected indication response received already
        if (zdoReply->isComplete()) {
            qCDebug(dcZigbeeDeviceObject()) << "Successfully received response for" << static_cast<ZigbeeDeviceProfile::ZdoCommand>(networkReply->request().clusterId());
            finishZdoReply(zdoReply);
            return;
        }
        // We received the confirmation but not yet the indication
    });

    return zdoReply;
}

ZigbeeDeviceObjectReply *ZigbeeDeviceObject::requestMgmtPermitJoining(quint8 duration)
{
    qCDebug(dcZigbeeDeviceObject()) << "Request management permit join from" << m_node << "with duration" << duration << "s";

    // Build APS request
    ZigbeeNetworkRequest request = buildZdoRequest(ZigbeeDeviceProfile::MgmtPermitJoinRequest);

    // Generate a new transaction sequence number for this device object
    quint8 transactionSequenceNumber = m_transactionSequenceNumber++;

    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << duration << static_cast<quint8>(0x01); // TC Significance always 1

    // Set the ZDO frame as APS request payload
    request.setAsdu(asdu);

    // Create the device object reply and wait for the response indication
    ZigbeeDeviceObjectReply *zdoReply = createZigbeeDeviceObjectReply(request, transactionSequenceNumber);
    qCDebug(dcZigbeeDeviceObject()) << "Sending request" << request;
    // Send the request, on finished read the confirm information
    ZigbeeNetworkReply *networkReply = m_network->sendRequest(request);
    connect(networkReply, &ZigbeeNetworkReply::finished, this, [this, networkReply, zdoReply](){
        if (!verifyNetworkError(zdoReply, networkReply)) {
            finishZdoReply(zdoReply);
            return;
        }

        // The request was successfully sent to the device
        // Now check if the expected indication response received already
        if (zdoReply->isComplete()) {
            qCDebug(dcZigbeeDeviceObject()) << "Successfully received response for" << static_cast<ZigbeeDeviceProfile::ZdoCommand>(networkReply->request().clusterId());
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
    zdoReply->m_expectedResponse =  static_cast<ZigbeeDeviceProfile::ZdoCommand>(request.clusterId() | 0x8000);
    zdoReply->m_transactionSequenceNumber = transactionSequenceNumber;
    connect(zdoReply, &ZigbeeDeviceObjectReply::finished, this, [=](){
        zdoReply->deleteLater();
        m_pendingReplies.remove(transactionSequenceNumber);
        if (zdoReply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeDeviceObject()) << "ZDO request error for TSN:" << transactionSequenceNumber;
        }
    });
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
        if (!zdoReply->m_zdpIndicationReceived) {
            zdoReply->m_timeoutTimer.start();
        }
        success = true;
        break;
    case ZigbeeNetworkReply::ErrorInterfaceError:
        zdoReply->m_error = ZigbeeDeviceObjectReply::ErrorInterfaceError;
        qCWarning(dcZigbeeDeviceObject()) << "Failed to send request" << static_cast<ZigbeeDeviceProfile::ZdoCommand>(networkReply->request().clusterId()) << m_node << networkReply->error();
        break;
    case ZigbeeNetworkReply::ErrorTimeout:
        zdoReply->m_error = ZigbeeDeviceObjectReply::ErrorTimeout;
        qCWarning(dcZigbeeDeviceObject()) << "Failed to send request" << static_cast<ZigbeeDeviceProfile::ZdoCommand>(networkReply->request().clusterId()) << m_node << networkReply->error();
        break;
    case ZigbeeNetworkReply::ErrorNetworkOffline:
        zdoReply->m_error = ZigbeeDeviceObjectReply::ErrorNetworkOffline;
        qCWarning(dcZigbeeDeviceObject()) << "Failed to send request" << static_cast<ZigbeeDeviceProfile::ZdoCommand>(networkReply->request().clusterId()) << m_node << networkReply->error();
        break;
    case ZigbeeNetworkReply::ErrorZigbeeMacStatusError:
        zdoReply->setZigbeeMacLayerStatus(networkReply->zigbeeMacStatus());
        qCWarning(dcZigbeeDeviceObject()) << "Failed to send request" << static_cast<ZigbeeDeviceProfile::ZdoCommand>(networkReply->request().clusterId()) << m_node << networkReply->zigbeeMacStatus();
        zdoReply->m_apsConfirmReceived = true;
        break;
    case ZigbeeNetworkReply::ErrorZigbeeApsStatusError:
        zdoReply->m_apsConfirmReceived = true;
        zdoReply->setZigbeeApsStatus(networkReply->zigbeeApsStatus());
        qCWarning(dcZigbeeDeviceObject()) << "Failed to send request" << static_cast<ZigbeeDeviceProfile::ZdoCommand>(networkReply->request().clusterId()) << m_node << networkReply->zigbeeApsStatus();
        zdoReply->m_apsConfirmReceived = true;
        break;
    case ZigbeeNetworkReply::ErrorZigbeeNwkStatusError:
        zdoReply->m_apsConfirmReceived = true;
        zdoReply->setZigbeeNwkLayerStatus(networkReply->zigbeeNwkStatus());
        qCWarning(dcZigbeeDeviceObject()) << "Failed to send request" << static_cast<ZigbeeDeviceProfile::ZdoCommand>(networkReply->request().clusterId()) << m_node << networkReply->zigbeeNwkStatus();
        zdoReply->m_apsConfirmReceived = true;
        break;
    }

    return success;
}

void ZigbeeDeviceObject::finishZdoReply(ZigbeeDeviceObjectReply *zdoReply)
{
    // Note: here all layer errors have already been set
    switch(zdoReply->error()) {
    case ZigbeeDeviceObjectReply::ErrorNoError:
        qCDebug(dcZigbeeDeviceObject()) << "Reply finished successfully" << zdoReply->request();
        break;
    case ZigbeeDeviceObjectReply::ErrorZigbeeDeviceObjectStatusError:
        qCWarning(dcZigbeeDeviceObject()) << "Reply finished with error" << zdoReply->request() << zdoReply->zigbeeDeviceObjectStatus();
        break;
    default:
        break;
    }

    zdoReply->m_timeoutTimer.stop();
    zdoReply->finished();
}

void ZigbeeDeviceObject::processApsDataIndication(const Zigbee::ApsdeDataIndication &indication)
{
    // Check if we have a waiting ZDO reply for this data
    ZigbeeDeviceProfile::Adpu asdu = ZigbeeDeviceProfile::parseAdpu(indication.asdu);
    ZigbeeDeviceObjectReply *zdoReply = m_pendingReplies.value(asdu.transactionSequenceNumber);
    if (zdoReply && indication.clusterId == (zdoReply->request().clusterId() | 0x8000)) {
        zdoReply->m_responseData = indication.asdu;
        zdoReply->m_responseAdpu = asdu;
        zdoReply->setZigbeeDeviceObjectStatus(asdu.status);
        zdoReply->m_zdpIndicationReceived = true;
        if (zdoReply->isComplete()) {
            finishZdoReply(zdoReply);
        }
        return;
    }

    qCWarning(dcZigbeeDeviceObject()) << "Unhandled ZDO indication" << m_node << indication << asdu;
}
