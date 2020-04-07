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

#include "zigbeenetworkdeconz.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeNetworkDeconz::ZigbeeNetworkDeconz(QObject *parent) :
    ZigbeeNetwork(parent)
{
    m_controller = new ZigbeeBridgeControllerDeconz(this);
    //connect(m_controller, &ZigbeeBridgeControllerDeconz::messageReceived, this, &ZigbeeNetworkDeconz::onMessageReceived);
    connect(m_controller, &ZigbeeBridgeControllerDeconz::availableChanged, this, &ZigbeeNetworkDeconz::onControllerAvailableChanged);

    m_pollNetworkStateTimer = new QTimer(this);
    m_pollNetworkStateTimer->setInterval(1000);
    m_pollNetworkStateTimer->setSingleShot(false);
    connect(m_pollNetworkStateTimer, &QTimer::timeout, this, &ZigbeeNetworkDeconz::onPollNetworkStateTimeout);
}

ZigbeeBridgeController *ZigbeeNetworkDeconz::bridgeController() const
{
    if (m_controller)
        return qobject_cast<ZigbeeBridgeController *>(m_controller);

    return nullptr;
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
                qCWarning(dcZigbeeController()) << "Could not stop network for creating a new one." << reply->statusCode();
                // FIXME: set an appropriate error
                return;
            }

            // Start polling the device state, should be Online -> Leaving -> Offline
            m_pollNetworkStateTimer->start();
        });
        break;
    }
    case CreateNetworkStateWriteConfiguration: {
        //  - Set coordinator
        //  - Set channel mask
        //  - Set APS extended PANID (zero to reset)
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
                qCWarning(dcZigbeeController()) << "Could not write parameter" << Deconz::ParameterNodeType << Deconz::NodeTypeCoordinator << reply->statusCode();
                // FIXME: set an appropriate error
                return;
            }

            qCDebug(dcZigbeeNetwork()) << "Configured successfully bridge to" << Deconz::NodeTypeCoordinator;

            QByteArray paramData;
            QDataStream stream(&paramData, QIODevice::WriteOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream << static_cast<quint32>(channelMask().toUInt32());
            qCDebug(dcZigbeeNetwork()) << "Configure channel mask" << channelMask();
            ZigbeeInterfaceDeconzReply *reply = m_controller->requestWriteParameter(Deconz::ParameterChannelMask, paramData);
            connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                    qCWarning(dcZigbeeController()) << "Could not write parameter" << Deconz::ParameterChannelMask << reply->statusCode();
                    // FIXME: set an appropriate error
                    return;
                }

                qCDebug(dcZigbeeNetwork()) << "Configured channel mask successfully";

                QByteArray paramData;
                QDataStream stream(&paramData, QIODevice::WriteOnly);
                stream << static_cast<quint64>(0);
                stream.setByteOrder(QDataStream::LittleEndian);
                qCDebug(dcZigbeeNetwork()) << "Configure APS extended PANID" << 0;
                ZigbeeInterfaceDeconzReply *reply = m_controller->requestWriteParameter(Deconz::ParameterApsExtendedPanId, paramData);
                connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                    if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                        qCWarning(dcZigbeeController()) << "Could not write parameter" << Deconz::ParameterApsExtendedPanId << reply->statusCode();
                        // FIXME: set an appropriate error
                        return;
                    }

                    qCDebug(dcZigbeeNetwork()) << "Configured APS extended PANID successfully";

                    QByteArray paramData;
                    QDataStream stream(&paramData, QIODevice::WriteOnly);
                    stream.setByteOrder(QDataStream::LittleEndian);
                    stream << m_controller->networkConfiguration().ieeeAddress.toUInt64();
                    qCDebug(dcZigbeeNetwork()) << "Configure trust center address" << m_controller->networkConfiguration().ieeeAddress.toString();
                    ZigbeeInterfaceDeconzReply *reply = m_controller->requestWriteParameter(Deconz::ParameterTrustCenterAddress, paramData);
                    connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                        if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                            qCWarning(dcZigbeeController()) << "Could not write parameter" << Deconz::ParameterTrustCenterAddress << reply->statusCode();
                            // FIXME: set an appropriate error
                            return;
                        }

                        qCDebug(dcZigbeeNetwork()) << "Configured trust center address successfully";

                        QByteArray paramData;
                        QDataStream stream(&paramData, QIODevice::WriteOnly);
                        stream.setByteOrder(QDataStream::LittleEndian);
                        stream << static_cast<quint8>(Deconz::SecurityModeNoMasterButTrustCenterKey);
                        qCDebug(dcZigbeeNetwork()) << "Configure security mode" << Deconz::SecurityModeNoMasterButTrustCenterKey;
                        ZigbeeInterfaceDeconzReply *reply = m_controller->requestWriteParameter(Deconz::ParameterSecurityMode, paramData);
                        connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                            if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                                qCWarning(dcZigbeeController()) << "Could not write parameter" << Deconz::ParameterSecurityMode << reply->statusCode();
                                // FIXME: set an appropriate error
                                return;
                            }

                            qCDebug(dcZigbeeNetwork()) << "Configured security mode successfully";


                            qCDebug(dcZigbeeNetwork()) << "Configure network key" << securityConfiguration().networkKey().toString();
                            ZigbeeInterfaceDeconzReply *reply = m_controller->requestWriteParameter(Deconz::ParameterNetworkKey, securityConfiguration().networkKey().toByteArray());
                            connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                                if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                                    qCWarning(dcZigbeeController()) << "Could not write parameter" << Deconz::ParameterNetworkKey << reply->statusCode();
                                    // FIXME: set an appropriate error
                                    // Note: writing the network key fails all the time...
                                    //return;
                                } else {
                                    qCDebug(dcZigbeeNetwork()) << "Configured network key successfully";
                                }

                                // Configuration finished, lets start the network
                                setCreateNetworkState(CreateNetworkStateStartNetwork);
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
                qCWarning(dcZigbeeController()) << "Could not start network for creating a new one." << reply->statusCode();
                // FIXME: set an appropriate error
                return;
            }

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
                qCWarning(dcZigbeeController()) << "Could not read network parameters during network start up." << reply->statusCode();
                // FIXME: set an appropriate error
                return;
            }

            qCDebug(dcZigbeeNetwork()) << "Reading network parameters finished successfully.";

            setPanId(m_controller->networkConfiguration().panId);
            setExtendedPanId(m_controller->networkConfiguration().extendedPanId);
            setChannel(m_controller->networkConfiguration().currentChannel);

            setCreateNetworkState(CreateNetworkStateInitializeCoordinatorNode);

        });
        break;
    }
    case CreateNetworkStateInitializeCoordinatorNode: {
        ZigbeeNodeDeconz *coordinatorNode = qobject_cast<ZigbeeNodeDeconz *>(createNode(this));
        coordinatorNode->setShortAddress(m_controller->networkConfiguration().shortAddress);
        coordinatorNode->setExtendedAddress(m_controller->networkConfiguration().ieeeAddress);

        // TODO: done when when node initialized
        m_coordinatorNode = coordinatorNode;
        addNode(coordinatorNode);

        setCreateNetworkState(CreateNetworkStateIdle);
        setState(StateRunning);

        //addUnitializedNode(coordinatorNode);
        //coordinatorNode->startInitialization();
    }
    }
}

ZigbeeNode *ZigbeeNetworkDeconz::createNode(QObject *parent)
{
    return new ZigbeeNodeDeconz(m_controller, parent);
}

void ZigbeeNetworkDeconz::setPermitJoiningInternal(bool permitJoining)
{
    //FIXME
    Q_UNUSED(permitJoining)
}

void ZigbeeNetworkDeconz::startNetworkInternally()
{
    qCDebug(dcZigbeeNetwork()) << "Start network internally";

    m_createNewNetwork = false;

    // Check if we have to create a pan ID and select the channel
    if (panId() == 0) {
        m_createNewNetwork = true;
        //setExtendedPanId(ZigbeeUtils::generateRandomPanId());
        //qCDebug(dcZigbeeNetwork()) << "Created new PAN ID:" << extendedPanId();
    }

    if (securityConfiguration().networkKey().isNull()) {
        m_createNewNetwork = true;
        qCDebug(dcZigbeeNetwork()) << "Create a new network key";
        ZigbeeNetworkKey key = ZigbeeNetworkKey::generateKey();
        m_securityConfiguration.setNetworkKey(key);
    }

    qCDebug(dcZigbeeNetwork()) << "Using" << securityConfiguration().networkKey() << "network link key";
    qCDebug(dcZigbeeNetwork()) << "Using" << securityConfiguration().globalTrustCenterLinkKey() << "global trust center link key";


    // - Read the firmware version
    // - Read the network configuration parameters
    // - Read the network state

    // - If network running and we don't have configurations, write them
    // - If network running and configurations match, we are done

    // Read the firmware version
    ZigbeeInterfaceDeconzReply *reply = m_controller->requestVersion();
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
        if (reply->statusCode() != Deconz::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Request" << reply->command() << "finished with error" << reply->statusCode();
            // FIXME: set an appropriate error
            return;
        }
        qCDebug(dcZigbeeNetwork()) << "Version request finished" << reply->statusCode() << ZigbeeUtils::convertByteArrayToHexString(reply->responseData());
        // Note: version is an uint32 value, little endian, but we can read the individual bytes in reversed order
        quint8 majorVersion = static_cast<quint8>(reply->responseData().at(3));
        quint8 minorVersion = static_cast<quint8>(reply->responseData().at(2));
        Deconz::Platform platform = static_cast<Deconz::Platform>(reply->responseData().at(1));
        QString firmwareVersion = QString("%1.%2").arg(majorVersion).arg(minorVersion);
        qCDebug(dcZigbeeNetwork()) << "Firmware version" << firmwareVersion << platform;

        // Read all network parameters
        ZigbeeInterfaceDeconzReply *reply = m_controller->readNetworkParameters();
        connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply, firmwareVersion](){
            if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                qCWarning(dcZigbeeController()) << "Could not read network parameters during network start up." << reply->statusCode();
                // FIXME: set an appropriate error
                return;
            }

            qCDebug(dcZigbeeNetwork()) << "Reading network parameters finished successfully.";
            QString protocolVersion = QString("%1.%2").arg(m_controller->networkConfiguration().protocolVersion >> 8 & 0xFF)
                    .arg(m_controller->networkConfiguration().protocolVersion & 0xFF);

            m_controller->setFirmwareVersionString(QString("%1 - %2").arg(firmwareVersion).arg(protocolVersion));

            ZigbeeInterfaceDeconzReply *reply = m_controller->requestDeviceState();
            connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                    qCWarning(dcZigbeeController()) << "Could not read device state during network start up." << reply->statusCode();
                    // FIXME: set an appropriate error
                    return;
                }

                qCDebug(dcZigbeeNetwork()) << "Read device state finished successfully";
                QDataStream stream(reply->responseData());
                stream.setByteOrder(QDataStream::LittleEndian);
                quint8 deviceStateFlag = 0;
                stream >> deviceStateFlag;
                // Update the device state in the controller
                m_controller->processDeviceState(m_controller->parseDeviceStateFlag(deviceStateFlag));

                if (m_createNewNetwork) {
                    setCreateNetworkState(CreateNetworkStateStopNetwork);
                } else {
                    // Get the network state and start the network if required
                    if (m_controller->networkState() == Deconz::NetworkStateConnected) {
                        qCDebug(dcZigbeeNetwork()) << "The network is already running.";
                        setState(StateRunning);
                    }
                }
            });
        });
    });
}

void ZigbeeNetworkDeconz::createNetwork()
{
    // Set offline
    setCreateNetworkState(CreateNetworkStateStopNetwork);

    // Write configurations

    // Set online

    // Read configurations

    // Create and initialize coordinator node
}

void ZigbeeNetworkDeconz::onControllerAvailableChanged(bool available)
{
    qCDebug(dcZigbeeNetwork()) << "Hardware controller is" << (available ? "now available" : "not available");

    if (!available) {
        setError(ErrorHardwareUnavailable);
        m_permitJoining = false;
        emit permitJoiningChanged(m_permitJoining);
        setState(StateOffline);
    } else {
        m_error = ErrorNoError;
        m_permitJoining = false;
        emit permitJoiningChanged(m_permitJoining);
        setState(StateStarting);
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

            qCDebug(dcZigbeeNetwork()) << "Read device state finished successfully";
            QDataStream stream(reply->responseData());
            stream.setByteOrder(QDataStream::LittleEndian);
            quint8 deviceStateFlag = 0;
            stream >> deviceStateFlag;
            // Update the device state in the controller
            m_controller->processDeviceState(m_controller->parseDeviceStateFlag(deviceStateFlag));
            if (m_controller->networkState() == Deconz::NetworkStateOffline) {
                qCDebug(dcZigbeeNetwork()) << "Network stopped successfully for creation";
                // The network is now offline, continue with the state machine
                setCreateNetworkState(CreateNetworkStateWriteConfiguration);
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
                qCWarning(dcZigbeeController()) << "Could not read device state during network start up." << reply->statusCode();
                // FIXME: set an appropriate error
                return;
            }

            qCDebug(dcZigbeeNetwork()) << "Read device state finished successfully";
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

void ZigbeeNetworkDeconz::startNetwork()
{
    loadNetwork();

    if (!m_controller->enable(serialPortName(), serialBaudrate())) {
        m_permitJoining = false;
        emit permitJoiningChanged(m_permitJoining);
        setState(StateOffline);
        //setStartingState(StartingStateNone);
        setError(ErrorHardwareUnavailable);
        return;
    }

    m_permitJoining = false;
    emit permitJoiningChanged(m_permitJoining);
    // Note: wait for the controller available signal and start the initialization there
}

void ZigbeeNetworkDeconz::stopNetwork()
{
    ZigbeeInterfaceDeconzReply *reply = m_controller->requestChangeNetworkState(Deconz::NetworkStateOffline);
    setState(StateStopping);
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
        if (reply->statusCode() != Deconz::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Could not leave network." << reply->statusCode();
            // FIXME: set an appropriate error
            return;
        }

        qCDebug(dcZigbeeNetwork()) << "Network left successfully";
        setState(StateOffline);
    });
}

void ZigbeeNetworkDeconz::reset()
{
    // TODO
}

void ZigbeeNetworkDeconz::factoryResetNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Factory reset network and forget all information. This cannot be undone.";
    clearSettings();
    setState(StateUninitialized);
    qCDebug(dcZigbeeNetwork()) << "The factory reset is finished. Start restart with a fresh network.";
    startNetwork();
}
