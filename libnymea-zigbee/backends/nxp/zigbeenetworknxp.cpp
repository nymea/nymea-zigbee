#include "zigbeenetworknxp.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeNetworkNxp::ZigbeeNetworkNxp(QObject *parent) :
    ZigbeeNetwork(parent)
{
    m_controller = new ZigbeeBridgeControllerNxp(this);
    connect(m_controller, &ZigbeeBridgeControllerNxp::availableChanged, this, &ZigbeeNetworkNxp::onControllerAvailableChanged);
    connect(m_controller, &ZigbeeBridgeControllerNxp::interfaceNotificationReceived, this, &ZigbeeNetworkNxp::onInterfaceNotificationReceived);
    connect(m_controller, &ZigbeeBridgeControllerNxp::controllerStateChanged, this, &ZigbeeNetworkNxp::onControllerStateChanged);
    //connect(m_controller, &ZigbeeBridgeControllerNxp::apsDataConfirmReceived, this, &ZigbeeNetworkNxp::onApsDataConfirmReceived);
    //connect(m_controller, &ZigbeeBridgeControllerNxp::apsDataIndicationReceived, this, &ZigbeeNetworkNxp::onApsDataIndicationReceived);
}

ZigbeeBridgeController *ZigbeeNetworkNxp::bridgeController() const
{
    if (!m_controller)
        return nullptr;

    return qobject_cast<ZigbeeBridgeController *>(m_controller);
}

ZigbeeNetworkReply *ZigbeeNetworkNxp::sendRequest(const ZigbeeNetworkRequest &request)
{
    Q_UNUSED(request)
    return nullptr;
}

ZigbeeNetworkReply *ZigbeeNetworkNxp::setPermitJoin(quint16 shortAddress, quint8 duration)
{
    Q_UNUSED(shortAddress)
    Q_UNUSED(duration)
    return nullptr;
}

void ZigbeeNetworkNxp::onControllerAvailableChanged(bool available)
{
    qCDebug(dcZigbeeNetwork()) << "Controller is" << (available ? "now available" : "not available any more");
    if (available) {
        // Get controller state

        // Get network state, depending on the controller state

        //reset();
        factoryResetNetwork();
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
        qCDebug(dcZigbeeNetwork()) << "Request controller version";
        ZigbeeInterfaceNxpReply *reply = m_controller->requestVersion();
        connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
            qCDebug(dcZigbeeNetwork()) << "Version reply finished" << reply->status();
            QByteArray payload = reply->responseData();
            QDataStream stream(&payload, QIODevice::ReadOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            quint8 major = 0; quint8 minor = 0; quint8 patch = 0; quint16 sdkVersion = 0;
            stream >> major >> minor >> patch >> sdkVersion;

            QString versionString = QString ("%1.%2.%3 - %4").arg(major).arg(minor).arg(patch).arg(sdkVersion);
            qCDebug(dcZigbeeNetwork()) << "Controller version" << versionString;
            m_controller->setFirmwareVersion(versionString);


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
                    qCDebug(dcZigbeeNetwork()) << "We already have the coordinator node. Network starting done.";
                    m_database->saveNode(m_coordinatorNode);
                    setState(StateRunning);
                    setPermitJoiningInternal(false);
                    return;
                }

                ZigbeeNode *coordinatorNode = createNode(networkAddress, ZigbeeAddress(ieeeAddress), this);
                m_coordinatorNode = coordinatorNode;

                // TODO: initialize

                m_database->saveNode(m_coordinatorNode);
                saveNetwork();
                setState(StateRunning);

//                // Network creation done when coordinator node is initialized
//                connect(coordinatorNode, &ZigbeeNode::stateChanged, this, [this, coordinatorNode](ZigbeeNode::State state){
//                    if (state == ZigbeeNode::StateInitialized) {
//                        qCDebug(dcZigbeeNetwork()) << "Coordinator initialized successfully." << coordinatorNode;
//                        m_initializing = false;
//                        setState(StateRunning);
//                        setPermitJoiningInternal(false);
//                        return;
//                    }
//                });

//                coordinatorNode->startInitialization();
//                addUnitializedNode(coordinatorNode);


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
        setState(StateStarting);
        qCDebug(dcZigbeeNetwork()) << "Request controller version";
        ZigbeeInterfaceNxpReply *reply = m_controller->requestVersion();
        connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
            qCDebug(dcZigbeeNetwork()) << "Version reply finished" << reply->status();
            //FIXME: error handling

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

    }


}

void ZigbeeNetworkNxp::setPermitJoiningInternal(bool permitJoining)
{
    qCDebug(dcZigbeeNetwork()) << "Set permit join internal" << permitJoining;

    quint8 duration = 0;
    if (permitJoining) {
        duration = 255;
    }

    // TODO: send broadcast message using ZDO and refrash

    qCDebug(dcZigbeeNetwork()) << "Set permit join in the coordinator node to" << duration << "[s]";
    ZigbeeInterfaceNxpReply *reply = m_controller->requestSetPermitJoinCoordinator(duration);
    connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply, permitJoining](){
        qCDebug(dcZigbeeNetwork()) << "Set permit join in the coordinator finished" << reply->status();
        m_permitJoining = permitJoining;
        emit permitJoiningChanged(m_permitJoining);
    });
}

void ZigbeeNetworkNxp::startNetwork()
{
    loadNetwork();

    if (!m_controller->enable(serialPortName(), serialBaudrate())) {
        m_permitJoining = false;
        emit permitJoiningChanged(m_permitJoining);
        setState(StateOffline);
        setError(ErrorHardwareUnavailable);
        return;
    }

    m_permitJoining = false;
    emit permitJoiningChanged(m_permitJoining);

    // Get current state and load information
}

void ZigbeeNetworkNxp::stopNetwork()
{

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
    ZigbeeInterfaceNxpReply *reply = m_controller->requestFactoryResetController();
    connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [reply](){
        qCDebug(dcZigbeeNetwork()) << "Factory reset reply finished" << reply->status();
    });
}
