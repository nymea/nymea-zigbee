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

#include "zdo/zigbeedeviceprofile.h"
#include "zigbeenetworkdeconz.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"
#include "zigbeenetworkdatabase.h"

#include <QDataStream>

ZigbeeNetworkDeconz::ZigbeeNetworkDeconz(const QUuid &networkUuid, QObject *parent) :
    ZigbeeNetwork(networkUuid, parent)
{
    m_controller = new ZigbeeBridgeControllerDeconz(this);
    connect(m_controller, &ZigbeeBridgeControllerDeconz::availableChanged, this, &ZigbeeNetworkDeconz::onControllerAvailableChanged);
    connect(m_controller, &ZigbeeBridgeControllerDeconz::firmwareVersionChanged, this, &ZigbeeNetworkDeconz::firmwareVersionChanged);
    connect(m_controller, &ZigbeeBridgeControllerDeconz::apsDataConfirmReceived, this, &ZigbeeNetworkDeconz::onApsDataConfirmReceived);
    connect(m_controller, &ZigbeeBridgeControllerDeconz::apsDataIndicationReceived, this, &ZigbeeNetworkDeconz::onApsDataIndicationReceived);

    m_pollNetworkStateTimer = new QTimer(this);
    m_pollNetworkStateTimer->setInterval(1000);
    m_pollNetworkStateTimer->setSingleShot(false);
    connect(m_pollNetworkStateTimer, &QTimer::timeout, this, &ZigbeeNetworkDeconz::onPollNetworkStateTimeout);
}

ZigbeeBridgeController *ZigbeeNetworkDeconz::bridgeController() const
{
    if (!m_controller)
        return nullptr;

    return qobject_cast<ZigbeeBridgeController *>(m_controller);
}

Zigbee::ZigbeeBackendType ZigbeeNetworkDeconz::backendType() const
{
    return Zigbee::ZigbeeBackendTypeDeconz;
}

ZigbeeNetworkReply *ZigbeeNetworkDeconz::sendRequest(const ZigbeeNetworkRequest &request)
{
    ZigbeeNetworkReply *reply = createNetworkReply(request);
    // Send the request, and keep the reply until transposrt, zigbee trasmission and response arrived
    m_pendingReplies.insert(request.requestId(), reply);
    connect(reply, &ZigbeeNetworkReply::finished, this, [this, request](){
        m_pendingReplies.remove(request.requestId());
    });

    // Finish the reply right the way if the network is offline
    if (!m_controller->available() || state() == ZigbeeNetwork::StateOffline) {
        finishNetworkReply(reply, ZigbeeNetworkReply::ErrorNetworkOffline);
        return reply;
    }

    ZigbeeInterfaceDeconzReply *interfaceReply = m_controller->requestSendRequest(request);
    connect(interfaceReply, &ZigbeeInterfaceDeconzReply::finished, reply, [this, reply, interfaceReply](){
        if (interfaceReply->statusCode() != Deconz::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Could send request to controller. SQN:" << interfaceReply->sequenceNumber() << interfaceReply->statusCode();
            finishNetworkReply(reply, ZigbeeNetworkReply::ErrorInterfaceError);
            return;
        }

        // The request has been sent successfully to the device, start the timeout timer now
        startWaitingReply(reply);
    });

    return reply;
}

void ZigbeeNetworkDeconz::setPermitJoining(quint8 duration, quint16 address)
{
    if (duration > 0) {
        qCDebug(dcZigbeeNetwork()) << "Set permit join for" << duration << "s on" << ZigbeeUtils::convertUint16ToHexString(address);
    } else {
        qCDebug(dcZigbeeNetwork()) << "Disable permit join on"<< ZigbeeUtils::convertUint16ToHexString(address);
    }

    // Note: will be reseted if permit join will not work
    setPermitJoiningEnabled(duration > 0);
    setPermitJoiningDuration(duration);
    setPermitJoiningRemaining(duration);

    // Note: since compliance version >= 21 the value 255 is not any more endless.
    // We need to refresh the command on timeout repeatedly if the duration is longer than 254 s

    ZigbeeNetworkReply *reply = requestSetPermitJoin(address, duration);
    connect(reply, &ZigbeeNetworkReply::finished, this, [this, reply, duration, address](){
        if (reply->zigbeeApsStatus() != Zigbee::ZigbeeApsStatusSuccess) {
            qCWarning(dcZigbeeNetwork()) << "Could not set permit join to" << duration << ZigbeeUtils::convertUint16ToHexString(address) << reply->zigbeeApsStatus();
            setPermitJoiningEnabled(false);
            setPermitJoiningDuration(duration);
            return;
        }
        qCDebug(dcZigbeeNetwork()) << "Permit join request finished successfully";

        setPermitJoiningEnabled(duration > 0);
        setPermitJoiningDuration(duration);
        setPermitJoiningRemaining(duration);
        if (duration > 0) {
            m_permitJoinTimer->start();
        } else {
            m_permitJoinTimer->stop();
        }

        // Set the permit joining timeout network configuration parameter
        QByteArray parameterData;
        QDataStream stream(&parameterData, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << duration;

        ZigbeeInterfaceDeconzReply *reply = m_controller->requestWriteParameter(Deconz::ParameterPermitJoin, parameterData);
        connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [reply](){
            if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                qCWarning(dcZigbeeController()) << "Request" << reply->command() << "finished with error" << reply->statusCode();
                return;
            }

            qCDebug(dcZigbeeNetwork()) << "Set permit join configuration request finished" << reply->statusCode();
        });
    });
}

ZigbeeNetworkReply *ZigbeeNetworkDeconz::requestSetPermitJoin(quint16 shortAddress, quint8 duration)
{
    // Get the power descriptor
    ZigbeeNetworkRequest request;
    request.setRequestId(generateSequenceNumber());
    request.setDestinationAddressMode(Zigbee::DestinationAddressModeGroup);
    request.setDestinationShortAddress(static_cast<quint16>(shortAddress));
    request.setProfileId(Zigbee::ZigbeeProfileDevice); // ZDP
    request.setClusterId(ZigbeeDeviceProfile::MgmtPermitJoinRequest);
    request.setSourceEndpoint(0); // ZDO
    request.setRadius(10);

    // Build ASDU
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << request.requestId();
    stream << duration;
    stream << static_cast<quint8>(0x01); // TrustCenter significance, always force to 1 according to Spec.
    request.setTxOptions(Zigbee::ZigbeeTxOptions()); // no ACK for broadcasts
    request.setAsdu(asdu);

    qCDebug(dcZigbeeNetwork()) << "Send permit join request" << ZigbeeUtils::convertUint16ToHexString(request.destinationShortAddress()) << duration << "s";
    return sendRequest(request);
}

void ZigbeeNetworkDeconz::setCreateNetworkState(ZigbeeNetworkDeconz::CreateNetworkState state)
{
    if (m_createState == state)
        return;

    m_createState = state;
    qCDebug(dcZigbeeNetwork()) << "Create network state changed" << m_createState;

    switch (m_createState) {
    case CreateNetworkStateIdle:

        break;
    case CreateNetworkStateStopNetwork: {
        ZigbeeInterfaceDeconzReply *reply = m_controller->requestChangeNetworkState(Deconz::NetworkStateOffline);
        connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
            if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                qCWarning(dcZigbeeController()) << "Could not stop network for creating a new one. SQN:" << reply->sequenceNumber() << reply->statusCode();
                // FIXME: set an appropriate error
                return;
            }

            qCDebug(dcZigbeeNetwork()) << "Stop network finished successfully. SQN:" << reply->sequenceNumber();

            // Start polling the device state, should be Online -> Leaving -> Offline
            m_pollNetworkStateTimer->start();
        });
        break;
    }
    case CreateNetworkStateWriteConfiguration: {
        //  - Set coordinator
        //  - Set channel mask
        //  - Set predefined network PANID (0: let the firmware pick, 1: use the defined pan id)
        //  - Set NWK PANID
        //  - Set trust center address (coordinator address)
        //  - Set security mode
        //  - Set network key

        QByteArray paramData;
        QDataStream stream(&paramData, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << static_cast<quint8>(Deconz::NodeTypeCoordinator);
        qCDebug(dcZigbeeNetwork()) << "Configure bridge to" << Deconz::NodeTypeCoordinator;
        ZigbeeInterfaceDeconzReply *reply = m_controller->requestWriteParameter(Deconz::ParameterNodeType, paramData);
        connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
            if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                qCWarning(dcZigbeeController()) << "Could not write parameter. SQN:" << reply->sequenceNumber() << Deconz::ParameterNodeType << Deconz::NodeTypeCoordinator << reply->statusCode();
                // FIXME: set an appropriate error
                return;
            }

            qCDebug(dcZigbeeNetwork()) << "Configured successfully bridge to" << Deconz::NodeTypeCoordinator << "SQN:" << reply->sequenceNumber();

            QByteArray paramData;
            QDataStream stream(&paramData, QIODevice::WriteOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream << static_cast<quint32>(channelMask().toUInt32());
            qCDebug(dcZigbeeNetwork()) << "Configure channel mask" << channelMask();
            ZigbeeInterfaceDeconzReply *reply = m_controller->requestWriteParameter(Deconz::ParameterChannelMask, paramData);
            connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                    qCWarning(dcZigbeeController()) << "Could not write parameter. SQN:" << reply->sequenceNumber() << Deconz::ParameterChannelMask << reply->statusCode();
                    // FIXME: set an appropriate error
                    return;
                }

                qCDebug(dcZigbeeNetwork()) << "Configured channel mask successfully. SQN:" << reply->sequenceNumber();


                qCDebug(dcZigbeeNetwork()) << "Configure firmware to use predefined network PANID";
                QByteArray paramData;
                QDataStream stream(&paramData, QIODevice::WriteOnly);
                stream.setByteOrder(QDataStream::LittleEndian);
                stream << static_cast<quint8>(0x01);
                ZigbeeInterfaceDeconzReply *reply = m_controller->requestWriteParameter(Deconz::ParameterPredefinedNwkPanId, paramData);
                connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                    if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                        qCWarning(dcZigbeeController()) << "Could not write parameter. SQN:" << reply->sequenceNumber() << Deconz::ParameterPredefinedNwkPanId << reply->statusCode();
                        // FIXME: set an appropriate error
                        return;
                    }

                    qCDebug(dcZigbeeController()) << "Configured firmware to use predefined network PNAID successfully. SQN:" << reply->sequenceNumber();



                    qCDebug(dcZigbeeNetwork()) << "Configure network PANID" << panId() << ZigbeeUtils::convertUint16ToHexString(panId());
                    QByteArray paramData;
                    QDataStream stream(&paramData, QIODevice::WriteOnly);
                    stream.setByteOrder(QDataStream::LittleEndian);
                    stream << panId();
                    ZigbeeInterfaceDeconzReply *reply = m_controller->requestWriteParameter(Deconz::ParameterPanId, paramData);
                    connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                        if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                            qCWarning(dcZigbeeController()) << "Could not write parameter. SQN:" << reply->sequenceNumber() << Deconz::ParameterPanId << reply->statusCode();
                            // FIXME: set an appropriate error
                            return;
                        }

                        qCDebug(dcZigbeeController()) << "Configured network PANID successfully. SQN:" << reply->sequenceNumber();

                        QByteArray paramData;
                        QDataStream stream(&paramData, QIODevice::WriteOnly);
                        stream.setByteOrder(QDataStream::LittleEndian);
                        stream << m_controller->networkConfiguration().ieeeAddress.toUInt64();
                        qCDebug(dcZigbeeNetwork()) << "Configure trust center address" << m_controller->networkConfiguration().ieeeAddress.toString();
                        ZigbeeInterfaceDeconzReply *reply = m_controller->requestWriteParameter(Deconz::ParameterTrustCenterAddress, paramData);
                        connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                            if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                                qCWarning(dcZigbeeController()) << "Could not write parameter. SQN:" << reply->sequenceNumber() << Deconz::ParameterTrustCenterAddress << reply->statusCode();
                                // FIXME: set an appropriate error
                                return;
                            }

                            qCDebug(dcZigbeeController()) << "Configured trust center address successfully. SQN:" << reply->sequenceNumber();

                            QByteArray paramData;
                            QDataStream stream(&paramData, QIODevice::WriteOnly);
                            stream.setByteOrder(QDataStream::LittleEndian);
                            stream << static_cast<quint8>(Deconz::SecurityModeNoMasterButTrustCenterKey);
                            qCDebug(dcZigbeeNetwork()) << "Configure security mode" << Deconz::SecurityModeNoMasterButTrustCenterKey;
                            ZigbeeInterfaceDeconzReply *reply = m_controller->requestWriteParameter(Deconz::ParameterSecurityMode, paramData);
                            connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                                if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                                    qCWarning(dcZigbeeController()) << "Could not write parameter. SQN:" << reply->sequenceNumber() << Deconz::ParameterSecurityMode << reply->statusCode();
                                    // FIXME: set an appropriate error
                                    return;
                                }

                                qCDebug(dcZigbeeController()) << "Configured security mode successfully. SQN:" << reply->sequenceNumber();


                                qCDebug(dcZigbeeNetwork()) << "Configure network key" << securityConfiguration().networkKey().toString();
                                ZigbeeInterfaceDeconzReply *reply = m_controller->requestWriteParameter(Deconz::ParameterNetworkKey, securityConfiguration().networkKey().toByteArray());
                                connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                                    if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                                        qCWarning(dcZigbeeController()) << "Could not write parameter. SQN:" << reply->sequenceNumber() << Deconz::ParameterNetworkKey << reply->statusCode();
                                        // FIXME: set an appropriate error
                                        // Note: writing the network key fails all the time...
                                        //return;
                                    } else {
                                        qCDebug(dcZigbeeController()) << "Configured network key successfully. SQN:" << reply->sequenceNumber();
                                    }

                                    // Re-read the configurations
                                    // Read all network parameters
                                    ZigbeeInterfaceDeconzReply *reply = m_controller->readNetworkParameters();
                                    connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                                        if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                                            qCWarning(dcZigbeeController()) << "Could not read network parameters during network start up. SQN:" << reply->sequenceNumber() << reply->statusCode();
                                        }

                                        qCDebug(dcZigbeeController()) << m_controller->networkConfiguration();

                                        // Configuration finished, lets start the network
                                        setCreateNetworkState(CreateNetworkStateStartNetwork);
                                    });
                                });
                            });
                        });
                    });
                });
            });
        });
        break;
    }
    case CreateNetworkStateStartNetwork: {
        ZigbeeInterfaceDeconzReply *reply = m_controller->requestChangeNetworkState(Deconz::NetworkStateConnected);
        connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
            if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                qCWarning(dcZigbeeController()) << "Could not start network for creating a new one. SQN:" << reply->sequenceNumber() << reply->statusCode();
                // FIXME: set an appropriate error
                return;
            }

            qCDebug(dcZigbeeNetwork()) << "Start network finished successfully. SQN:" << reply->sequenceNumber();
            // Start polling the device state, should be Online -> Leaving -> Offline
            m_pollNetworkStateTimer->start();
        });
        break;
    }
    case CreateNetworkStateReadConfiguration: {
        // Read all network parameters
        ZigbeeInterfaceDeconzReply *reply = m_controller->readNetworkParameters();
        connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
            if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                qCWarning(dcZigbeeController()) << "Could not read network parameters during network start up. SQN:" << reply->sequenceNumber() << reply->statusCode();
                // FIXME: set an appropriate error
                return;
            }

            qCDebug(dcZigbeeNetwork()) << "Reading network parameters finished successfully. SQN:" << reply->sequenceNumber();

            qCDebug(dcZigbeeNetwork()) << "Network running with following configuration:";
            qCDebug(dcZigbeeNetwork()) << m_controller->networkConfiguration();

            setPanId(m_controller->networkConfiguration().panId);
            setExtendedPanId(m_controller->networkConfiguration().extendedPanId);
            setChannel(m_controller->networkConfiguration().currentChannel);



            setCreateNetworkState(CreateNetworkStateInitializeCoordinatorNode);
        });
        break;
    }
    case CreateNetworkStateInitializeCoordinatorNode: {
        if (m_coordinatorNode) {
            if (!macAddress().isNull() && m_coordinatorNode->extendedAddress() != macAddress()) {
                qCWarning(dcZigbeeNetwork()) << "The mac address of the coordinator has changed since the network has been set up.";
                qCWarning(dcZigbeeNetwork()) << "The network is bound to a specific controller. Since the controller has changed the network can not be started.";
                qCWarning(dcZigbeeNetwork()) << "Please factory reset the network or plug in the original controller.";
                setError(ZigbeeNetwork::ErrorHardwareModuleChanged);
                stopNetwork();
                return;
            }

            qCDebug(dcZigbeeNetwork()) << "We already have the coordinator node. Network starting done.";
            m_initializing = false;
            setNodeInformation(m_coordinatorNode, "deCONZ", "", bridgeController()->firmwareVersion());

            setState(StateRunning);
            setPermitJoining(0);
            return;
        }

        ZigbeeNode *coordinatorNode = createNode(m_controller->networkConfiguration().shortAddress, m_controller->networkConfiguration().ieeeAddress, this);
        m_coordinatorNode = coordinatorNode;

        // Network creation done when coordinator node is initialized
        connect(coordinatorNode, &ZigbeeNode::stateChanged, this, [this, coordinatorNode](ZigbeeNode::State state){
            if (state == ZigbeeNode::StateInitialized) {
                qCDebug(dcZigbeeNetwork()) << "Coordinator initialized successfully." << coordinatorNode;
                m_initializing = false;
                setState(StateRunning);
                setPermitJoining(0);
                return;
            }
        });

        coordinatorNode->startInitialization();
        addUnitializedNode(coordinatorNode);
    }
    }
}

void ZigbeeNetworkDeconz::runNetworkInitProcess()
{
    // - Read the network state (until success)
    // - Read the firmware version
    // - Read the network configuration parameters

    // - If network running and we don't have configurations, write them
    // - If network running and configurations match, we are done

    // Read the firmware version
    qCDebug(dcZigbeeNetwork()) << "Request current device state...";
    ZigbeeInterfaceDeconzReply *reply = m_controller->requestDeviceState();
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
        if (reply->statusCode() != Deconz::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Request" << reply->command() << "finished with error" << reply->statusCode();
            m_initRetry++;
            if (!m_controller->available()) {
                m_initRetry = 0;
                return;
            }

            if (m_initRetry < 10) {
                qCDebug(dcZigbeeNetwork()) << "Retry to initialize network" << m_initRetry << "/ 10";
                runNetworkInitProcess();
            } else {
                qCWarning(dcZigbeeNetwork()) << "Failed to read device state after 10 attempts. Giving up";
                m_controller->reconnectConrtroller();
            }
            return;
        }

        m_initRetry = 0;

        // Read all network parameters
        qCDebug(dcZigbeeNetwork()) << "Start reading controller network parameters...";
        ZigbeeInterfaceDeconzReply *reply = m_controller->readNetworkParameters();
        connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
            if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                qCWarning(dcZigbeeController()) << "Could not read network parameters during network start up." << reply->statusCode();
                m_initRetry++;
                if (m_initRetry < 3) {
                    qCDebug(dcZigbeeNetwork()) << "Retry to read network parameters" << m_initRetry << "/ 3";
                    runNetworkInitProcess();
                } else {
                    qCWarning(dcZigbeeNetwork()) << "Failed to read network parameters after 3 attempts. Giving up";
                    m_controller->reconnectConrtroller();
                }
                return;
            }

            qCDebug(dcZigbeeNetwork()) << "Reading network parameters finished successfully.";
            qCDebug(dcZigbeeNetwork()) << m_controller->networkConfiguration();

            m_protocolVersion = QString("%1.%2").arg(m_controller->networkConfiguration().protocolVersion >> 8 & 0xFF)
                    .arg(m_controller->networkConfiguration().protocolVersion & 0xFF);
            qCDebug(dcZigbeeNetwork()) << "Controller API protocol version" << ZigbeeUtils::convertUint16ToHexString(m_controller->networkConfiguration().protocolVersion) << m_protocolVersion;

            // Try read firmware version (changed with firmware 0x26680700)
            qCDebug(dcZigbeeNetwork()) << "Request current firmware version...";
            ZigbeeInterfaceDeconzReply *reply = m_controller->requestVersion();
            connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                QString firmwareVersion;
                if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                    qCWarning(dcZigbeeController()) << "Request" << reply->command() << "finished with error" << reply->statusCode();
                } else {
                    qCDebug(dcZigbeeNetwork()) << "Version request finished successfully" << ZigbeeUtils::convertByteArrayToHexString(reply->responseData());
                    // Note: version is an uint32 value, little endian, but we can read the individual bytes in reversed order
                    quint8 majorVersion = static_cast<quint8>(reply->responseData().at(3));
                    quint8 minorVersion = static_cast<quint8>(reply->responseData().at(2));
                    Deconz::Platform platform = static_cast<Deconz::Platform>(reply->responseData().at(1));
                    m_firmwareVersion = QString("%1.%2").arg(majorVersion).arg(minorVersion);
                    qCDebug(dcZigbeeNetwork()) << "Firmware version" << firmwareVersion << platform;

                }

                if (!m_firmwareVersion.isEmpty()) {
                    m_controller->setFirmwareVersionString(QString("%1 - %2").arg(m_firmwareVersion).arg(m_protocolVersion));
                } else {
                    m_controller->setFirmwareVersionString(m_protocolVersion);
                }

                qCDebug(dcZigbeeNetwork()) << "Reading current network state";
                ZigbeeInterfaceDeconzReply *reply = m_controller->requestDeviceState();
                connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                    if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                        qCWarning(dcZigbeeController()) << "Could not read device state during network start up. SQN:" << reply->sequenceNumber() << reply->statusCode();
                        // FIXME: set an appropriate error
                        return;
                    }

                    qCDebug(dcZigbeeNetwork()) << "Reading current network state finished successfully." << "SQN:" << reply->sequenceNumber();

                    QDataStream stream(reply->responseData());
                    stream.setByteOrder(QDataStream::LittleEndian);
                    quint8 deviceStateFlag = 0;
                    stream >> deviceStateFlag;
                    DeconzDeviceState deviceState = m_controller->parseDeviceStateFlag(deviceStateFlag);
                    qCDebug(dcZigbeeNetwork()) << deviceState;

                    // Update the device state in the controller
                    m_controller->processDeviceState(deviceState);

                    if (m_createNewNetwork) {
                        // Set offline
                        // Write configurations
                        // Set online
                        // Read configurations
                        // Create and initialize coordinator node
                        // Done. Save network
                        setCreateNetworkState(CreateNetworkStateStopNetwork);
                    } else {
                        // Get the network state and start the network if required
                        if (m_controller->networkState() == Deconz::NetworkStateConnected) {
                            qCDebug(dcZigbeeNetwork()) << "The network is already running.";
                            m_initializing = false;
                            setPermitJoiningEnabled(false);
                            // Set the permit joining timeout network configuration parameter
                            QByteArray parameterData;
                            QDataStream stream(&parameterData, QIODevice::WriteOnly);
                            stream.setByteOrder(QDataStream::LittleEndian);
                            stream << static_cast<quint8>(0);

                            ZigbeeInterfaceDeconzReply *reply = m_controller->requestWriteParameter(Deconz::ParameterPermitJoin, parameterData);
                            connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                                if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                                    qCWarning(dcZigbeeController()) << "Request" << reply->command() << "finished with error" << reply->statusCode();
                                    // FIXME: set an appropriate error
                                    return;
                                }

                                qCDebug(dcZigbeeNetwork()) << "Set permit join configuration request finished" << reply->statusCode();
                                setState(StateRunning);
                            });

                        } else if (m_controller->networkState() == Deconz::NetworkStateOffline) {
                            m_initializing = true;
                            qCDebug(dcZigbeeNetwork()) << "The network is offline. Lets start it";
                            setCreateNetworkState(CreateNetworkStateStartNetwork);
                        } else {
                            // The network is not running yet, lets wait for the state changed
                        }
                    }
                });
            });
        });
    });
}

void ZigbeeNetworkDeconz::startNetworkInternally()
{
    qCDebug(dcZigbeeNetwork()) << "Start zigbee network internally";

    m_createNewNetwork = false;
    // Check if we have to create a pan ID and select the channel
    if (panId() == 0 || !m_coordinatorNode) {
        setPanId(ZigbeeUtils::generateRandomPanId());
        qCDebug(dcZigbeeNetwork()) << "Generated new extended PAN ID" << panId() << ZigbeeUtils::convertUint16ToHexString(panId());
        m_createNewNetwork = true;
    }

    m_initRetry = 0;
    runNetworkInitProcess();
}

void ZigbeeNetworkDeconz::onControllerAvailableChanged(bool available)
{
    if (!available) {
        qCWarning(dcZigbeeNetwork()) << "Hardware controller is not available any more.";
        setError(ErrorHardwareUnavailable);
        m_initializing = false;
        setPermitJoiningEnabled(false);
        setState(StateOffline);
    } else {
        m_error = ErrorNoError;
        setPermitJoiningEnabled(false);
        setState(StateStarting);
        qCDebug(dcZigbeeNetwork()) << "Hardware controller is now available.";
        startNetworkInternally();
    }
}

void ZigbeeNetworkDeconz::onPollNetworkStateTimeout()
{
    // Stop the timer and make the request
    m_pollNetworkStateTimer->stop();

    switch (m_createState) {
    case CreateNetworkStateStopNetwork: {
        ZigbeeInterfaceDeconzReply *reply = m_controller->requestDeviceState();
        connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
            if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                qCWarning(dcZigbeeController()) << "Could not read device state during network start up." << reply->statusCode();
                // FIXME: set an appropriate error
                return;
            }

            //qCDebug(dcZigbeeNetwork()) << "Read device state finished successfully";
            QDataStream stream(reply->responseData());
            stream.setByteOrder(QDataStream::LittleEndian);
            quint8 deviceStateFlag = 0;
            stream >> deviceStateFlag;
            // Update the device state in the controller
            m_controller->processDeviceState(m_controller->parseDeviceStateFlag(deviceStateFlag));
            if (m_controller->networkState() == Deconz::NetworkStateOffline) {
                qCDebug(dcZigbeeNetwork()) << "Network stopped successfully for creation";
                // The network is now offline, continue with the state machine in one second (some grace period after network shutdown)
                QTimer::singleShot(1000, this, [=](){
                    setCreateNetworkState(CreateNetworkStateWriteConfiguration);
                });
            } else {
                // Not offline yet, continue poll
                m_pollNetworkStateTimer->start();
            }
        });
        break;
    }
    case CreateNetworkStateStartNetwork: {
        ZigbeeInterfaceDeconzReply *reply = m_controller->requestDeviceState();
        connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
            if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                qCWarning(dcZigbeeController()) << "Could not read device state during network start up. SQN:" << reply->sequenceNumber() << reply->statusCode();
                // FIXME: set an appropriate error
                return;
            }

            //qCDebug(dcZigbeeNetwork()) << "Read device state finished successfully. SQN:" << reply->sequenceNumber();
            QDataStream stream(reply->responseData());
            stream.setByteOrder(QDataStream::LittleEndian);
            quint8 deviceStateFlag = 0;
            stream >> deviceStateFlag;
            // Update the device state in the controller
            m_controller->processDeviceState(m_controller->parseDeviceStateFlag(deviceStateFlag));
            if (m_controller->networkState() == Deconz::NetworkStateConnected) {
                // The network is now online, continue with the state machine
                setCreateNetworkState(CreateNetworkStateReadConfiguration);
            } else if (m_controller->networkState() == Deconz::NetworkStateOffline) {
                qCWarning(dcZigbeeNetwork()) << "Failed to start the network.";
                setCreateNetworkState(CreateNetworkStateIdle);
                setState(StateOffline);
                setError(ErrorZigbeeError);
                return;
            } else {
                // Not offline yet, continue poll
                m_pollNetworkStateTimer->start();
            }
        });
        break;
    }
    default:
        break;
    }
}

void ZigbeeNetworkDeconz::onApsDataConfirmReceived(const Zigbee::ApsdeDataConfirm &confirm)
{
    ZigbeeNetworkReply *reply = m_pendingReplies.value(confirm.requestId);
    if (!reply) {
        qCWarning(dcZigbeeNetwork()) << "Received confirmation but could not find any reply. Ignoring the confirmation";
        return;
    }

    setReplyResponseError(reply, confirm.zigbeeStatusCode);
}

void ZigbeeNetworkDeconz::onApsDataIndicationReceived(const Zigbee::ApsdeDataIndication &indication)
{
    // Check if this indocation is related to any pending reply
    if (indication.profileId == Zigbee::ZigbeeProfileDevice) {
        handleZigbeeDeviceProfileIndication(indication);
        return;
    }

    // Else let the node handle this indication
    handleZigbeeClusterLibraryIndication(indication);
}

void ZigbeeNetworkDeconz::startNetwork()
{
    loadNetwork();

    if (!m_controller->enable(serialPortName(), serialBaudrate())) {
        setPermitJoiningEnabled(false);
        setState(StateOffline);
        setCreateNetworkState(CreateNetworkStateIdle);
        setError(ErrorHardwareUnavailable);
        return;
    }

    setPermitJoiningEnabled(false);
    m_initializing = true;

    // Note: wait for the controller available signal and start the initialization there
}

void ZigbeeNetworkDeconz::stopNetwork()
{
    ZigbeeInterfaceDeconzReply *reply = m_controller->requestChangeNetworkState(Deconz::NetworkStateOffline);
    setState(StateStopping);
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
        if (reply->statusCode() != Deconz::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Could not leave network. SQN:" << reply->sequenceNumber() << reply->statusCode();
            // FIXME: set an appropriate error
            return;
        }

        qCDebug(dcZigbeeNetwork()) << "Network left successfully. SQN:" << reply->sequenceNumber();
        setState(StateOffline);
        m_controller->disable();
    });
}

void ZigbeeNetworkDeconz::reset()
{
    qCDebug(dcZigbeeNetwork()) << "Reboot the controller. The stack will perform a restart.";
    m_controller->rebootController();
}

void ZigbeeNetworkDeconz::factoryResetNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Factory reset network and forget all information. This cannot be undone.";
    m_controller->disable();
    clearSettings();
    setState(StateUninitialized);
    qCDebug(dcZigbeeNetwork()) << "The factory reset is finished. Start restart with a fresh network.";
    startNetwork();
}

void ZigbeeNetworkDeconz::destroyNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Destroy network and delete the database";
    m_controller->disable();
    clearSettings();
}
