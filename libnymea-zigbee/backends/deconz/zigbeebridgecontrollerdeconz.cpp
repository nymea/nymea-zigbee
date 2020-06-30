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
#include "loggingcategory.h"
#include "zigbeechannelmask.h"
#include "zdo/zigbeedeviceprofile.h"
#include "zigbeebridgecontrollerdeconz.h"

#include <QDataStream>

ZigbeeBridgeControllerDeconz::ZigbeeBridgeControllerDeconz(QObject *parent) :
    ZigbeeBridgeController(parent)
{
    m_interface = new ZigbeeInterfaceDeconz(this);
    connect(m_interface, &ZigbeeInterfaceDeconz::availableChanged, this, &ZigbeeBridgeControllerDeconz::onInterfaceAvailableChanged);
    connect(m_interface, &ZigbeeInterfaceDeconz::packageReceived, this, &ZigbeeBridgeControllerDeconz::onInterfacePackageReceived);

    m_watchdogTimer = new QTimer(this);
    m_watchdogTimer->setSingleShot(false);
    m_watchdogTimer->setInterval(m_watchdogResetTimout * 1000);
    connect(m_watchdogTimer, &QTimer::timeout, this, &ZigbeeBridgeControllerDeconz::resetControllerWatchdog);
}

ZigbeeBridgeControllerDeconz::~ZigbeeBridgeControllerDeconz()
{
    qCDebug(dcZigbeeController()) << "Destroy controller";
}

DeconzNetworkConfiguration ZigbeeBridgeControllerDeconz::networkConfiguration() const
{
    return m_networkConfiguration;
}

void ZigbeeBridgeControllerDeconz::setFirmwareVersionString(const QString &firmwareVersion)
{
    m_firmwareVersion = firmwareVersion;
    emit firmwareVersionChanged(m_firmwareVersion);
}

Deconz::NetworkState ZigbeeBridgeControllerDeconz::networkState() const
{
    return m_networkState;
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestVersion()
{

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandVersion);
    stream << static_cast<quint8>(0); // SQN will be generated right before sending
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(5); // Frame length

    return createReply(Deconz::CommandVersion, "Request controller version", message, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestDeviceState()
{
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandDeviceState);
    stream << static_cast<quint8>(0); // SQN will be generated right before sending
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(8); // Frame length
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint8>(0); // Reserverd

    return createReply(Deconz::CommandDeviceState, "Request controller device state", message, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestReadParameter(Deconz::Parameter parameter)
{
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandReadParameter);
    stream << static_cast<quint8>(0); // SQN will be generated right before sending
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(8); // Frame length 7 + 1 payload
    stream << static_cast<quint16>(1); // Payload length
    stream << static_cast<quint8>(parameter);

    return createReply(Deconz::CommandReadParameter, "Request controller read parameter", message, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestWriteParameter(Deconz::Parameter parameter, const QByteArray &data)
{
    quint16 payloadLength = static_cast<quint16>(1 + data.length());
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandWriteParameter);
    stream << static_cast<quint8>(0); // SQN will be generated right before sending
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(7 + payloadLength); // Frame length 7 + payload length
    stream << static_cast<quint16>(payloadLength); // 1 parameter + parameter data length
    stream << static_cast<quint8>(parameter);
    for (int i = 0; i < data.length(); i++) {
        stream << static_cast<quint8>(data.at(i));
    }

    return createReply(Deconz::CommandWriteParameter, "Request controller write parameter", message, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestChangeNetworkState(Deconz::NetworkState networkState)
{
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandChangeNetworkState);
    stream << static_cast<quint8>(0); // SQN will be generated right before sending
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(6); // Frame length
    stream << static_cast<quint8>(networkState);

    return createReply(Deconz::CommandChangeNetworkState, "Request controller write parameter", message, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestReadReceivedDataIndication(Deconz::SourceAddressMode sourceAddressMode)
{
    quint16 payloadLength = 0;
    if (sourceAddressMode != Deconz::SourceAddressModeNone) {
        payloadLength = 1;
    }

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandApsDataIndication);
    stream << static_cast<quint8>(0); // SQN will be generated right before sending
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(7 + payloadLength); // Frame length + payload length
    stream << static_cast<quint16>(payloadLength); // payload length
    if (payloadLength > 0)
        stream << static_cast<quint8>(sourceAddressMode);

    return createReply(Deconz::CommandApsDataIndication, "Request read received data indication", message, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestQuerySendDataConfirm()
{
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandApsDataConfirm);
    stream << static_cast<quint8>(0); // SQN will be generated right before sending
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(7); // Frame length
    stream << static_cast<quint16>(0); // Payload length

    return createReply(Deconz::CommandApsDataConfirm, "Request query send data confirm", message, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestSendRequest(const ZigbeeNetworkRequest &request)
{
    ZigbeeInterfaceDeconzReply *interfaceReply = nullptr;
    switch (request.destinationAddressMode()) {
    case Zigbee::DestinationAddressModeGroup:
        interfaceReply = requestEnqueueSendDataGroup(request.requestId(), request.destinationShortAddress(),
                                                     request.profileId(), request.clusterId(),request.sourceEndpoint(),
                                                     request.asdu(), request.txOptions(), request.radius());
        break;
    case Zigbee::DestinationAddressModeShortAddress:
        interfaceReply = requestEnqueueSendDataShortAddress(request.requestId(), request.destinationShortAddress(),
                                                            request.destinationEndpoint(), request.profileId(), request.clusterId(),
                                                            request.sourceEndpoint(), request.asdu(), request.txOptions(), request.radius());
        break;
    case Zigbee::DestinationAddressModeIeeeAddress:
        interfaceReply = requestEnqueueSendDataIeeeAddress(request.requestId(), request.destinationIeeeAddress(),
                                                           request.destinationEndpoint(), request.profileId(), request.clusterId(),
                                                           request.sourceEndpoint(), request.asdu(), request.txOptions(), request.radius());
        break;
    }

    return interfaceReply;
}

void ZigbeeBridgeControllerDeconz::sendNextRequest()
{
    // Check if there is a reply request to send
    if (m_replyQueue.isEmpty())
        return;

    // Check if there is currently a running reply
    if (m_currentReply)
        return;

//    // FIXME: If the controler request queue is full, wait until it's free again
//    if (!m_apsFreeSlotsAvailable)
//        return;

    // Get the next reply, set the sequence number, send the request data over the interface and start waiting
    m_currentReply = m_replyQueue.dequeue();
    m_currentReply->setSequenceNumber(generateSequenceNumber());
    qCDebug(dcZigbeeController()) << "Send request" << m_currentReply;
    m_interface->sendPackage(m_currentReply->requestData());
    m_currentReply->m_timer->start();
}

quint8 ZigbeeBridgeControllerDeconz::generateSequenceNumber()
{
    return m_sequenceNumber++;
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::createReply(Deconz::Command command, const QString &requestName, const QByteArray &requestData, QObject *parent)
{
    // Create the reply
    ZigbeeInterfaceDeconzReply *reply = new ZigbeeInterfaceDeconzReply(command, parent);
    reply->m_requestName = requestName;
    reply->m_requestData = requestData;

    // Make sure we clean up on timeout
    connect(reply, &ZigbeeInterfaceDeconzReply::timeout, this, [this, reply](){
        qCWarning(dcZigbeeController()) << "Reply timeout" << reply;

        // Make sure we can send the next read confirm reply
        if (m_readConfirmReply == reply) {
            m_readConfirmReply = nullptr;
        }

        // Make sure we can send the next read indication reply
        if (m_readIndicationReply == reply) {
            m_readIndicationReply = nullptr;
        }

        // Note: send next reply with the finished signal
    });

    // Auto delete the object on finished
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, reply, [this, reply](){
        reply->deleteLater();

        if (m_currentReply == reply) {
            m_currentReply = nullptr;
            QMetaObject::invokeMethod(this, "sendNextRequest", Qt::QueuedConnection);
        }
    });

    // Enqueu this reply and send it once the current reply slot is free

    // If this is a data indication or a confirmation, prepend the reply since responses have higher priority than new requests
    if (command == Deconz::CommandApsDataConfirm || command == Deconz::CommandApsDataIndication) {
        m_replyQueue.prepend(reply);
        qCDebug(dcZigbeeController()) << "Prepend request to queue:" << reply->requestName();
    } else {
        m_replyQueue.enqueue(reply);
        qCDebug(dcZigbeeController()) << "Enqueue request:" << reply->requestName();
    }

    QMetaObject::invokeMethod(this, "sendNextRequest", Qt::QueuedConnection);
    return reply;
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestEnqueueSendDataGroup(quint8 requestId, quint16 groupAddress, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius)
{
//    quint8 sequenceNumber = generateSequenceNumber();
//    qCDebug(dcZigbeeController()) << "Request enqueue send data to group" << ZigbeeUtils::convertUint16ToHexString(groupAddress)
//                                  << "SQN:" << sequenceNumber
//                                  << static_cast<Zigbee::ZigbeeProfile>(profileId)
//                                  << ZigbeeUtils::convertUint16ToHexString(clusterId)
//                                  << ZigbeeUtils::convertByteToHexString(sourceEndpoint);

    Q_ASSERT_X(asdu.length() <= 127, "ASDU", "ASDU package length has to <= 127 bytes");

    // Note: 14 protocol bytes + asdu package length
    quint16 payloadLength = static_cast<quint16>(14 + asdu.length());

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandApsDataRequest);
    stream << static_cast<quint8>(0); // SQN will be generated right before sending
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(7 + payloadLength); // Frame length
    stream << payloadLength;
    stream << requestId;
    stream << static_cast<quint8>(0); // Flags
    stream << static_cast<quint8>(Zigbee::DestinationAddressModeGroup);
    stream << groupAddress;
    stream << profileId;
    stream << clusterId;
    stream << sourceEndpoint;
    stream << static_cast<quint16>(asdu.length());
    for (int i = 0; i < asdu.length(); i++) {
        stream << static_cast<quint8>(asdu.at(i));
    }
    stream << static_cast<quint8>(txOptions);
    stream << radius;

    return createReply(Deconz::CommandApsDataRequest, "Request enqueue send data to group", message, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestEnqueueSendDataShortAddress(quint8 requestId, quint16 shortAddress, quint8 destinationEndpoint, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius)
{
//    quint8 sequenceNumber = generateSequenceNumber();
//    qCDebug(dcZigbeeController()) << "Request enqueue send data to short address" << ZigbeeUtils::convertUint16ToHexString(shortAddress)
//                                  << "SQN:" << sequenceNumber
//                                  << ZigbeeUtils::convertByteToHexString(destinationEndpoint)
//                                  << static_cast<Zigbee::ZigbeeProfile>(profileId)
//                                  << ZigbeeUtils::convertUint16ToHexString(clusterId)
//                                  << ZigbeeUtils::convertByteToHexString(sourceEndpoint);

    Q_ASSERT_X(asdu.length() <= 127, "ASDU", "ASDU package length has to <= 127 bytes");

    // Note: 15 protocol bytes + asdu package length
    quint16 payloadLength = static_cast<quint16>(15 + asdu.length());

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandApsDataRequest);
    stream << static_cast<quint8>(0); // SQN will be generated right before sending
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(7 + payloadLength); // Frame length
    stream << payloadLength;
    stream << requestId;
    stream << static_cast<quint8>(0); // Flags
    stream << static_cast<quint8>(Zigbee::DestinationAddressModeShortAddress);
    stream << shortAddress << destinationEndpoint;
    stream << profileId;
    stream << clusterId;
    stream << sourceEndpoint;
    stream << static_cast<quint16>(asdu.length());
    for (int i = 0; i < asdu.length(); i++) {
        stream << static_cast<quint8>(asdu.at(i));
    }
    stream << static_cast<quint8>(txOptions); // TX Options: Use APS ACKs
    stream << radius;

    return createReply(Deconz::CommandApsDataRequest, "Request enqueue send data to short address", message, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestEnqueueSendDataIeeeAddress(quint8 requestId, ZigbeeAddress ieeeAddress, quint8 destinationEndpoint, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius)
{
//    quint8 sequenceNumber = generateSequenceNumber();
//    qCDebug(dcZigbeeController()) << "Request enqueue send data to IEEE address" << ieeeAddress.toString()
//                                  << "SQN:" << sequenceNumber
//                                  << ZigbeeUtils::convertByteToHexString(destinationEndpoint)
//                                  << profileId << clusterId
//                                  << ZigbeeUtils::convertByteToHexString(sourceEndpoint);

    Q_ASSERT_X(asdu.length() <= 127, "ZigbeeController", "ASDU package length has to be <= 127 bytes");

    // Note: 21 protocol bytes + asdu package length
    quint16 payloadLength = static_cast<quint16>(21 + asdu.length());

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandApsDataRequest);
    stream << static_cast<quint8>(0); // SQN will be generated right before sending
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(7 + payloadLength); // Frame length
    stream << payloadLength;
    stream << requestId;
    stream << static_cast<quint8>(0); // Flags
    stream << static_cast<quint8>(Zigbee::DestinationAddressModeIeeeAddress);
    stream << ieeeAddress.toUInt64() << destinationEndpoint;
    stream << profileId;
    stream << clusterId;
    stream << sourceEndpoint;
    stream << static_cast<quint16>(asdu.length());
    for (int i = 0; i < asdu.length(); i++) {
        stream << static_cast<quint8>(asdu.at(i));
    }
    stream << static_cast<quint8>(txOptions); // TX Options: Use APS ACKs
    stream << radius;

    return createReply(Deconz::CommandApsDataRequest, "Request enqueue send data to IEEE address", message, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::readNetworkParameters()
{
    qCDebug(dcZigbeeController()) << "Start reading network parameters";

    // This method reads all network configuration parameters sequentially. This method returns a reply which will be finished either
    // when a read request failes or all requests finished successfully.
    // If read request failes, this mehtod returns the status code of the failed request.

    // Create an independent reply for finishing the entire read sequence
    ZigbeeInterfaceDeconzReply *readNetworkParametersReply = new ZigbeeInterfaceDeconzReply(Deconz::CommandReadParameter, this);
    connect(readNetworkParametersReply, &ZigbeeInterfaceDeconzReply::finished, readNetworkParametersReply, &ZigbeeInterfaceDeconzReply::deleteLater, Qt::QueuedConnection);

    // Read MAC address of the bridge
    ZigbeeInterfaceDeconzReply *replyMacAddress = requestReadParameter(Deconz::ParameterMacAddress);
    connect(replyMacAddress, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyMacAddress](){
        if (replyMacAddress->statusCode() != Deconz::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Request" << "SQN:" << replyMacAddress->sequenceNumber() << replyMacAddress->command()
                                            << Deconz::ParameterMacAddress << "finished with error" << replyMacAddress->statusCode();

            readNetworkParametersReply->m_statusCode = replyMacAddress->statusCode();
            readNetworkParametersReply->finished();
            return;
        }
        QDataStream stream(replyMacAddress->responseData());
        stream.setByteOrder(QDataStream::LittleEndian);
        quint16 payloadLenght = 0; quint8 parameter = 0; quint64 macAddress = 0;
        stream >> payloadLenght >> parameter >> macAddress;

        m_networkConfiguration.ieeeAddress = ZigbeeAddress(macAddress);
        qCDebug(dcZigbeeController()) << "Request" << "SQN:" << replyMacAddress->sequenceNumber() << replyMacAddress->command()
                                      << static_cast<Deconz::Parameter>(parameter) << "finished successfully";
        qCDebug(dcZigbeeController()) << "IEEE address:" << m_networkConfiguration.ieeeAddress.toString();

        // Read PAN ID
        ZigbeeInterfaceDeconzReply *replyPanId = requestReadParameter(Deconz::ParameterPanId);
        connect(replyPanId, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyPanId](){
            if (replyPanId->statusCode() != Deconz::StatusCodeSuccess) {
                qCWarning(dcZigbeeController()) << "Request" << "SQN:" << replyPanId->sequenceNumber() << replyPanId->command()
                                                << Deconz::ParameterPanId << "finished with error" << replyPanId->statusCode();
                readNetworkParametersReply->m_statusCode = replyPanId->statusCode();
                readNetworkParametersReply->finished();
                return;
            }
            QDataStream stream(replyPanId->responseData());
            stream.setByteOrder(QDataStream::LittleEndian);
            quint16 payloadLenght = 0; quint8 parameter = 0; quint16 panId = 0;
            stream >> payloadLenght >> parameter >> panId;

            m_networkConfiguration.panId = panId;
            qCDebug(dcZigbeeController()) << "Request" << "SQN:" << replyPanId->sequenceNumber() << replyPanId->command()
                                          << static_cast<Deconz::Parameter>(parameter) << "finished successfully";
            qCDebug(dcZigbeeController()) << "PAN ID:" << ZigbeeUtils::convertUint16ToHexString(m_networkConfiguration.panId);

            // Read short address
            ZigbeeInterfaceDeconzReply *replyShortAddress = requestReadParameter(Deconz::ParameterNetworkAddress);
            connect(replyShortAddress, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyShortAddress](){
                if (replyShortAddress->statusCode() != Deconz::StatusCodeSuccess) {
                    qCWarning(dcZigbeeController()) << "Request" << "SQN:" << replyShortAddress->sequenceNumber() << replyShortAddress->command() << Deconz::ParameterNetworkAddress
                                                    << "finished with error" << replyShortAddress->statusCode();
                    readNetworkParametersReply->m_statusCode = replyShortAddress->statusCode();
                    readNetworkParametersReply->finished();
                    return;
                }

                QDataStream stream(replyShortAddress->responseData());
                stream.setByteOrder(QDataStream::LittleEndian);
                quint16 payloadLenght = 0; quint8 parameter = 0; quint16 shortAddress = 0;
                stream >> payloadLenght >> parameter >> shortAddress;
                m_networkConfiguration.shortAddress = shortAddress;
                qCDebug(dcZigbeeController()) << "Request" << "SQN:" << replyShortAddress->sequenceNumber()
                                              << replyShortAddress->command() << static_cast<Deconz::Parameter>(parameter) << "finished successfully";
                qCDebug(dcZigbeeController()) << ZigbeeUtils::convertUint16ToHexString(m_networkConfiguration.shortAddress);

                // Read extended PAN ID
                ZigbeeInterfaceDeconzReply *replyExtendedPanId = requestReadParameter(Deconz::ParameterNetworkExtendedPanId);
                connect(replyExtendedPanId, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyExtendedPanId](){
                    if (replyExtendedPanId->statusCode() != Deconz::StatusCodeSuccess) {
                        qCWarning(dcZigbeeController()) << "Request" << "SQN:" << replyExtendedPanId->sequenceNumber() << replyExtendedPanId->command()
                                                        << Deconz::ParameterNetworkExtendedPanId << "finished with error" << replyExtendedPanId->statusCode();
                        readNetworkParametersReply->m_statusCode = replyExtendedPanId->statusCode();
                        readNetworkParametersReply->finished();
                        return;
                    }

                    QDataStream stream(replyExtendedPanId->responseData());
                    stream.setByteOrder(QDataStream::LittleEndian);
                    quint16 payloadLenght = 0; quint8 parameter = 0; quint64 networkExtendedPanId = 0;
                    stream >> payloadLenght >> parameter >> networkExtendedPanId;
                    m_networkConfiguration.extendedPanId = networkExtendedPanId;
                    qCDebug(dcZigbeeController()) << "Request" << "SQN:" << replyExtendedPanId->sequenceNumber() << replyExtendedPanId->command() << static_cast<Deconz::Parameter>(parameter)
                                                  << "finished successfully";
                    qCDebug(dcZigbeeController()) << ZigbeeUtils::convertUint64ToHexString(m_networkConfiguration.extendedPanId);

                    // Read device type
                    ZigbeeInterfaceDeconzReply *replyNodeType = requestReadParameter(Deconz::ParameterNodeType);
                    connect(replyNodeType, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyNodeType](){
                        if (replyNodeType->statusCode() != Deconz::StatusCodeSuccess) {
                            qCWarning(dcZigbeeController()) << "Request" << "SQN:" << replyNodeType->sequenceNumber() << replyNodeType->command()
                                                            << Deconz::ParameterNodeType << "finished with error" << replyNodeType->statusCode();
                            readNetworkParametersReply->m_statusCode = replyNodeType->statusCode();
                            readNetworkParametersReply->finished();
                            return;
                        }

                        QDataStream stream(replyNodeType->responseData());
                        stream.setByteOrder(QDataStream::LittleEndian);
                        quint16 payloadLenght = 0; quint8 parameter = 0; quint8 nodeType = 0;
                        stream >> payloadLenght >> parameter >> nodeType;

                        m_networkConfiguration.nodeType = static_cast<Deconz::NodeType>(nodeType);
                        qCDebug(dcZigbeeController()) << "Request" << "SQN:" << replyNodeType->sequenceNumber() << replyNodeType->command() << static_cast<Deconz::Parameter>(parameter)
                                                      << "finished successfully";
                        qCDebug(dcZigbeeController()) << m_networkConfiguration.nodeType;

                        // Read channel mask
                        ZigbeeInterfaceDeconzReply *replyChannelMask = requestReadParameter(Deconz::ParameterChannelMask);
                        connect(replyChannelMask, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyChannelMask](){
                            if (replyChannelMask->statusCode() != Deconz::StatusCodeSuccess) {
                                qCWarning(dcZigbeeController()) << "Request" << "SQN:" << replyChannelMask->sequenceNumber() << replyChannelMask->command() << Deconz::ParameterChannelMask
                                                                << "finished with error" << replyChannelMask->statusCode();
                                readNetworkParametersReply->m_statusCode = replyChannelMask->statusCode();
                                readNetworkParametersReply->finished();
                                return;
                            }

                            QDataStream stream(replyChannelMask->responseData());
                            stream.setByteOrder(QDataStream::LittleEndian);
                            quint16 payloadLenght = 0; quint8 parameter = 0; quint32 channelMask = 0;
                            stream >> payloadLenght >> parameter >> channelMask;

                            m_networkConfiguration.channelMask = channelMask;
                            qCDebug(dcZigbeeController()) << "Request" << "SQN:" << replyChannelMask->sequenceNumber() << replyChannelMask->command()
                                                          << static_cast<Deconz::Parameter>(parameter) << "finished successfully";
                            qCDebug(dcZigbeeController()) << ZigbeeUtils::convertUint32ToHexString(m_networkConfiguration.channelMask);

                            // Read APS extended PAN ID
                            ZigbeeInterfaceDeconzReply *replyApsExtendedPanId = requestReadParameter(Deconz::ParameterApsExtendedPanId);
                            connect(replyApsExtendedPanId, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyApsExtendedPanId](){
                                if (replyApsExtendedPanId->statusCode() != Deconz::StatusCodeSuccess) {
                                    qCWarning(dcZigbeeController()) << "Request" << "SQN:" << replyApsExtendedPanId->sequenceNumber() << replyApsExtendedPanId->command()
                                                                    << Deconz::ParameterApsExtendedPanId << "finished with error" << replyApsExtendedPanId->statusCode();
                                    readNetworkParametersReply->m_statusCode = replyApsExtendedPanId->statusCode();
                                    readNetworkParametersReply->finished();
                                    return;
                                }

                                QDataStream stream(replyApsExtendedPanId->responseData());
                                stream.setByteOrder(QDataStream::LittleEndian);
                                quint16 payloadLenght = 0; quint8 parameter = 0; quint64 apsExtendedPanId = 0;
                                stream >> payloadLenght >> parameter >> apsExtendedPanId;

                                m_networkConfiguration.apsExtendedPanId = apsExtendedPanId;
                                qCDebug(dcZigbeeController()) << "Request" << "SQN:" << replyApsExtendedPanId->sequenceNumber() << replyApsExtendedPanId->command()
                                                              << static_cast<Deconz::Parameter>(parameter) << "finished successfully";
                                qCDebug(dcZigbeeController()) << ZigbeeUtils::convertUint64ToHexString(m_networkConfiguration.apsExtendedPanId);

                                // Read trust center address
                                ZigbeeInterfaceDeconzReply *replyTrustCenterAddress = requestReadParameter(Deconz::ParameterTrustCenterAddress);
                                connect(replyTrustCenterAddress, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyTrustCenterAddress](){
                                    if (replyTrustCenterAddress->statusCode() != Deconz::StatusCodeSuccess) {
                                        qCWarning(dcZigbeeController()) << "Request" << "SQN:" << replyTrustCenterAddress->sequenceNumber() << replyTrustCenterAddress->command()
                                                                        << Deconz::ParameterTrustCenterAddress << "finished with error" << replyTrustCenterAddress->statusCode();
                                        readNetworkParametersReply->m_statusCode = replyTrustCenterAddress->statusCode();
                                        readNetworkParametersReply->finished();
                                        return;
                                    }

                                    QDataStream stream(replyTrustCenterAddress->responseData());
                                    stream.setByteOrder(QDataStream::LittleEndian);
                                    quint16 payloadLenght = 0; quint8 parameter = 0; quint64 trustCenterAddress = 0;
                                    stream >> payloadLenght >> parameter >> trustCenterAddress;

                                    m_networkConfiguration.trustCenterAddress = ZigbeeAddress(trustCenterAddress);
                                    qCDebug(dcZigbeeController()) << "Request" << "SQN:" << replyTrustCenterAddress->sequenceNumber() << replyTrustCenterAddress->command()
                                                                  << static_cast<Deconz::Parameter>(parameter) << "finished successfully";
                                    qCDebug(dcZigbeeController()) << m_networkConfiguration.trustCenterAddress;

                                    // Read security mode
                                    ZigbeeInterfaceDeconzReply *replySecurityMode = requestReadParameter(Deconz::ParameterSecurityMode);
                                    connect(replySecurityMode, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replySecurityMode](){
                                        if (replySecurityMode->statusCode() != Deconz::StatusCodeSuccess) {
                                            qCWarning(dcZigbeeController()) << "Request" << "SQN:" << replySecurityMode->sequenceNumber() << replySecurityMode->command()
                                                                            << Deconz::ParameterSecurityMode << "finished with error" << replySecurityMode->statusCode();
                                            readNetworkParametersReply->m_statusCode = replySecurityMode->statusCode();
                                            readNetworkParametersReply->finished();
                                            return;
                                        }

                                        QDataStream stream(replySecurityMode->responseData());
                                        stream.setByteOrder(QDataStream::LittleEndian);
                                        quint16 payloadLenght = 0; quint8 parameter = 0; quint8 securityMode = 0;
                                        stream >> payloadLenght >> parameter >> securityMode;

                                        m_networkConfiguration.securityMode = static_cast<Deconz::SecurityMode>(securityMode);
                                        qCDebug(dcZigbeeController()) << "Request" << "SQN:" << replySecurityMode->sequenceNumber() << replySecurityMode->command()
                                                                      << static_cast<Deconz::Parameter>(parameter) << "finished successfully";
                                        qCDebug(dcZigbeeController()) << m_networkConfiguration.securityMode;

                                        // Note: reading the network key returns "InavlidParameter". Might be for security reasons which is good!

                                        // Read channel
                                        ZigbeeInterfaceDeconzReply *replyChannel = requestReadParameter(Deconz::ParameterCurrentChannel);
                                        connect(replyChannel, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyChannel](){
                                            if (replyChannel->statusCode() != Deconz::StatusCodeSuccess) {
                                                qCWarning(dcZigbeeController()) << "Request" << "SQN:" << replyChannel->sequenceNumber() << replyChannel->command() << Deconz::ParameterCurrentChannel
                                                                                << "finished with error" << replyChannel->statusCode();
                                                readNetworkParametersReply->m_statusCode = replyChannel->statusCode();
                                                readNetworkParametersReply->finished();
                                                return;
                                            }

                                            QDataStream stream(replyChannel->responseData());
                                            stream.setByteOrder(QDataStream::LittleEndian);
                                            quint16 payloadLenght = 0; quint8 parameter = 0; quint8 channel = 0;
                                            stream >> payloadLenght >> parameter >> channel;
                                            m_networkConfiguration.currentChannel = channel;
                                            qCDebug(dcZigbeeController()) << "Request" << "SQN:" << replyChannel->sequenceNumber() << replyChannel->command() << static_cast<Deconz::Parameter>(parameter)
                                                                          << "finished successfully";
                                            qCDebug(dcZigbeeController()) << "Current channel:" << m_networkConfiguration.currentChannel;


                                            // Read permit join status
                                            ZigbeeInterfaceDeconzReply *replyPermitJoin = requestReadParameter(Deconz::ParameterPermitJoin);
                                            connect(replyPermitJoin, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyPermitJoin](){
                                                if (replyPermitJoin->statusCode() != Deconz::StatusCodeSuccess) {
                                                    qCWarning(dcZigbeeController()) << "Request" << "SQN:" << replyPermitJoin->sequenceNumber() << replyPermitJoin->command()
                                                                                    << Deconz::ParameterPermitJoin << "finished with error" << replyPermitJoin->statusCode();
                                                    readNetworkParametersReply->m_statusCode = replyPermitJoin->statusCode();
                                                    readNetworkParametersReply->finished();
                                                    return;
                                                }

                                                QDataStream stream(replyPermitJoin->responseData());
                                                stream.setByteOrder(QDataStream::LittleEndian);
                                                quint16 payloadLenght = 0; quint8 parameter = 0;
                                                stream >> payloadLenght >> parameter;
                                                //m_networkConfiguration.currentChannel = channel;
                                                qCDebug(dcZigbeeController()) << "Request" << "SQN:" << replyPermitJoin->sequenceNumber() << replyPermitJoin->command() << static_cast<Deconz::Parameter>(parameter)
                                                                              << "finished successfully" << ZigbeeUtils::convertByteArrayToHexString(replyPermitJoin->responseData());


                                                // Read protocol version
                                                ZigbeeInterfaceDeconzReply *replyProtocolVersion = requestReadParameter(Deconz::ParameterProtocolVersion);
                                                connect(replyProtocolVersion, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyProtocolVersion](){
                                                    if (replyProtocolVersion->statusCode() != Deconz::StatusCodeSuccess) {
                                                        qCWarning(dcZigbeeController()) << "Request" << "SQN:" << replyProtocolVersion->sequenceNumber() << replyProtocolVersion->command()
                                                                                        << Deconz::ParameterProtocolVersion << "finished with error" << replyProtocolVersion->statusCode();
                                                        readNetworkParametersReply->m_statusCode = replyProtocolVersion->statusCode();
                                                        readNetworkParametersReply->finished();
                                                        return;
                                                    }

                                                    QDataStream stream(replyProtocolVersion->responseData());
                                                    stream.setByteOrder(QDataStream::LittleEndian);
                                                    quint16 payloadLenght = 0; quint8 parameter = 0; quint16 protocolVersion = 0;
                                                    stream >> payloadLenght >> parameter >> protocolVersion;
                                                    m_networkConfiguration.protocolVersion = protocolVersion;
                                                    qCDebug(dcZigbeeController()) << "Request" << "SQN:" << replyProtocolVersion->sequenceNumber() << replyProtocolVersion->command()
                                                                                  << static_cast<Deconz::Parameter>(parameter) << "finished successfully";
                                                    qCDebug(dcZigbeeController()) << "Protocol version:" << ZigbeeUtils::convertUint16ToHexString(m_networkConfiguration.protocolVersion);

                                                    // Read network updat id
                                                    ZigbeeInterfaceDeconzReply *replyNetworkUpdateId = requestReadParameter(Deconz::ParameterNetworkUpdateId);
                                                    connect(replyNetworkUpdateId, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyNetworkUpdateId](){
                                                        if (replyNetworkUpdateId->statusCode() != Deconz::StatusCodeSuccess) {
                                                            qCWarning(dcZigbeeController()) << "Request" << "SQN:" << replyNetworkUpdateId->sequenceNumber() << replyNetworkUpdateId->command()
                                                                                            << Deconz::ParameterNetworkUpdateId << "finished with error" << replyNetworkUpdateId->statusCode();
                                                            readNetworkParametersReply->m_statusCode = replyNetworkUpdateId->statusCode();
                                                            readNetworkParametersReply->finished();
                                                            return;
                                                        }

                                                        QDataStream stream(replyNetworkUpdateId->responseData());
                                                        stream.setByteOrder(QDataStream::LittleEndian);
                                                        quint16 payloadLenght = 0; quint8 parameter = 0; quint8 networkUpdateId = 0;
                                                        stream >> payloadLenght >> parameter >> networkUpdateId;
                                                        m_networkConfiguration.networkUpdateId = networkUpdateId;
                                                        qCDebug(dcZigbeeController()) << "Request" << "SQN:" << replyNetworkUpdateId->sequenceNumber() << replyNetworkUpdateId->command()
                                                                                      << static_cast<Deconz::Parameter>(parameter) << "finished successfully";
                                                        qCDebug(dcZigbeeController()) << "Network update ID:" << m_networkConfiguration.networkUpdateId;

                                                        // Make sure the watchdog is available for this version
                                                        if (m_networkConfiguration.protocolVersion < 0x0108) {
                                                            qCDebug(dcZigbeeController()) << "The watchdog api is available since protocol version 0x0108. The watchdog is not required for this version";
                                                            m_watchdogTimer->stop();

                                                            // Finished reading all parameters. Finish the independent reply in order to indicate the process has finished
                                                            emit networkConfigurationParameterChanged(m_networkConfiguration);
                                                            readNetworkParametersReply->m_statusCode = Deconz::StatusCodeSuccess;
                                                            readNetworkParametersReply->finished();
                                                            return;
                                                        }

                                                        // Reset the watchdog in any case
                                                        resetControllerWatchdog();

                                                        // Read watchdog timeout
                                                        ZigbeeInterfaceDeconzReply *replyWatchdogTimeout = requestReadParameter(Deconz::ParameterWatchdogTtl);
                                                        connect(replyWatchdogTimeout, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyWatchdogTimeout](){
                                                            if (replyWatchdogTimeout->statusCode() != Deconz::StatusCodeSuccess) {
                                                                qCWarning(dcZigbeeController()) << "Request" << "SQN:" << replyWatchdogTimeout->sequenceNumber() << replyWatchdogTimeout->command()
                                                                                                << Deconz::ParameterWatchdogTtl << "finished with error" << replyWatchdogTimeout->statusCode();
                                                                readNetworkParametersReply->m_statusCode = replyWatchdogTimeout->statusCode();
                                                                readNetworkParametersReply->finished();
                                                                return;
                                                            }

                                                            QDataStream stream(replyWatchdogTimeout->responseData());
                                                            stream.setByteOrder(QDataStream::LittleEndian);
                                                            quint16 payloadLenght = 0; quint8 parameter = 0; quint32 watchdogTimeout = 0;
                                                            stream >> payloadLenght >> parameter >> watchdogTimeout;
                                                            m_networkConfiguration.watchdogTimeout = watchdogTimeout;
                                                            qCDebug(dcZigbeeController()) << "Request" << "SQN:" << replyWatchdogTimeout->sequenceNumber() << replyWatchdogTimeout->command()
                                                                                          << static_cast<Deconz::Parameter>(parameter) << "finished successfully";
                                                            qCDebug(dcZigbeeController()) << "Watchdog timeout:" << m_networkConfiguration.watchdogTimeout;

                                                            // Finished reading all parameters. Finish the independent reply in order to indicate the process has finished
                                                            emit networkConfigurationParameterChanged(m_networkConfiguration);
                                                            readNetworkParametersReply->m_statusCode = Deconz::StatusCodeSuccess;
                                                            readNetworkParametersReply->finished();
                                                        });
                                                    });
                                                });
                                            });
                                        });
                                    });
                                });
                            });
                        });
                    });
                });
            });
        });
    });

    return readNetworkParametersReply;
}

DeconzDeviceState ZigbeeBridgeControllerDeconz::parseDeviceStateFlag(quint8 deviceStateFlag)
{
    DeconzDeviceState state;
    state.networkState = static_cast<Deconz::NetworkState>(deviceStateFlag & 0x03);
    state.apsDataConfirm = (deviceStateFlag & 0x04);
    state.apsDataIndication = (deviceStateFlag & 0x08);
    state.configurationChanged = (deviceStateFlag & 0x10);
    state.apsDataRequestFreeSlots = (deviceStateFlag & 0x20);
    return state;
}

void ZigbeeBridgeControllerDeconz::readDataIndication()
{
    if (m_readIndicationReply) {
        // There is alreay a read indication reply in the queue, let finish that first before creating a new one
        return;
    }

    ZigbeeInterfaceDeconzReply *reply = requestReadReceivedDataIndication();
    // Set this as the current read indication reply so we don't request more than one at the time
    m_readIndicationReply = reply;
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
        // Allow to send the next read indication reply if required
        m_readIndicationReply = nullptr;

        if (reply->statusCode() != Deconz::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Could not read data indication." << "SQN:" << reply->sequenceNumber() << reply->statusCode();
            // FIXME: set an appropriate error
            return;
        }


        // APS data indication received, process the content
        qCDebug(dcZigbeeController()) << "Reading data indication finished successfully" << "SQN:" << reply->sequenceNumber();
        processDataIndication(reply->responseData());
    });
}

void ZigbeeBridgeControllerDeconz::readDataConfirm()
{
    if (m_readConfirmReply) {
        // There is alreay a read confirm reply in the queue, let finish that first before creating a new one
        return;
    }

    ZigbeeInterfaceDeconzReply *reply = requestQuerySendDataConfirm();
    // Set this as the current read confirm reply so we don't request more than one at the time
    m_readConfirmReply = reply;
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
        // Allow to send the next read confirm reply if required
        m_readConfirmReply = nullptr;

        if (reply->statusCode() != Deconz::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Could not read data confirm." << "SQN:" << reply->sequenceNumber() << reply->statusCode();
            // FIXME: set an appropriate error
            return;
        }

        // APS data confirm received, process the content
        qCDebug(dcZigbeeController()) << "Reading data confirm finished successfully" << "SQN:" << reply->sequenceNumber();
        processDataConfirm(reply->responseData());
    });
}

void ZigbeeBridgeControllerDeconz::processDeviceState(DeconzDeviceState deviceState)
{
    qCDebug(dcZigbeeController()) << "Process device state notification" << deviceState;

    if (m_networkState != deviceState.networkState) {
        qCDebug(dcZigbeeController()) << "Network state changed" << deviceState.networkState;
        m_networkState = deviceState.networkState;
        emit networkStateChanged(m_networkState);
    }

    if (m_apsFreeSlotsAvailable != deviceState.apsDataRequestFreeSlots) {
        m_apsFreeSlotsAvailable = deviceState.apsDataRequestFreeSlots;
        if (!m_apsFreeSlotsAvailable) {
            qCWarning(dcZigbeeController()) << "The APS request table is full on the device. Cannot send requests until the queue gets processed on the controller.";
            return;
        } else {
            qCDebug(dcZigbeeController()) << "The APS request table is free again. Sending the next request";
            sendNextRequest();
        }
    }

    if (m_networkState != Deconz::NetworkStateConnected)
        return;

    // Note: read a data indication before a confirmation since the confirmation arrives after a related indication normally

    // Check if we have to read a data indication message
    if (deviceState.apsDataIndication) {
        readDataIndication();
    }

    // Check if we have a response to read for a request
    if (deviceState.apsDataConfirm) {
        readDataConfirm();
    }

}

void ZigbeeBridgeControllerDeconz::processDataIndication(const QByteArray &data)
{
    // APS data indication
    QDataStream stream(data);
    stream.setByteOrder(QDataStream::LittleEndian);
    quint16 payloadLenght = 0; quint8 deviceStateFlag = 0; quint8 reserved = 0; quint16 asduLength = 0;

    Zigbee::ApsdeDataIndication indication;
    stream >> payloadLenght >> deviceStateFlag;
    stream >> indication.destinationAddressMode;
    Zigbee::DestinationAddressMode destinationAddressMode = static_cast<Zigbee::DestinationAddressMode>(indication.destinationAddressMode);
    if (destinationAddressMode == Zigbee::DestinationAddressModeGroup || destinationAddressMode == Zigbee::DestinationAddressModeShortAddress)
        stream >> indication.destinationShortAddress;

    if (destinationAddressMode == Zigbee::DestinationAddressModeIeeeAddress)
        stream >> indication.destinationIeeeAddress;

    stream >> indication.destinationEndpoint >> indication.sourceAddressMode;

    Zigbee::SourceAddressMode sourceAddressMode = static_cast<Zigbee::SourceAddressMode>(indication.sourceAddressMode);
    if (sourceAddressMode == Zigbee::SourceAddressModeShortAddress || sourceAddressMode == Zigbee::SourceAddressModeShortAndIeeeAddress)
        stream >> indication.sourceShortAddress;

    if (sourceAddressMode == Zigbee::SourceAddressModeIeeeAddress || sourceAddressMode == Zigbee::SourceAddressModeShortAndIeeeAddress)
        stream >> indication.sourceIeeeAddress;

    stream >> indication.sourceEndpoint >> indication.profileId >> indication.clusterId >> asduLength;
    // Fill asdu data
    for (int i = 0; i < asduLength; i++) {
        quint8 byte = 0;
        stream >> byte;
        indication.asdu.append(static_cast<char>(byte));
    }

    stream >> reserved >> reserved >> indication.lqi >> reserved >> reserved >> reserved >> reserved >> indication.rssi;

    // Print the information for debugging
    qCDebug(dcZigbeeController()) << indication;
    qCDebug(dcZigbeeAps()) << "APSDE-DATA.indication" << indication;

    emit apsDataIndicationReceived(indication);

    // Process the device state in order to check if we have to request another indication
    DeconzDeviceState deviceState = parseDeviceStateFlag(deviceStateFlag);
    qCDebug(dcZigbeeController()) << "Verify device state after data indication response" << deviceState;
    if (deviceState.apsDataIndication) {
        readDataIndication();
    }
}

void ZigbeeBridgeControllerDeconz::processDataConfirm(const QByteArray &data)
{
    QDataStream stream(data);
    stream.setByteOrder(QDataStream::LittleEndian);
    Zigbee::ApsdeDataConfirm confirm;
    quint16 payloadLenght = 0; quint8 deviceStateFlag = 0;
    stream >> payloadLenght >> deviceStateFlag;
    stream >> confirm.requestId >> confirm.destinationAddressMode;

    if (confirm.destinationAddressMode == Zigbee::DestinationAddressModeGroup || confirm.destinationAddressMode == Zigbee::DestinationAddressModeShortAddress)
        stream >> confirm.destinationShortAddress;

    if (confirm.destinationAddressMode == Zigbee::DestinationAddressModeIeeeAddress)
        stream >> confirm.destinationIeeeAddress;

    stream >> confirm.destinationEndpoint >> confirm.sourceEndpoint >> confirm.zigbeeStatusCode;

    // Print the information for debugging
    qCDebug(dcZigbeeController()) << confirm;
    qCDebug(dcZigbeeAps()) << "APSDE-DATA.confirm" << confirm;

    emit apsDataConfirmReceived(confirm);

    // Process the device state in order to check if we have to request another indication
    DeconzDeviceState deviceState = parseDeviceStateFlag(deviceStateFlag);
    qCDebug(dcZigbeeController()) << "Verify device state after data confirmation response" << deviceState;
    if (deviceState.apsDataConfirm) {
        readDataConfirm();
    }
}

void ZigbeeBridgeControllerDeconz::onInterfaceAvailableChanged(bool available)
{
    qCDebug(dcZigbeeController()) << "Interface available changed" << available;
    if (!available) {
        // Clean up any pending replies
        while (!m_replyQueue.isEmpty()) {
            ZigbeeInterfaceDeconzReply *reply = m_replyQueue.dequeue();
            reply->abort();
        }

        m_sequenceNumber = 0;
        m_apsFreeSlotsAvailable = true;
        m_watchdogTimer->stop();
    }

    setAvailable(available);
    sendNextRequest();
}

void ZigbeeBridgeControllerDeconz::onInterfacePackageReceived(const QByteArray &package)
{
    QDataStream stream(package);
    stream.setByteOrder(QDataStream::LittleEndian);
    quint8 commandInt = 0; quint8 sequenceNumber = 0; quint8 statusInt = 0; quint16 frameLength = 0;
    stream >> commandInt >> sequenceNumber >> statusInt >> frameLength;

    QByteArray data = package.right(package.length() - 5);
    Deconz::Command command = static_cast<Deconz::Command>(commandInt);
    Deconz::StatusCode status = static_cast<Deconz::StatusCode>(statusInt);
    qCDebug(dcZigbeeController()) << "Interface message received" << command << "SQN:" << sequenceNumber
                                  << status << "Frame length:" << frameLength << ZigbeeUtils::convertByteArrayToHexString(data);

    // Check if this is the response to the current active reply
    if (m_currentReply && m_currentReply->sequenceNumber() == sequenceNumber && m_currentReply->command() == command) {
        m_currentReply->m_responseData = data;
        m_currentReply->m_statusCode = status;
        m_currentReply->finished();
        // Note: the current reply will be cleaned up in the finished slot
        return;
    }

    // We got a notification, lets set the current sequence number to the notification id,
    // so the next request will be a continuouse increase
    m_sequenceNumber = sequenceNumber + 1;

    // No request for this data, lets check which notification and process the data
    switch (command) {
    case Deconz::CommandDeviceStateChanged: {
        quint8 deviceStateFlag = 0;
        stream >> deviceStateFlag;
        processDeviceState(parseDeviceStateFlag(deviceStateFlag));
        break;
    }
    case Deconz::CommandMacPoll: {
        // FIXME: parse the data and print info
        qCDebug(dcZigbeeController()) << "MAC Poll command received" << ZigbeeUtils::convertByteArrayToHexString(data);
        break;
    }
    case Deconz::CommandSimplifiedBeacon: {
        // FIXME: parse the data and print info
        qCDebug(dcZigbeeController()) << "Simplified beacon command received" << ZigbeeUtils::convertByteArrayToHexString(data);
        break;
    }
    default:
        qCWarning(dcZigbeeController()) << "Unhandled interface package received" << command << "SQN:" << sequenceNumber
                                        << status << "Frame length:" << frameLength << ZigbeeUtils::convertByteArrayToHexString(data);
        break;
    }
}

void ZigbeeBridgeControllerDeconz::resetControllerWatchdog()
{
    QByteArray parameterData;
    QDataStream stream(&parameterData, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << m_watchdogTimeout;
    ZigbeeInterfaceDeconzReply *reply = requestWriteParameter(Deconz::ParameterWatchdogTtl, parameterData);
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
        if (reply->statusCode() != Deconz::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Could not reset the application watchdog on the deCONZ controller." << reply->statusCode();
            return;
        }
        qCDebug(dcZigbeeController()) << "Reset application watchdog on the deCONZ controller successfully";
        m_watchdogTimer->start();
    });
}

bool ZigbeeBridgeControllerDeconz::enable(const QString &serialPort, qint32 baudrate)
{
    return m_interface->enable(serialPort, baudrate);
}

void ZigbeeBridgeControllerDeconz::disable()
{
    m_interface->disable();
}

QDebug operator<<(QDebug debug, const DeconzDeviceState &deviceState)
{
    debug.nospace() << "DeviceState(";
    switch (deviceState.networkState) {
    case Deconz::NetworkStateJoining:
        debug.nospace() << "Joining, ";
        break;
    case Deconz::NetworkStateLeaving:
        debug.nospace() << "Leaving, ";
        break;
    case Deconz::NetworkStateOffline:
        debug.nospace() << "Offline, ";
        break;
    case Deconz::NetworkStateConnected:
        debug.nospace() << "Connected, ";
        break;
    }

    debug.nospace() << "Confirm=" << static_cast<int>(deviceState.apsDataConfirm) << ", ";
    debug.nospace() << "Indication=" << static_cast<int>(deviceState.apsDataIndication) << ", ";
    debug.nospace() << "ConfigChanged=" << static_cast<int>(deviceState.configurationChanged) << ", ";
    debug.nospace() << "CanSend=" << deviceState.apsDataRequestFreeSlots << ")";
    return debug.space();
}

QDebug operator<<(QDebug debug, const DeconzNetworkConfiguration &configuration)
{
    debug.nospace() << "Network configuration:" << endl;
    debug.nospace() << " - Node type:" << configuration.nodeType << endl;
    debug.nospace() << " - IEEE address:" << configuration.ieeeAddress.toString() << endl;
    debug.nospace() << " - NWK address:" << ZigbeeUtils::convertUint16ToHexString(configuration.shortAddress) << endl;
    debug.nospace() << " - PAN ID:" << ZigbeeUtils::convertUint16ToHexString(configuration.panId) << endl;
    debug.nospace() << " - Extended PAN ID:" << ZigbeeUtils::convertUint64ToHexString(configuration.extendedPanId) << endl;
    debug.nospace() << " - APS Extended PAN ID:" << ZigbeeUtils::convertUint64ToHexString(configuration.apsExtendedPanId) << endl;
    debug.nospace() << " - Trust center IEEE address:" << configuration.trustCenterAddress.toString() << endl;
    debug.nospace() << " - Channel mask:" << ZigbeeChannelMask(configuration.channelMask) << endl;
    debug.nospace() << " - Channel:" << configuration.currentChannel << endl;
    debug.nospace() << " - Security mode:" << configuration.securityMode << endl;
    debug.nospace() << " - Protocol version:" << ZigbeeUtils::convertUint16ToHexString(configuration.protocolVersion) << endl;
    debug.nospace() << " - Network update ID:" << ZigbeeUtils::convertByteToHexString(configuration.networkUpdateId) << endl;
    debug.nospace() << " - Watchdog TTL:" << configuration.watchdogTimeout << endl;
    return debug.space();
}
