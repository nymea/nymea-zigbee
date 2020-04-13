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
#include "zigbeedeviceprofile.h"
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
    m_watchdogTimer->setInterval(m_watchdogResetTimout * 1000); // Set the watchdog to 85 seconds, reset every 60 s
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
    quint8 sequenceNumber = generateSequenceNumber();
    qCDebug(dcZigbeeController()) << "Request version. SQN:" << sequenceNumber;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandVersion);
    stream << static_cast<quint8>(sequenceNumber);
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(5); // Frame length

    ZigbeeInterfaceDeconzReply *reply = new ZigbeeInterfaceDeconzReply(Deconz::CommandVersion, sequenceNumber, this);
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, reply, &ZigbeeInterfaceDeconzReply::deleteLater);
    m_pendingReplies.insert(sequenceNumber, reply);

    m_interface->sendPackage(message);
    return reply;
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestDeviceState()
{
    quint8 sequenceNumber = generateSequenceNumber();
    qCDebug(dcZigbeeController()) << "Request device state. SQN:" << sequenceNumber;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandDeviceState);
    stream << static_cast<quint8>(sequenceNumber);
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(8); // Frame length
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint8>(0); // Reserverd

    ZigbeeInterfaceDeconzReply *reply = new ZigbeeInterfaceDeconzReply(Deconz::CommandDeviceState, sequenceNumber, this);
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, reply, &ZigbeeInterfaceDeconzReply::deleteLater);
    m_pendingReplies.insert(sequenceNumber, reply);

    m_interface->sendPackage(message);

    return createReply(Deconz::CommandDeviceState, sequenceNumber, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestReadParameter(Deconz::Parameter parameter)
{
    quint8 sequenceNumber = generateSequenceNumber();
    qCDebug(dcZigbeeController()) << "Request read parameter. SQN:" << sequenceNumber << parameter;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandReadParameter);
    stream << static_cast<quint8>(sequenceNumber);
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(8); // Frame length 7 + 1 payload
    stream << static_cast<quint16>(1); // Payload length
    stream << static_cast<quint8>(parameter);

    m_interface->sendPackage(message);

    return createReply(Deconz::CommandReadParameter, sequenceNumber, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestWriteParameter(Deconz::Parameter parameter, const QByteArray &data)
{
    quint8 sequenceNumber = generateSequenceNumber();
    qCDebug(dcZigbeeController()) << "Request write parameter. SQN:" << sequenceNumber << parameter << ZigbeeUtils::convertByteArrayToHexString(data);

    quint16 payloadLength = static_cast<quint16>(1 + data.length());
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandWriteParameter);
    stream << static_cast<quint8>(sequenceNumber);
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(7 + payloadLength); // Frame length 7 + payload length
    stream << static_cast<quint16>(payloadLength); // 1 parameter + parameter data length
    stream << static_cast<quint8>(parameter);
    for (int i = 0; i < data.length(); i++) {
        stream << static_cast<quint8>(data.at(i));
    }

    m_interface->sendPackage(message);

    return createReply(Deconz::CommandWriteParameter, sequenceNumber, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestChangeNetworkState(Deconz::NetworkState networkState)
{
    quint8 sequenceNumber = generateSequenceNumber();
    qCDebug(dcZigbeeController()) << "Request change network state. SQN:" << sequenceNumber << networkState;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandChangeNetworkState);
    stream << static_cast<quint8>(sequenceNumber);
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(6); // Frame length
    stream << static_cast<quint8>(networkState);

    m_interface->sendPackage(message);

    return createReply(Deconz::CommandChangeNetworkState, sequenceNumber, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestReadReceivedDataIndication(Deconz::SourceAddressMode sourceAddressMode)
{
    quint8 sequenceNumber = generateSequenceNumber();
    qCDebug(dcZigbeeController()) << "Request read received data indication. SQN:" << sequenceNumber << ZigbeeUtils::convertByteToHexString(sourceAddressMode);

    quint16 payloadLength = 0;
    if (sourceAddressMode != Deconz::SourceAddressModeNone) {
        payloadLength = 1;
    }

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandApsDataIndication);
    stream << static_cast<quint8>(sequenceNumber);
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(7 + payloadLength); // Frame length + payload length
    stream << static_cast<quint16>(payloadLength); // payload length
    if (payloadLength > 0)
        stream << static_cast<quint8>(sourceAddressMode);

    m_interface->sendPackage(message);

    return createReply(Deconz::CommandApsDataIndication, sequenceNumber, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestQuerySendDataConfirm()
{
    quint8 sequenceNumber = generateSequenceNumber();
    qCDebug(dcZigbeeController()) << "Request query send data confirm. SQN:" << sequenceNumber;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandApsDataConfirm);
    stream << static_cast<quint8>(sequenceNumber);
    stream << static_cast<quint8>(0); // Reserverd
    stream << static_cast<quint16>(7); // Frame length
    stream << static_cast<quint16>(0); // Payload length

    m_interface->sendPackage(message);

    return createReply(Deconz::CommandApsDataConfirm, sequenceNumber, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestEnqueueSendDataGroup(quint8 requestId, quint16 groupAddress, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius)
{
    quint8 sequenceNumber = generateSequenceNumber();
    qCDebug(dcZigbeeController()) << "Request enqueue send data to group" << ZigbeeUtils::convertUint16ToHexString(groupAddress)
                                  << "SQN:" << sequenceNumber
                                  << static_cast<Zigbee::ZigbeeProfile>(profileId)
                                  << ZigbeeUtils::convertUint16ToHexString(clusterId)
                                  << ZigbeeUtils::convertByteToHexString(sourceEndpoint);

    Q_ASSERT_X(asdu.length() <= 127, "ASDU", "ASDU package length has to <= 127 bytes");

    // Note: 14 protocol bytes + asdu package length
    quint16 payloadLength = static_cast<quint16>(14 + asdu.length());

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandApsDataRequest);
    stream << static_cast<quint8>(sequenceNumber);
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

    m_interface->sendPackage(message);

    return createReply(Deconz::CommandApsDataRequest, sequenceNumber, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestEnqueueSendDataShortAddress(quint8 requestId, quint16 shortAddress, quint8 destinationEndpoint, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius)
{
    quint8 sequenceNumber = generateSequenceNumber();
    qCDebug(dcZigbeeController()) << "Request enqueue send data to short address" << ZigbeeUtils::convertUint16ToHexString(shortAddress)
                                  << "SQN:" << sequenceNumber
                                  << ZigbeeUtils::convertByteToHexString(destinationEndpoint)
                                  << static_cast<Zigbee::ZigbeeProfile>(profileId)
                                  << ZigbeeUtils::convertUint16ToHexString(clusterId)
                                  << ZigbeeUtils::convertByteToHexString(sourceEndpoint);

    Q_ASSERT_X(asdu.length() <= 127, "ASDU", "ASDU package length has to <= 127 bytes");

    // Note: 15 protocol bytes + asdu package length
    quint16 payloadLength = static_cast<quint16>(15 + asdu.length());

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandApsDataRequest);
    stream << static_cast<quint8>(sequenceNumber);
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
    stream << static_cast<quint8>(txOptions); // TX Options: Use ASP ACKs
    stream << radius;

    m_interface->sendPackage(message);

    return createReply(Deconz::CommandApsDataRequest, sequenceNumber, this);
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::requestEnqueueSendDataIeeeAddress(quint8 requestId, ZigbeeAddress ieeeAddress, quint8 destinationEndpoint, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius)
{
    quint8 sequenceNumber = generateSequenceNumber();
    qCDebug(dcZigbeeController()) << "Request enqueue send data to IEEE address" << ieeeAddress.toString()
                                  << "SQN:" << sequenceNumber
                                  << ZigbeeUtils::convertByteToHexString(destinationEndpoint)
                                  << profileId << clusterId
                                  << ZigbeeUtils::convertByteToHexString(sourceEndpoint);

    Q_ASSERT_X(asdu.length() <= 127, "ASDU", "ASDU package length has to <= 127 bytes");

    // Note: 21 protocol bytes + asdu package length
    quint16 payloadLength = static_cast<quint16>(21 + asdu.length());

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Deconz::CommandApsDataRequest);
    stream << static_cast<quint8>(sequenceNumber);
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
    stream << static_cast<quint8>(txOptions); // TX Options: Use ASP ACKs
    stream << radius;

    m_interface->sendPackage(message);

    return createReply(Deconz::CommandApsDataRequest, sequenceNumber, this);
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

quint8 ZigbeeBridgeControllerDeconz::generateSequenceNumber()
{
    return m_sequenceNumber++;
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::createReply(Deconz::Command command, quint8 sequenceNumber, QObject *parent)
{
    // Create the reply
    ZigbeeInterfaceDeconzReply *reply = new ZigbeeInterfaceDeconzReply(command, sequenceNumber, parent);

    // Auto delete the object on finished
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, reply, &ZigbeeInterfaceDeconzReply::deleteLater);

    // Add it to the pending list
    m_pendingReplies.insert(sequenceNumber, reply);

    // Fixme: start the timer once actually sent to the interface
    reply->m_timer->start();

    return reply;
}

ZigbeeInterfaceDeconzReply *ZigbeeBridgeControllerDeconz::readNetworkParameters()
{
    qCDebug(dcZigbeeController()) << "Start reading network parameters";

    // This method reads all network configuration parameters sequentially. This method returns a reply which will be finished either
    // when a read request failes or all requests finished successfully.
    // If read request failes, this mehtod returns the status code of the failed request.

    // Create an independent reply for finishing the entire read sequence
    ZigbeeInterfaceDeconzReply *readNetworkParametersReply = new ZigbeeInterfaceDeconzReply(Deconz::CommandReadParameter, m_sequenceNumber, this);
    connect(readNetworkParametersReply, &ZigbeeInterfaceDeconzReply::finished, readNetworkParametersReply, &ZigbeeInterfaceDeconzReply::deleteLater);

    // Read MAC address of the bridge
    ZigbeeInterfaceDeconzReply *replyMacAddress = requestReadParameter(Deconz::ParameterMacAddress);
    connect(replyMacAddress, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyMacAddress](){
        if (replyMacAddress->statusCode() != Deconz::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Request" << replyMacAddress->command() << Deconz::ParameterMacAddress
                                            << "finished with error" << replyMacAddress->statusCode();

            readNetworkParametersReply->m_statusCode = replyMacAddress->statusCode();
            readNetworkParametersReply->finished();
            return;
        }
        QDataStream stream(replyMacAddress->responseData());
        stream.setByteOrder(QDataStream::LittleEndian);
        quint16 payloadLenght = 0; quint8 parameter = 0; quint64 macAddress = 0;
        stream >> payloadLenght >> parameter >> macAddress;

        m_networkConfiguration.ieeeAddress = ZigbeeAddress(macAddress);
        qCDebug(dcZigbeeController()) << "Request" << replyMacAddress->command() << static_cast<Deconz::Parameter>(parameter)
                                      << "finished successfully";
        qCDebug(dcZigbeeController()) << "IEEE address:" << m_networkConfiguration.ieeeAddress.toString();

        // Read PAN ID
        ZigbeeInterfaceDeconzReply *replyPanId = requestReadParameter(Deconz::ParameterPanId);
        connect(replyPanId, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyPanId](){
            if (replyPanId->statusCode() != Deconz::StatusCodeSuccess) {
                qCWarning(dcZigbeeController()) << "Request" << replyPanId->command() << Deconz::ParameterPanId
                                                << "finished with error" << replyPanId->statusCode();
                readNetworkParametersReply->m_statusCode = replyPanId->statusCode();
                readNetworkParametersReply->finished();
                return;
            }
            QDataStream stream(replyPanId->responseData());
            stream.setByteOrder(QDataStream::LittleEndian);
            quint16 payloadLenght = 0; quint8 parameter = 0; quint16 panId = 0;
            stream >> payloadLenght >> parameter >> panId;

            m_networkConfiguration.panId = panId;
            qCDebug(dcZigbeeController()) << "Request" << replyPanId->command() << static_cast<Deconz::Parameter>(parameter)
                                          << "finished successfully";
            qCDebug(dcZigbeeController()) << "PAN ID:" << ZigbeeUtils::convertUint16ToHexString(m_networkConfiguration.panId);

            // Read short address
            ZigbeeInterfaceDeconzReply *replyShortAddress = requestReadParameter(Deconz::ParameterNetworkAddress);
            connect(replyShortAddress, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyShortAddress](){
                if (replyShortAddress->statusCode() != Deconz::StatusCodeSuccess) {
                    qCWarning(dcZigbeeController()) << "Request" << replyShortAddress->command() << Deconz::ParameterNetworkAddress
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
                qCDebug(dcZigbeeController()) << "Request" << replyShortAddress->command() << static_cast<Deconz::Parameter>(parameter)
                                              << "finished successfully";
                qCDebug(dcZigbeeController()) << ZigbeeUtils::convertUint16ToHexString(m_networkConfiguration.shortAddress);

                // Read extended PAN ID
                ZigbeeInterfaceDeconzReply *replyExtendedPanId = requestReadParameter(Deconz::ParameterNetworkExtendedPanId);
                connect(replyExtendedPanId, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyExtendedPanId](){
                    if (replyExtendedPanId->statusCode() != Deconz::StatusCodeSuccess) {
                        qCWarning(dcZigbeeController()) << "Request" << replyExtendedPanId->command() << Deconz::ParameterNetworkExtendedPanId
                                                        << "finished with error" << replyExtendedPanId->statusCode();
                        readNetworkParametersReply->m_statusCode = replyExtendedPanId->statusCode();
                        readNetworkParametersReply->finished();
                        return;
                    }

                    QDataStream stream(replyExtendedPanId->responseData());
                    stream.setByteOrder(QDataStream::LittleEndian);
                    quint16 payloadLenght = 0; quint8 parameter = 0; quint64 networkExtendedPanId = 0;
                    stream >> payloadLenght >> parameter >> networkExtendedPanId;
                    m_networkConfiguration.extendedPanId = networkExtendedPanId;
                    qCDebug(dcZigbeeController()) << "Request" << replyExtendedPanId->command() << static_cast<Deconz::Parameter>(parameter)
                                                  << "finished successfully";
                    qCDebug(dcZigbeeController()) << ZigbeeUtils::convertUint64ToHexString(m_networkConfiguration.extendedPanId);

                    // Read device type
                    ZigbeeInterfaceDeconzReply *replyNodeType = requestReadParameter(Deconz::ParameterNodeType);
                    connect(replyNodeType, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyNodeType](){
                        if (replyNodeType->statusCode() != Deconz::StatusCodeSuccess) {
                            qCWarning(dcZigbeeController()) << "Request" << replyNodeType->command() << Deconz::ParameterNodeType
                                                            << "finished with error" << replyNodeType->statusCode();
                            readNetworkParametersReply->m_statusCode = replyNodeType->statusCode();
                            readNetworkParametersReply->finished();
                            return;
                        }

                        QDataStream stream(replyNodeType->responseData());
                        stream.setByteOrder(QDataStream::LittleEndian);
                        quint16 payloadLenght = 0; quint8 parameter = 0; quint8 nodeType = 0;
                        stream >> payloadLenght >> parameter >> nodeType;

                        m_networkConfiguration.nodeType = static_cast<Deconz::NodeType>(nodeType);
                        qCDebug(dcZigbeeController()) << "Request" << replyNodeType->command() << static_cast<Deconz::Parameter>(parameter)
                                                      << "finished successfully";
                        qCDebug(dcZigbeeController()) << m_networkConfiguration.nodeType;

                        // Read channel mask
                        ZigbeeInterfaceDeconzReply *replyChannelMask = requestReadParameter(Deconz::ParameterChannelMask);
                        connect(replyChannelMask, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyChannelMask](){
                            if (replyChannelMask->statusCode() != Deconz::StatusCodeSuccess) {
                                qCWarning(dcZigbeeController()) << "Request" << replyChannelMask->command() << Deconz::ParameterChannelMask
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
                            qCDebug(dcZigbeeController()) << "Request" << replyChannelMask->command() << static_cast<Deconz::Parameter>(parameter)
                                                          << "finished successfully";
                            qCDebug(dcZigbeeController()) << ZigbeeUtils::convertUint32ToHexString(m_networkConfiguration.channelMask);

                            // Read APS extended PAN ID
                            ZigbeeInterfaceDeconzReply *replyApsExtendedPanId = requestReadParameter(Deconz::ParameterApsExtendedPanId);
                            connect(replyApsExtendedPanId, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyApsExtendedPanId](){
                                if (replyApsExtendedPanId->statusCode() != Deconz::StatusCodeSuccess) {
                                    qCWarning(dcZigbeeController()) << "Request" << replyApsExtendedPanId->command() << Deconz::ParameterApsExtendedPanId
                                                                    << "finished with error" << replyApsExtendedPanId->statusCode();
                                    readNetworkParametersReply->m_statusCode = replyApsExtendedPanId->statusCode();
                                    readNetworkParametersReply->finished();
                                    return;
                                }

                                QDataStream stream(replyApsExtendedPanId->responseData());
                                stream.setByteOrder(QDataStream::LittleEndian);
                                quint16 payloadLenght = 0; quint8 parameter = 0; quint64 apsExtendedPanId = 0;
                                stream >> payloadLenght >> parameter >> apsExtendedPanId;

                                m_networkConfiguration.apsExtendedPanId = apsExtendedPanId;
                                qCDebug(dcZigbeeController()) << "Request" << replyApsExtendedPanId->command() << static_cast<Deconz::Parameter>(parameter)
                                                              << "finished successfully";
                                qCDebug(dcZigbeeController()) << ZigbeeUtils::convertUint64ToHexString(m_networkConfiguration.apsExtendedPanId);

                                // Read trust center address
                                ZigbeeInterfaceDeconzReply *replyTrustCenterAddress = requestReadParameter(Deconz::ParameterTrustCenterAddress);
                                connect(replyTrustCenterAddress, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyTrustCenterAddress](){
                                    if (replyTrustCenterAddress->statusCode() != Deconz::StatusCodeSuccess) {
                                        qCWarning(dcZigbeeController()) << "Request" << replyTrustCenterAddress->command() << Deconz::ParameterTrustCenterAddress
                                                                        << "finished with error" << replyTrustCenterAddress->statusCode();
                                        readNetworkParametersReply->m_statusCode = replyTrustCenterAddress->statusCode();
                                        readNetworkParametersReply->finished();
                                        return;
                                    }

                                    QDataStream stream(replyTrustCenterAddress->responseData());
                                    stream.setByteOrder(QDataStream::LittleEndian);
                                    quint16 payloadLenght = 0; quint8 parameter = 0; quint64 trustCenterAddress = 0;
                                    stream >> payloadLenght >> parameter >> trustCenterAddress;

                                    m_networkConfiguration.trustCenterAddress = ZigbeeAddress(trustCenterAddress);
                                    qCDebug(dcZigbeeController()) << "Request" << replyTrustCenterAddress->command() << static_cast<Deconz::Parameter>(parameter)
                                                                  << "finished successfully";
                                    qCDebug(dcZigbeeController()) << m_networkConfiguration.trustCenterAddress;

                                    // Read security mode
                                    ZigbeeInterfaceDeconzReply *replySecurityMode = requestReadParameter(Deconz::ParameterSecurityMode);
                                    connect(replySecurityMode, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replySecurityMode](){
                                        if (replySecurityMode->statusCode() != Deconz::StatusCodeSuccess) {
                                            qCWarning(dcZigbeeController()) << "Request" << replySecurityMode->command() << Deconz::ParameterSecurityMode
                                                                            << "finished with error" << replySecurityMode->statusCode();
                                            readNetworkParametersReply->m_statusCode = replySecurityMode->statusCode();
                                            readNetworkParametersReply->finished();
                                            return;
                                        }

                                        QDataStream stream(replySecurityMode->responseData());
                                        stream.setByteOrder(QDataStream::LittleEndian);
                                        quint16 payloadLenght = 0; quint8 parameter = 0; quint8 securityMode = 0;
                                        stream >> payloadLenght >> parameter >> securityMode;

                                        m_networkConfiguration.securityMode = static_cast<Deconz::SecurityMode>(securityMode);
                                        qCDebug(dcZigbeeController()) << "Request" << replySecurityMode->command() << static_cast<Deconz::Parameter>(parameter)
                                                                      << "finished successfully";
                                        qCDebug(dcZigbeeController()) << m_networkConfiguration.securityMode;

                                        // Note: reading the network key returns "InavlidParameter". Might be for security reasons which is good!

                                        // Read channel
                                        ZigbeeInterfaceDeconzReply *replyChannel = requestReadParameter(Deconz::ParameterCurrentChannel);
                                        connect(replyChannel, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyChannel](){
                                            if (replyChannel->statusCode() != Deconz::StatusCodeSuccess) {
                                                qCWarning(dcZigbeeController()) << "Request" << replyChannel->command() << Deconz::ParameterCurrentChannel
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
                                            qCDebug(dcZigbeeController()) << "Request" << replyChannel->command() << static_cast<Deconz::Parameter>(parameter)
                                                                          << "finished successfully";
                                            qCDebug(dcZigbeeController()) << "Current channel:" << m_networkConfiguration.currentChannel;


                                            // Read permit join status
                                            ZigbeeInterfaceDeconzReply *replyPermitJoin = requestReadParameter(Deconz::ParameterPermitJoin);
                                            connect(replyPermitJoin, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyPermitJoin](){
                                                if (replyPermitJoin->statusCode() != Deconz::StatusCodeSuccess) {
                                                    qCWarning(dcZigbeeController()) << "Request" << replyPermitJoin->command() << Deconz::ParameterPermitJoin
                                                                                    << "finished with error" << replyPermitJoin->statusCode();
                                                    readNetworkParametersReply->m_statusCode = replyPermitJoin->statusCode();
                                                    readNetworkParametersReply->finished();
                                                    return;
                                                }

                                                QDataStream stream(replyPermitJoin->responseData());
                                                stream.setByteOrder(QDataStream::LittleEndian);
                                                quint16 payloadLenght = 0; quint8 parameter = 0;
                                                stream >> payloadLenght >> parameter;
                                                //m_networkConfiguration.currentChannel = channel;
                                                qCDebug(dcZigbeeController()) << "Request" << replyPermitJoin->command() << static_cast<Deconz::Parameter>(parameter)
                                                                              << "finished successfully" << ZigbeeUtils::convertByteArrayToHexString(replyPermitJoin->responseData());


                                                // Read protocol version
                                                ZigbeeInterfaceDeconzReply *replyProtocolVersion = requestReadParameter(Deconz::ParameterProtocolVersion);
                                                connect(replyProtocolVersion, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyProtocolVersion](){
                                                    if (replyProtocolVersion->statusCode() != Deconz::StatusCodeSuccess) {
                                                        qCWarning(dcZigbeeController()) << "Request" << replyProtocolVersion->command() << Deconz::ParameterProtocolVersion
                                                                                        << "finished with error" << replyProtocolVersion->statusCode();
                                                        readNetworkParametersReply->m_statusCode = replyProtocolVersion->statusCode();
                                                        readNetworkParametersReply->finished();
                                                        return;
                                                    }

                                                    QDataStream stream(replyProtocolVersion->responseData());
                                                    stream.setByteOrder(QDataStream::LittleEndian);
                                                    quint16 payloadLenght = 0; quint8 parameter = 0; quint16 protocolVersion = 0;
                                                    stream >> payloadLenght >> parameter >> protocolVersion;
                                                    m_networkConfiguration.protocolVersion = protocolVersion;
                                                    qCDebug(dcZigbeeController()) << "Request" << replyProtocolVersion->command() << static_cast<Deconz::Parameter>(parameter)
                                                                                  << "finished successfully";
                                                    qCDebug(dcZigbeeController()) << "Protocol version:" << ZigbeeUtils::convertUint16ToHexString(m_networkConfiguration.protocolVersion);

                                                    // Read network updat id
                                                    ZigbeeInterfaceDeconzReply *replyNetworkUpdateId = requestReadParameter(Deconz::ParameterNetworkUpdateId);
                                                    connect(replyNetworkUpdateId, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyNetworkUpdateId](){
                                                        if (replyNetworkUpdateId->statusCode() != Deconz::StatusCodeSuccess) {
                                                            qCWarning(dcZigbeeController()) << "Request" << replyNetworkUpdateId->command() << Deconz::ParameterNetworkUpdateId
                                                                                            << "finished with error" << replyNetworkUpdateId->statusCode();
                                                            readNetworkParametersReply->m_statusCode = replyNetworkUpdateId->statusCode();
                                                            readNetworkParametersReply->finished();
                                                            return;
                                                        }

                                                        QDataStream stream(replyNetworkUpdateId->responseData());
                                                        stream.setByteOrder(QDataStream::LittleEndian);
                                                        quint16 payloadLenght = 0; quint8 parameter = 0; quint8 networkUpdateId = 0;
                                                        stream >> payloadLenght >> parameter >> networkUpdateId;
                                                        m_networkConfiguration.networkUpdateId = networkUpdateId;
                                                        qCDebug(dcZigbeeController()) << "Request" << replyNetworkUpdateId->command() << static_cast<Deconz::Parameter>(parameter)
                                                                                      << "finished successfully";
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

                                                        // Read watchdog timeout
                                                        ZigbeeInterfaceDeconzReply *replyWatchdogTimeout = requestReadParameter(Deconz::ParameterWatchdogTtl);
                                                        connect(replyWatchdogTimeout, &ZigbeeInterfaceDeconzReply::finished, this, [this, readNetworkParametersReply, replyWatchdogTimeout](){
                                                            if (replyWatchdogTimeout->statusCode() != Deconz::StatusCodeSuccess) {
                                                                qCWarning(dcZigbeeController()) << "Request" << replyWatchdogTimeout->command() << Deconz::ParameterWatchdogTtl
                                                                                                << "finished with error" << replyWatchdogTimeout->statusCode();
                                                                readNetworkParametersReply->m_statusCode = replyWatchdogTimeout->statusCode();
                                                                readNetworkParametersReply->finished();
                                                                return;
                                                            }

                                                            QDataStream stream(replyWatchdogTimeout->responseData());
                                                            stream.setByteOrder(QDataStream::LittleEndian);
                                                            quint16 payloadLenght = 0; quint8 parameter = 0; quint32 watchdogTimeout = 0;
                                                            stream >> payloadLenght >> parameter >> watchdogTimeout;
                                                            m_networkConfiguration.watchdogTimeout = watchdogTimeout;
                                                            qCDebug(dcZigbeeController()) << "Request" << replyWatchdogTimeout->command() << static_cast<Deconz::Parameter>(parameter)
                                                                                          << "finished successfully";
                                                            qCDebug(dcZigbeeController()) << "Watchdog timeout:" << m_networkConfiguration.watchdogTimeout;

                                                            // Note: this value describes how much seconds are left until the watchdog triggers. Reset it right the way
                                                            if (watchdogTimeout < 15) {
                                                                resetControllerWatchdog();
                                                            }

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
    state.aspDataConfirm = (deviceStateFlag & 0x04);
    state.aspDataIndication = (deviceStateFlag & 0x08);
    state.configurationChanged = (deviceStateFlag & 0x10);
    state.aspDataRequestFreeSlots = (deviceStateFlag & 0x20);
    return state;
}

void ZigbeeBridgeControllerDeconz::readDataIndication()
{
    ZigbeeInterfaceDeconzReply *reply = requestReadReceivedDataIndication();
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
        if (reply->statusCode() != Deconz::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Could not read data indication." << reply->statusCode();
            // FIXME: set an appropriate error
            return;
        }

        // ASP data indication received, process the content
        qCDebug(dcZigbeeController()) << "Reading data indication finished successfully";
        processDataIndication(reply->responseData());
    });
}

void ZigbeeBridgeControllerDeconz::readDataConfirm()
{
    ZigbeeInterfaceDeconzReply *reply = requestQuerySendDataConfirm();
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
        if (reply->statusCode() != Deconz::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Could not read data confirm." << reply->statusCode();
            // FIXME: set an appropriate error
            return;
        }

        // ASP data indication received, process the content
        qCDebug(dcZigbeeController()) << "Reading data confirm finished successfully";
        processDataConfirm(reply->responseData());
    });
}

void ZigbeeBridgeControllerDeconz::processDeviceState(DeconzDeviceState deviceState)
{
    qCDebug(dcZigbeeController()) << deviceState;

    if (m_networkState != deviceState.networkState) {
        qCDebug(dcZigbeeController()) << "Network state changed" << deviceState.networkState;
        m_networkState = deviceState.networkState;
        emit networkStateChanged(m_networkState);
    }

    if (m_aspFreeSlotsAvailable != deviceState.aspDataRequestFreeSlots) {
        m_aspFreeSlotsAvailable = deviceState.aspDataRequestFreeSlots;

        // FIXME: if changed to true, send next asp data request

    }

    if (m_networkState != Deconz::NetworkStateConnected)
        return;

    // Note: read a data indication before a confirmation since the confirmation arrives after a related indication normally

    // Check if we have to read a data indication message
    if (deviceState.aspDataIndication) {
        readDataIndication();
    }

    // Check if we have a response to read for a request
    if (deviceState.aspDataConfirm) {
        readDataConfirm();
    }

}

void ZigbeeBridgeControllerDeconz::processDataIndication(const QByteArray &data)
{
    // ASP data indication
    QDataStream stream(data);
    stream.setByteOrder(QDataStream::LittleEndian);
    quint16 payloadLenght = 0; quint8 deviceStateFlag = 0; quint8 reserved = 0; quint16 asduLength = 0;

    DeconzApsDataIndication indication;
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

    emit aspDataIndicationReceived(indication);

    // Process the device state in order to check if we have to request another indication
    DeconzDeviceState deviceState = parseDeviceStateFlag(deviceStateFlag);
    if (deviceState.aspDataIndication) {
        readDataIndication();
    }
}

void ZigbeeBridgeControllerDeconz::processDataConfirm(const QByteArray &data)
{
    QDataStream stream(data);
    stream.setByteOrder(QDataStream::LittleEndian);
    DeconzApsDataConfirm confirm;
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

    emit aspDataConfirmReceived(confirm);

    // Process the device state in order to check if we have to request another indication
    DeconzDeviceState deviceState = parseDeviceStateFlag(deviceStateFlag);
    if (deviceState.aspDataConfirm) {
        readDataConfirm();
    }
}

void ZigbeeBridgeControllerDeconz::onInterfaceAvailableChanged(bool available)
{
    if (available) {
        // FIXME: only start if the protocol version is >= 0x0108
        m_watchdogTimer->start();
    } else {
        // Clean up any pending replies
        foreach (quint8 id, m_pendingReplies.keys()) {
            ZigbeeInterfaceDeconzReply *reply = m_pendingReplies.take(id);
            reply->abort();
        }

        m_watchdogTimer->stop();
    }

    setAvailable(available);
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
    //qCDebug(dcZigbeeController()) << "Interface message received" << command << "SQN:" << sequenceNumber
    //                              << status << "Frame length:" << frameLength << ZigbeeUtils::convertByteArrayToHexString(data);

    // Check if this is an interface response for a pending reply
    if (m_pendingReplies.contains(sequenceNumber) && m_pendingReplies.value(sequenceNumber)->command() == command) {
        ZigbeeInterfaceDeconzReply *reply = m_pendingReplies.take(sequenceNumber);
        reply->m_responseData = data;
        reply->m_statusCode = status;
        reply->finished();
        return;
    }

    // Note: we got a notification, lets set the current sequence number to the notification id,
    //       so the next request will be a continuouse increase

    m_sequenceNumber = sequenceNumber;

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
        break;
    }
    case Deconz::CommandSimplifiedBeacon: {
        // FIXME: parse the data and print info
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
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [reply](){
        if (reply->statusCode() != Deconz::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Could not reset the application watchdog on the deCONZ controller." << reply->statusCode();
            return;
        }
        qCDebug(dcZigbeeController()) << "Reset application watchdog on the deCONZ controller successfully";
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

    debug.nospace() << "Confirm=" << static_cast<int>(deviceState.aspDataConfirm) << ", ";
    debug.nospace() << "Indication=" << static_cast<int>(deviceState.aspDataIndication) << ", ";
    debug.nospace() << "ConfigChanged=" << static_cast<int>(deviceState.configurationChanged) << ", ";
    debug.nospace() << "CanSend=" << deviceState.aspDataRequestFreeSlots << ")";
    return debug.space();
}

QDebug operator<<(QDebug debug, const DeconzApsDataConfirm &confirm)
{
    debug.nospace() << "ASP.Confirm(";
    debug.nospace() << "Request ID: " << confirm.requestId << ", ";

    if (confirm.destinationAddressMode == Zigbee::DestinationAddressModeGroup)
        debug.nospace() << "Group address:" << ZigbeeUtils::convertUint16ToHexString(confirm.destinationShortAddress) << ", ";

    if (confirm.destinationAddressMode == Zigbee::DestinationAddressModeShortAddress)
        debug.nospace() << "NWK address:" << ZigbeeUtils::convertUint16ToHexString(confirm.destinationShortAddress) << ", ";

    if (confirm.destinationAddressMode == Zigbee::DestinationAddressModeIeeeAddress)
        debug.nospace() << "IEEE address:" << ZigbeeAddress(confirm.destinationIeeeAddress).toString() << ", ";

    debug.nospace() << "Destination EP:" << ZigbeeUtils::convertByteToHexString(confirm.destinationEndpoint) << ", ";
    debug.nospace() << "Source EP:" << ZigbeeUtils::convertByteToHexString(confirm.sourceEndpoint) << ", ";
    debug.nospace() << static_cast<Zigbee::ZigbeeStatus>(confirm.zigbeeStatusCode);
    debug.nospace() << ")";

    return debug.space();
}

QDebug operator<<(QDebug debug, const DeconzApsDataIndication &indication)
{
    debug.nospace() << "ASP.Indication(";
    if (indication.destinationAddressMode == Zigbee::DestinationAddressModeGroup)
        debug.nospace() << "Group address:" << ZigbeeUtils::convertUint16ToHexString(indication.destinationShortAddress) << ", ";

    if (indication.destinationAddressMode == Zigbee::DestinationAddressModeShortAddress)
        debug.nospace() << "NWK address:" << ZigbeeUtils::convertUint16ToHexString(indication.destinationShortAddress) << ", ";

    if (indication.destinationAddressMode == Zigbee::DestinationAddressModeIeeeAddress)
        debug.nospace() << "IEEE address:" << ZigbeeAddress(indication.destinationIeeeAddress).toString() << ", ";

    debug.nospace() << "Destination EP:" << ZigbeeUtils::convertByteToHexString(indication.destinationEndpoint) << ", ";
    debug.nospace() << "Source EP:" << ZigbeeUtils::convertByteToHexString(indication.sourceEndpoint) << ", ";

    if (indication.sourceAddressMode == Zigbee::SourceAddressModeShortAddress || indication.sourceAddressMode == Zigbee::SourceAddressModeShortAndIeeeAddress)
        debug.nospace() << "Source NWK address:" << ZigbeeUtils::convertUint16ToHexString(indication.sourceShortAddress) << ", ";

    if (indication.sourceAddressMode == Zigbee::SourceAddressModeIeeeAddress || indication.sourceAddressMode == Zigbee::SourceAddressModeShortAndIeeeAddress)
        debug.nospace() << "Source IEEE address:" << ZigbeeAddress(indication.sourceIeeeAddress).toString() << ", ";

    debug.nospace() << static_cast<Zigbee::ZigbeeProfile>(indication.profileId) << ", ";
    if (indication.profileId == static_cast<quint16>(Zigbee::ZigbeeProfileDevice)) {
        debug.nospace() << static_cast<ZigbeeDeviceProfile::ZdoCommand>(indication.clusterId) << ", ";
    } else {
        debug.nospace() << static_cast<Zigbee::ClusterId>(indication.clusterId) << ", ";
    }

    debug.nospace() << "ASDU: " << ZigbeeUtils::convertByteArrayToHexString(indication.asdu) << ", ";
    debug.nospace() << "LQI: " << indication.lqi << ", ";
    debug.nospace() << "RSSI: " << indication.rssi << "dBm)";
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
    debug.nospace() << " - ASP Extended PAN ID:" << ZigbeeUtils::convertUint64ToHexString(configuration.apsExtendedPanId) << endl;
    debug.nospace() << " - Trust center IEEE address:" << configuration.trustCenterAddress.toString() << endl;
    debug.nospace() << " - Channel mask:" << ZigbeeChannelMask(configuration.channelMask) << endl;
    debug.nospace() << " - Channel:" << configuration.currentChannel << endl;
    debug.nospace() << " - Security mode:" << configuration.securityMode << endl;
    debug.nospace() << " - Protocol version:" << ZigbeeUtils::convertUint16ToHexString(configuration.protocolVersion) << endl;
    debug.nospace() << " - Network update ID:" << ZigbeeUtils::convertByteToHexString(configuration.networkUpdateId) << endl;
    debug.nospace() << " - Watchdog TTL:" << configuration.watchdogTimeout << endl;
    return debug.space();
}
