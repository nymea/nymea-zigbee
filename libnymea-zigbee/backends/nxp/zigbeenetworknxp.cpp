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

#include "zigbeenetworknxp.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"
#include "zigbeenetworkdatabase.h"

#include <QDataStream>

ZigbeeNetworkNxp::ZigbeeNetworkNxp(const QUuid &networkUuid, QObject *parent) :
    ZigbeeNetwork(networkUuid, parent)
{
    m_controller = new ZigbeeBridgeControllerNxp(this);
    connect(m_controller, &ZigbeeBridgeControllerNxp::availableChanged, this, &ZigbeeNetworkNxp::onControllerAvailableChanged);
    connect(m_controller, &ZigbeeBridgeControllerNxp::firmwareVersionChanged, this, &ZigbeeNetworkNxp::firmwareVersionChanged);
    connect(m_controller, &ZigbeeBridgeControllerNxp::interfaceNotificationReceived, this, &ZigbeeNetworkNxp::onInterfaceNotificationReceived);
    connect(m_controller, &ZigbeeBridgeControllerNxp::controllerStateChanged, this, &ZigbeeNetworkNxp::onControllerStateChanged);
    connect(m_controller, &ZigbeeBridgeControllerNxp::apsDataConfirmReceived, this, &ZigbeeNetworkNxp::onApsDataConfirmReceived);
    connect(m_controller, &ZigbeeBridgeControllerNxp::apsDataIndicationReceived, this, &ZigbeeNetworkNxp::onApsDataIndicationReceived);
    connect(m_controller, &ZigbeeBridgeControllerNxp::nodeLeft, this, &ZigbeeNetworkNxp::onNodeLeftIndication);
    connect(m_controller, &ZigbeeBridgeControllerNxp::canUpdateChanged, this, [](bool canUpdate){
        if (canUpdate) {
            qCDebug(dcZigbeeNetwork()) << "The controller of this network can be updated.";
        } else {
            qCDebug(dcZigbeeNetwork()) << "The controller of this network can not be updated.";
        }
    });

    connect(m_controller, &ZigbeeBridgeControllerNxp::updateRunningChanged, this, [this](bool updateRunning){
        if (updateRunning) {
            qCDebug(dcZigbeeNetwork()) << "The controller is performing an update.";
            setState(StateUpdating);
        }
    });
}

ZigbeeBridgeController *ZigbeeNetworkNxp::bridgeController() const
{
    if (!m_controller)
        return nullptr;

    return qobject_cast<ZigbeeBridgeController *>(m_controller);
}

Zigbee::ZigbeeBackendType ZigbeeNetworkNxp::backendType() const
{
    return Zigbee::ZigbeeBackendTypeNxp;
}

ZigbeeNetworkReply *ZigbeeNetworkNxp::sendRequest(const ZigbeeNetworkRequest &request)
{
    ZigbeeNetworkReply *reply = createNetworkReply(request);
    // Send the request, and keep the reply until transposrt, zigbee trasmission and response arrived
    connect(reply, &ZigbeeNetworkReply::finished, this, [this, reply](){
        if (!m_pendingReplies.values().contains(reply)) {
            //qCWarning(dcZigbeeNetwork()) << "#### Reply finished but not in the pending replies list" << reply;
            return;
        }
        quint8 requestId = m_pendingReplies.key(reply);
        m_pendingReplies.remove(requestId);
        //qCWarning(dcZigbeeNetwork()) << "#### Removed network reply" << reply << "ID:" << requestId << "Current reply count" << m_pendingReplies.count();
    });

    // Finish the reply right the way if the network is offline
    if (!m_controller->available() || state() == ZigbeeNetwork::StateOffline) {
        finishReplyInternally(reply, ZigbeeNetworkReply::ErrorNetworkOffline);
        return reply;
    }

    // Enqueu reply and send next one if we have enouth capacity
    m_replyQueue.enqueue(reply);
    qCDebug(dcZigbeeNetwork()) << "=== Pending replies count (enqueued)" << m_replyQueue.count();
    sendNextReply();

    return reply;
}

void ZigbeeNetworkNxp::setPermitJoining(quint8 duration, quint16 address)
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

    if (address == 0x0000) {
        // Only the coordinator is allowed to join the network
        qCDebug(dcZigbeeNetwork()) << "Set permit join in the coordinator node only to" << duration << "[s]";
        ZigbeeInterfaceNxpReply *reply = m_controller->requestSetPermitJoinCoordinator(duration);
        connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply, duration](){
            qCDebug(dcZigbeeNetwork()) << "Set permit join in the coordinator finished" << reply->status();
            if (reply->status() != Nxp::StatusSuccess) {
                qCWarning(dcZigbeeNetwork()) << "Failed to set permit join status in coordinator";
                setPermitJoiningEnabled(false);
                setPermitJoiningDuration(duration);
            } else {
                setPermitJoiningEnabled(duration > 0);
                setPermitJoiningDuration(duration);
                setPermitJoiningRemaining(duration);
                if (duration > 0) {
                    m_permitJoinTimer->start();
                }
            }
        });

        return;
    }

    // Note: since compliance version >= 21 the value 255 is not any more endless.
    // We need to refresh the command on timeout if the duration is longer

    ZigbeeNetworkReply *reply = requestSetPermitJoin(address, duration);
    connect(reply, &ZigbeeNetworkReply::finished, this, [this, reply, duration, address](){
        if (reply->zigbeeApsStatus() != Zigbee::ZigbeeApsStatusSuccess) {
            qCWarning(dcZigbeeNetwork()) << "Could not set permit join to" << duration << ZigbeeUtils::convertUint16ToHexString(address) << reply->zigbeeApsStatus();
            setPermitJoiningEnabled(false);
            setPermitJoiningDuration(duration);
            m_permitJoinTimer->stop();
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

        if (address == Zigbee::BroadcastAddressAllRouters || address == 0x0000) {
            qCDebug(dcZigbeeNetwork()) << "Set permit join in the coordinator node to" << duration << "[s]";
            ZigbeeInterfaceNxpReply *reply = m_controller->requestSetPermitJoinCoordinator(duration);
            connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [reply](){
                qCDebug(dcZigbeeNetwork()) << "Set permit join in the coordinator finished" << reply->status();
                if (reply->status() != Nxp::StatusSuccess) {
                    qCWarning(dcZigbeeNetwork()) << "Failed to set permit join status in coordinator";
                }
            });
        }
    });
}

void ZigbeeNetworkNxp::sendNextReply()
{
    if (m_replyQueue.isEmpty())
        return;

    if (m_currentReply)
        return;


    ZigbeeNetworkReply *reply = m_replyQueue.dequeue();
    qCDebug(dcZigbeeNetwork()) << "=== Pending replies count (dequeued)" << m_replyQueue.count();

    ZigbeeInterfaceNxpReply *interfaceReply = m_controller->requestSendRequest(reply->request());
    connect(interfaceReply, &ZigbeeInterfaceNxpReply::finished, reply, [this, reply, interfaceReply](){
        if (interfaceReply->status() != Nxp::StatusSuccess) {
            qCWarning(dcZigbeeController()) << "Could send request to controller. SQN:" << interfaceReply->sequenceNumber() << interfaceReply->status();
            finishReplyInternally(reply, ZigbeeNetworkReply::ErrorInterfaceError);
            return;
        }

        // Note: this is a special case for nxp coordinator requests, they don't send a confirm because the request will not be sent trough the network
        if (reply->request().destinationShortAddress() == 0x0000 && reply->request().profileId() == Zigbee::ZigbeeProfileDevice) {
            qCDebug(dcZigbeeNetwork()) << "Finish reply since there will be no CONFIRM for local node requests.";
            finishReplyInternally(reply);
            return;
        }

        quint8 networkRequestId = interfaceReply->responseData().at(0);
        //qCDebug(dcZigbeeNetwork()) << "Request has network SQN" << networkRequestId;
        reply->request().setRequestId(networkRequestId);
        //qCWarning(dcZigbeeNetwork()) << "#### Insert network reply" << reply << "ID:" << networkRequestId << "Current reply count" << m_pendingReplies.count();
        m_pendingReplies.insert(networkRequestId, reply);
        // The request has been sent successfully to the device, start the timeout timer now
        startWaitingReply(reply);
    });

}

void ZigbeeNetworkNxp::finishReplyInternally(ZigbeeNetworkReply *reply, ZigbeeNetworkReply::Error error)
{
    finishNetworkReply(reply, error);
    if (m_currentReply == reply) {
        m_currentReply = nullptr;
    }

    sendNextReply();
}

ZigbeeNetworkReply *ZigbeeNetworkNxp::requestSetPermitJoin(quint16 shortAddress, quint8 duration)
{
    // Get the power descriptor
    ZigbeeNetworkRequest request;
    request.setRequestId(generateSequenceNumber());
    request.setDestinationAddressMode(Zigbee::DestinationAddressModeGroup);
    request.setDestinationShortAddress(static_cast<quint16>(shortAddress));
    request.setProfileId(Zigbee::ZigbeeProfileDevice); // ZDP
    request.setClusterId(ZigbeeDeviceProfile::MgmtPermitJoinRequest);
    request.setSourceEndpoint(0); // ZDO
    request.setRadius(0);

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

bool ZigbeeNetworkNxp::processVersionReply(ZigbeeInterfaceNxpReply *reply)
{
    qCDebug(dcZigbeeNetwork()) << "Version reply finished" << reply->status();
    if (reply->timendOut()) {
        m_reconnectCounter++;
        if (m_reconnectCounter >= 3) {
            if (m_controller->canUpdate()) {
                qCDebug(dcZigbeeNetwork()) << "Unable to get controller version.";
                qCDebug(dcZigbeeNetwork()) << "Firmware update provider available. Try to flash the firmware, maybe that fixes the problem.";
                // FIXME: try 3 times, then give up or perform a factory flash
                if (!m_controller->updateRunning()) {
                    clearSettings();
                    qCDebug(dcZigbeeNetwork()) << "Starting firmware update...";
                    m_controller->startFirmwareUpdate();
                } else {
                    qCWarning(dcZigbeeNetwork()) << "There is already an update running...";
                }
                return false;
            } else {
                qCDebug(dcZigbeeNetwork()) << "Unable to get controller version. There is no firmware upgrade available. Giving up.";
                return false;
            }
        }

        qCWarning(dcZigbeeNetwork()) << "Failed to read firmware version. Retry" << m_reconnectCounter << "/ 3";
        ZigbeeInterfaceNxpReply *reply = m_controller->requestVersion();
        connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
            if (processVersionReply(reply)) {
                m_controller->refreshControllerState();
            }
        });

        return false;
    }

    QByteArray payload = reply->responseData();
    QDataStream stream(&payload, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    quint8 major = 0; quint8 minor = 0; quint8 patch = 0; quint16 sdkVersion = 0;
    stream >> major >> minor >> patch >> sdkVersion;

    QString version = QString("%1.%2.%3").arg(major).arg(minor).arg(patch);
    QString versionString = QString("%1 - %2").arg(version).arg(sdkVersion);
    qCDebug(dcZigbeeNetwork()) << "Controller version" << versionString;
    m_controller->setFirmwareVersion(versionString);

    if (m_controller->canUpdate()) {
        if (m_controller->updateAvailable(version)) {
            qCDebug(dcZigbeeNetwork()) << "There is an update available for the this zigbee controller:" << version << "-->" << m_controller->updateFirmwareVersion();
            qCDebug(dcZigbeeNetwork()) << "Starting firmware update...";
            m_controller->startFirmwareUpdate();
            return false;
        } else {
            qCDebug(dcZigbeeNetwork()) << "The current firmware is up to date.";
        }
    }

    return true;
}

void ZigbeeNetworkNxp::handleZigbeeDeviceProfileIndication(const Zigbee::ApsdeDataIndication &indication)
{
    // Check if this is a device announcement
    if (indication.clusterId == ZigbeeDeviceProfile::DeviceAnnounce) {
        QDataStream stream(indication.asdu);
        stream.setByteOrder(QDataStream::LittleEndian);
        quint8 sequenceNumber = 0; quint16 shortAddress = 0; quint64 ieeeAddress = 0; quint8 macFlag = 0;
        stream >> sequenceNumber >> shortAddress >> ieeeAddress >> macFlag;
        onDeviceAnnounced(shortAddress, ZigbeeAddress(ieeeAddress), macFlag);
        return;
    }

    if (indication.destinationShortAddress == Zigbee::BroadcastAddressAllNodes ||
            indication.destinationShortAddress == Zigbee::BroadcastAddressAllRouters ||
            indication.destinationShortAddress == Zigbee::BroadcastAddressAllNonSleepingNodes) {
        qCDebug(dcZigbeeNetwork()) << "Received unhandled broadcast ZDO indication" << indication;

        // FIXME: check what we can do with such messages like permit join
        return;
    }

    ZigbeeNode *node = getZigbeeNode(indication.sourceShortAddress);
    if (!node) {
        qCWarning(dcZigbeeNetwork()) << "Received a ZDO indication for an unrecognized node. There is no such node in the system. Ignoring indication" << indication;
        // FIXME: check if we want to create it since the device definitly exists within the network
        return;
    }

    // Let the node handle this indication
    handleNodeIndication(node, indication);
}

void ZigbeeNetworkNxp::handleZigbeeClusterLibraryIndication(const Zigbee::ApsdeDataIndication &indication)
{
    ZigbeeClusterLibrary::Frame frame = ZigbeeClusterLibrary::parseFrameData(indication.asdu);
    //qCDebug(dcZigbeeNetwork()) << "Handle ZCL indication" << indication << frame;

    // Get the node
    ZigbeeNode *node = getZigbeeNode(indication.sourceShortAddress);
    if (!node) {
        qCWarning(dcZigbeeNetwork()) << "Received a ZCL indication for an unrecognized node. There is no such node in the system. Ignoring indication" << indication;
        // FIXME: maybe create and init the node, since it is in the network, but not recognized
        // FIXME: maybe remove this node since we might have removed it but it did not respond, or we not explicitly allowed it to join.
        return;
    }
    // Let the node handle this indication
    handleNodeIndication(node, indication);
}

void ZigbeeNetworkNxp::onControllerAvailableChanged(bool available)
{
    qCDebug(dcZigbeeNetwork()) << "Controller is" << (available ? "now available" : "not available any more");
    if (available) {
        if (m_controller->canUpdate() && !m_controller->initiallyFlashed()) {
            qCDebug(dcZigbeeNetwork()) << "The firmware of the controller can be updated and has not been initially flashed. Perform a factory reset flash procedure...";
            m_controller->startFactoryResetUpdate();
            return;
        }

        m_reconnectCounter = 0;
        ZigbeeInterfaceNxpReply *reply = m_controller->requestVersion();
        connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
            // Retry or firmware upgrade if available
            if (!processVersionReply(reply))
                return;

            m_controller->refreshControllerState();
        });
    } else {
        setState(StateOffline);
    }
}

void ZigbeeNetworkNxp::onControllerStateChanged(ZigbeeBridgeControllerNxp::ControllerState controllerState)
{
    qCDebug(dcZigbeeNetwork()) << "Controller state changed" << controllerState;
    switch (controllerState) {
    case ZigbeeBridgeControllerNxp::ControllerStateRunning: {
        setState(StateStarting);
        m_reconnectCounter = 0;

        qCDebug(dcZigbeeNetwork()) << "Request controller version";
        ZigbeeInterfaceNxpReply *reply = m_controller->requestVersion();
        connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
            // Retry or firmware upgrade if available
            if (!processVersionReply(reply))
                return;

            qCDebug(dcZigbeeNetwork()) << "Get the current network state";
            ZigbeeInterfaceNxpReply *reply = m_controller->requestNetworkState();
            connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
                qCDebug(dcZigbeeNetwork()) << "Get network state response" << reply->status();
                //FIXME: error handling

                QByteArray data = reply->responseData();
                QDataStream payloadStream(&data, QIODevice::ReadOnly);
                payloadStream.setByteOrder(QDataStream::LittleEndian);
                quint16 networkAddress; quint64 ieeeAddress; quint8 channel;
                quint16 panId; quint64 extendedPanId;
                payloadStream >> networkAddress >> ieeeAddress >> channel >> panId >> extendedPanId;
                qCDebug(dcZigbeeNetwork()) << "Network running" << ZigbeeUtils::convertUint16ToHexString(networkAddress)
                                           << ZigbeeAddress(ieeeAddress).toString()
                                           << "Channel:" << channel
                                           << "PAN ID:" << panId << "Extended PAN ID:" << ZigbeeUtils::convertUint64ToHexString(extendedPanId);

                setPanId(panId);
                setChannel(channel);

                // Initialize the coordinator node if not already done.

                if (m_coordinatorNode) {
                    if (!macAddress().isNull() && ZigbeeAddress(ieeeAddress) != macAddress()) {
                        qCWarning(dcZigbeeNetwork()) << "The mac address of the coordinator has changed since the network has been set up.";
                        qCWarning(dcZigbeeNetwork()) << "The network is bound to a specific controller. Since the controller has changed the network can not be started.";
                        qCWarning(dcZigbeeNetwork()) << "Please factory reset the network or plug in the original controller.";
                        setError(ZigbeeNetwork::ErrorHardwareModuleChanged);
                        stopNetwork();
                        return;
                    }

                    qCDebug(dcZigbeeNetwork()) << "We already have the coordinator node. Network starting done.";
                    m_database->saveNode(m_coordinatorNode);
                    setPermitJoining(0);
                    setState(StateRunning);
                    return;
                }

                ZigbeeNode *coordinatorNode = createNode(networkAddress, ZigbeeAddress(ieeeAddress), this);
                m_coordinatorNode = coordinatorNode;

                // Network creation done when coordinator node is initialized
                connect(coordinatorNode, &ZigbeeNode::stateChanged, this, [this, coordinatorNode](ZigbeeNode::State state){
                    if (state == ZigbeeNode::StateInitialized) {
                        qCDebug(dcZigbeeNetwork()) << "Coordinator initialized successfully." << coordinatorNode;
                        /* Note: this currently has been hardcoded into the firmware. TODO: implement appropriate method for binding coordinator to group

                        ZigbeeClusterGroups *groupsCluster = coordinatorNode->getEndpoint(0x01)->inputCluster<ZigbeeClusterGroups>(ZigbeeClusterLibrary::ClusterIdGroups);
                        if (!groupsCluster) {
                            qCWarning(dcZigbeeNetwork()) << "Failed to get groups cluster from coordinator. The coordinator will not be in default group 0x0000";
                            setState(StateRunning);
                            setPermitJoining(0);
                            return;
                        }

                        ZigbeeClusterReply *reply = groupsCluster->addGroup(0x0000, "Default");
                        connect(reply, &ZigbeeClusterReply::finished, this, [=](){
                            if (reply->error() != ZigbeeClusterReply::ErrorNoError) {
                                qCWarning(dcZigbeeNetwork()) << "Failed to add coordinator to default group 0x0000. The coordinator will not be in default group 0x0000";
                            }
                            setState(StateRunning);
                            setPermitJoining(0);
                        });
                        */

                        setState(StateRunning);
                        setPermitJoining(0);
                        return;
                    }
                });

                coordinatorNode->startInitialization();
                addUnitializedNode(coordinatorNode);
            });
        });
        break;
    }
    case ZigbeeBridgeControllerNxp::ControllerStateStarting:
        setState(StateStarting);
        break;
    case ZigbeeBridgeControllerNxp::ControllerStateBooting:
        setState(StateStarting);
        break;
    case ZigbeeBridgeControllerNxp::ControllerStateRunningUninitialized: {
        // Create the database if there is no database available
        setState(StateStarting);
        if (!m_database) {
            QString networkDatabaseFileName = settingsDirectory().absolutePath() + QDir::separator() + QString("zigbee-network-%1.db").arg(networkUuid().toString().remove('{').remove('}'));
            qCDebug(dcZigbeeNetwork()) << "Using ZigBee network database" << QFileInfo(networkDatabaseFileName).fileName();
            m_database = new ZigbeeNetworkDatabase(this, networkDatabaseFileName, this);
        }

        qCDebug(dcZigbeeNetwork()) << "Request controller version";
        ZigbeeInterfaceNxpReply *reply = m_controller->requestVersion();
        connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
            // Retry or firmware upgrade if available
            if (!processVersionReply(reply))
                return;

            QByteArray payload = reply->responseData();
            QDataStream stream(&payload, QIODevice::ReadOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            quint8 major = 0; quint8 minor = 0; quint8 patch = 0; quint16 sdkVersion = 0;
            stream >> major >> minor >> patch >> sdkVersion;

            QString versionString = QString ("%1.%2.%3 - %4").arg(major).arg(minor).arg(patch).arg(sdkVersion);
            qCDebug(dcZigbeeNetwork()) << "Controller version" << versionString;
            m_controller->setFirmwareVersion(versionString);

            if (extendedPanId() == 0) {
                quint64 panId = ZigbeeUtils::generateRandomPanId();
                setExtendedPanId(panId);
                qCDebug(dcZigbeeNetwork()) << "There is no pan id set yet. Generated new PAN ID" << panId << ZigbeeUtils::convertUint64ToHexString(panId);

                ZigbeeSecurityConfiguration securityConfiguration;
                securityConfiguration.setNetworkKey(ZigbeeNetworkKey::generateKey());
                setSecurityConfiguration(securityConfiguration);

                qCDebug(dcZigbeeNetwork()) << "Generated new network key" << securityConfiguration.networkKey().toString();
            }

            qCDebug(dcZigbeeNetwork()) << "Set PAN ID" << ZigbeeUtils::convertUint64ToHexString(extendedPanId()) << extendedPanId();
            ZigbeeInterfaceNxpReply *reply = m_controller->requestSetPanId(extendedPanId());
            connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
                qCDebug(dcZigbeeNetwork()) << "Set PAN ID reply response" << reply->status();
                //FIXME: error handling

                qCDebug(dcZigbeeNetwork()) << "Set channel mask" << channelMask();
                ZigbeeInterfaceNxpReply *reply = m_controller->requestSetChannelMask(channelMask().toUInt32());
                connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
                    qCDebug(dcZigbeeNetwork()) << "Set channel mask reply response" << reply->status();
                    //FIXME: error handling

                    qCDebug(dcZigbeeNetwork()) << "Set global link key" << securityConfiguration().globalTrustCenterLinkKey().toString();
                    ZigbeeInterfaceNxpReply *reply = m_controller->requestSetSecurityKey(Nxp::KeyTypeGlobalLinkKey, securityConfiguration().globalTrustCenterLinkKey());
                    connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
                        qCDebug(dcZigbeeNetwork()) << "Set global link key response" << reply->status();
                        //FIXME: error handling

                        qCDebug(dcZigbeeNetwork()) << "Set network link key" << securityConfiguration().networkKey().toString();
                        ZigbeeInterfaceNxpReply *reply = m_controller->requestSetSecurityKey(Nxp::KeyTypeUniqueLinkKey, securityConfiguration().networkKey());
                        connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
                            qCDebug(dcZigbeeNetwork()) << "Set network link key response" << reply->status();
                            //FIXME: error handling

                            qCDebug(dcZigbeeNetwork()) << "Start the network";
                            ZigbeeInterfaceNxpReply *reply = m_controller->requestStartNetwork();
                            connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [reply](){
                                qCDebug(dcZigbeeNetwork()) << "Start network response" << reply->status();
                                //FIXME: error handling
                                qCDebug(dcZigbeeNetwork()) << "Waiting for the network to start...";
                            });
                        });
                    });
                });
            });
        });
        break;
    }
    case ZigbeeBridgeControllerNxp::ControllerStateNotRunning:
        setState(StateOffline);
        break;
    }
}

void ZigbeeNetworkNxp::onInterfaceNotificationReceived(Nxp::Notification notification, const QByteArray &payload)
{
    switch (notification) {
    case Nxp::NotificationNetworkStarted: {
        QByteArray data = payload;
        QDataStream payloadStream(&data, QIODevice::ReadOnly);
        payloadStream.setByteOrder(QDataStream::LittleEndian);
        quint16 networkAddress; quint64 ieeeAddress; quint8 channel;
        payloadStream >> networkAddress >> ieeeAddress >> channel;

        qCDebug(dcZigbeeNetwork()) << "Network started" << ZigbeeUtils::convertUint16ToHexString(networkAddress) << ZigbeeAddress(ieeeAddress).toString() << "Channel:" << channel;
        break;
    }
    default:
        qCWarning(dcZigbeeNetwork()) << "Unhandeld interface notification received" << notification << ZigbeeUtils::convertByteArrayToHexString(payload);
        break;
    }
}

void ZigbeeNetworkNxp::onApsDataConfirmReceived(const Zigbee::ApsdeDataConfirm &confirm)
{
    ZigbeeNetworkReply *reply = m_pendingReplies.value(confirm.requestId);
    if (!reply) {
        qCDebug(dcZigbeeNetwork()) << "Received confirmation but could not find any reply. Ignoring the confirmation";
        return;
    }

    setReplyResponseError(reply, static_cast<Zigbee::ZigbeeApsStatus>(confirm.zigbeeStatusCode));
}

void ZigbeeNetworkNxp::onApsDataIndicationReceived(const Zigbee::ApsdeDataIndication &indication)
{
    // Check if this indocation is related to any pending reply
    if (indication.profileId == Zigbee::ZigbeeProfileDevice) {
        handleZigbeeDeviceProfileIndication(indication);
        return;
    }

    // Let the node handle this indication
    handleZigbeeClusterLibraryIndication(indication);
}

void ZigbeeNetworkNxp::onDeviceAnnounced(quint16 shortAddress, ZigbeeAddress ieeeAddress, quint8 macCapabilities)
{
    qCDebug(dcZigbeeNetwork()) << "Device announced" << ZigbeeUtils::convertUint16ToHexString(shortAddress) << ieeeAddress.toString() << ZigbeeUtils::convertByteToHexString(macCapabilities);

    // Lets check if this device is in the uninitialized node list, if so, remove it and recreate the device
    if (hasUninitializedNode(ieeeAddress)) {
        qCWarning(dcZigbeeNetwork()) << "Device announced but there is already an initialization running for it. Remove the device and restart the initialization.";
        ZigbeeNode *uninitializedNode = getZigbeeNode(ieeeAddress);
        removeUninitializedNode(uninitializedNode);
    }

    if (hasNode(ieeeAddress)) {
        ZigbeeNode *node = getZigbeeNode(ieeeAddress);
        if (shortAddress == node->shortAddress()) {
            qCDebug(dcZigbeeNetwork()) << "Already known device announced and is reachable again" << node;
            setNodeReachable(node, true);
            return;
        } else {
            qCWarning(dcZigbeeNetwork()) << "Already known device announced with different network address. FIXME: update the network address or reinitialize node...";


            //removeNode(node);
        }
    }

    ZigbeeNode *node = createNode(shortAddress, ieeeAddress, macCapabilities, this);
    addUnitializedNode(node);
    node->startInitialization();
}

void ZigbeeNetworkNxp::onNodeLeftIndication(const ZigbeeAddress &ieeeAddress, bool rejoining)
{
    qCDebug(dcZigbeeNetwork()) << "Received node left indication" << ieeeAddress.toString() << "rejoining:" << rejoining;
    if (!hasNode(ieeeAddress)) {
        qCDebug(dcZigbeeNetwork()) << "Node left the network" << ieeeAddress.toString();
        return;
    }

    ZigbeeNode *node = getZigbeeNode(ieeeAddress);
    qCDebug(dcZigbeeNetwork()) << node << "left the network";
    removeNode(node);
}

void ZigbeeNetworkNxp::startNetwork()
{
    loadNetwork();

    setPermitJoiningEnabled(false);

    if (!m_controller->enable(serialPortName(), serialBaudrate())) {
        setState(StateOffline);
        setError(ErrorHardwareUnavailable);
        return;
    }

    // Wait for available signal...
}

void ZigbeeNetworkNxp::stopNetwork()
{
    m_controller->disable();
}

void ZigbeeNetworkNxp::reset()
{
    qCDebug(dcZigbeeNetwork()) << "Soft reset the controller. The stack will perform a restart.";
    ZigbeeInterfaceNxpReply *reply = m_controller->requestSoftResetController();
    connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [reply](){
        qCDebug(dcZigbeeNetwork()) << "Soft reset reply finished" << reply->status();
    });
}

void ZigbeeNetworkNxp::factoryResetNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Factory reset network and forget all information. This cannot be undone.";
    clearSettings();

    ZigbeeInterfaceNxpReply *reply = m_controller->requestFactoryResetController();
    connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [reply](){
        qCDebug(dcZigbeeNetwork()) << "Factory reset reply finished" << reply->status();
    });
}

void ZigbeeNetworkNxp::destroyNetwork()
{
    qCDebug(dcZigbeeNetwork()) << "Destroy network and delete the database";
    m_controller->disable();
    clearSettings();
}
