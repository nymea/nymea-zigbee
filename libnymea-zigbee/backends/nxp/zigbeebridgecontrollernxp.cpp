#include "zigbeebridgecontrollernxp.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeBridgeControllerNxp::ZigbeeBridgeControllerNxp(QObject *parent) :
    ZigbeeBridgeController(parent)
{
    m_interface = new ZigbeeInterfaceNxp(this);
    connect(m_interface, &ZigbeeInterfaceNxp::availableChanged, this, &ZigbeeBridgeControllerNxp::onInterfaceAvailableChanged);
    connect(m_interface, &ZigbeeInterfaceNxp::packageReceived, this, &ZigbeeBridgeControllerNxp::onInterfacePackageReceived);
}

ZigbeeBridgeControllerNxp::~ZigbeeBridgeControllerNxp()
{
    qCDebug(dcZigbeeController()) << "Destroy controller";
}

ZigbeeBridgeControllerNxp::ControllerState ZigbeeBridgeControllerNxp::controllerState() const
{
    return m_controllerState;
}

void ZigbeeBridgeControllerNxp::refreshControllerState()
{
    // Get controller state
    qCDebug(dcZigbeeController()) << "Refresh controller state";
    ZigbeeInterfaceNxpReply *reply = requestControllerState();
    connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
        qCDebug(dcZigbeeNetwork()) << "Request controller state" << reply->status();

        if (reply->status() != Nxp::StatusSuccess) {
            qCWarning(dcZigbeeController()) << "Failed to request controller state" << reply->status();
            return;
        }

        m_controllerState = static_cast<ControllerState>(reply->responseData().at(0));
        qCDebug(dcZigbeeController()) << "Controller state changed" << m_controllerState;
        emit controllerStateChanged(m_controllerState);
    });
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestVersion()
{
    QByteArray message;
    bumpSequenceNumber();
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandGetVersion);
    stream << static_cast<quint8>(m_sequenceNumber);
    stream << static_cast<quint16>(0); // Frame length

    return createReply(Nxp::CommandGetVersion, m_sequenceNumber, "Request controller version", message, this);
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestControllerState()
{
    QByteArray message;
    bumpSequenceNumber();
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandGetControllerState);
    stream << static_cast<quint8>(m_sequenceNumber);
    stream << static_cast<quint16>(0); // Frame length

    return createReply(Nxp::CommandGetControllerState, m_sequenceNumber, "Request controller state", message, this);
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestSoftResetController()
{
    QByteArray message;
    bumpSequenceNumber();
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandSoftReset);
    stream << static_cast<quint8>(m_sequenceNumber);
    stream << static_cast<quint16>(0); // Frame length

    return createReply(Nxp::CommandSoftReset, m_sequenceNumber, "Request soft reset controller", message, this);
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestFactoryResetController()
{
    QByteArray message;
    bumpSequenceNumber();
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandFactoryReset);
    stream << static_cast<quint8>(m_sequenceNumber);
    stream << static_cast<quint16>(0); // Frame length

    return createReply(Nxp::CommandFactoryReset, m_sequenceNumber, "Request factory reset controller", message, this);
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestSetPanId(quint64 panId)
{
    QByteArray message;
    bumpSequenceNumber();
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandSetPanId);
    stream << static_cast<quint8>(m_sequenceNumber);
    stream << static_cast<quint16>(8); // Frame length
    stream << panId;

    return createReply(Nxp::CommandSetPanId, m_sequenceNumber, "Request set PAN ID", message, this);
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestSetChannelMask(quint32 channelMask)
{
    QByteArray message;
    bumpSequenceNumber();
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandSetChannelMask);
    stream << static_cast<quint8>(m_sequenceNumber);
    stream << static_cast<quint16>(4); // Frame length
    stream << channelMask;

    return createReply(Nxp::CommandSetChannelMask, m_sequenceNumber, "Request set channel mask", message, this);
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestSetSecurityKey(Nxp::KeyType keyType, const ZigbeeNetworkKey &key)
{
    QByteArray message;
    bumpSequenceNumber();
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandSetSecurityKey);
    stream << static_cast<quint8>(m_sequenceNumber);
    stream << static_cast<quint16>(17); // Frame length
    stream << static_cast<quint8>(keyType);
    QByteArray keyData = key.toByteArray();
    for (int i = 0; i < 16; i++) {
        stream << static_cast<quint8>(keyData.at(i));
    }

    return createReply(Nxp::CommandSetSecurityKey, m_sequenceNumber, "Request set security key", message, this);
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestStartNetwork()
{
    QByteArray message;
    bumpSequenceNumber();
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandStartNetwork);
    stream << static_cast<quint8>(m_sequenceNumber);
    stream << static_cast<quint16>(0); // Frame length

    return createReply(Nxp::CommandStartNetwork, m_sequenceNumber, "Request start network", message, this);
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestNetworkState()
{
    QByteArray message;
    bumpSequenceNumber();
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandGetNetworkState);
    stream << static_cast<quint8>(m_sequenceNumber);
    stream << static_cast<quint16>(0); // Frame length

    return createReply(Nxp::CommandGetNetworkState, m_sequenceNumber, "Request network state", message, this);
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestSetPermitJoinCoordinator(quint8 duration)
{
    QByteArray message;
    bumpSequenceNumber();
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandSetPermitJoinCoordinator);
    stream << static_cast<quint8>(m_sequenceNumber);
    stream << static_cast<quint16>(1); // Frame length
    stream << duration;

    return createReply(Nxp::CommandSetPermitJoinCoordinator, m_sequenceNumber, "Request set permit join in coordinator", message, this);
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestSendRequest(const ZigbeeNetworkRequest &request)
{
    ZigbeeInterfaceNxpReply *interfaceReply = nullptr;
    qCDebug(dcZigbeeAps()) << "APSDE-DATA.request" << request;

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

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::createReply(Nxp::Command command, quint8 sequenceNumber, const QString &requestName, const QByteArray &requestData, QObject *parent)
{
    // Create the reply
    ZigbeeInterfaceNxpReply *reply = new ZigbeeInterfaceNxpReply(command, parent);
    reply->m_requestName = requestName;
    reply->m_requestData = requestData;
    reply->m_sequenceNumber = sequenceNumber;
    // Make sure we clean up on timeout
    connect(reply, &ZigbeeInterfaceNxpReply::timeout, this, [reply](){
        qCWarning(dcZigbeeController()) << "Reply timeout" << reply;
    });

    // Auto delete the object on finished
    connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
        reply->deleteLater();
        if (m_currentReply == reply) {
            m_currentReply = nullptr;
            QMetaObject::invokeMethod(this, "sendNextRequest", Qt::QueuedConnection);
        }
    });

    qCDebug(dcZigbeeController()) << "Enqueue request" << reply->command() << "SQN:" << reply->sequenceNumber();
    m_replyQueue.enqueue(reply);

    QMetaObject::invokeMethod(this, "sendNextRequest", Qt::QueuedConnection);
    return reply;
}

void ZigbeeBridgeControllerNxp::bumpSequenceNumber()
{
    m_sequenceNumber += 1;
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestEnqueueSendDataGroup(quint8 requestId, quint16 groupAddress, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius)
{
    Q_UNUSED(txOptions)
    Q_ASSERT_X(asdu.length() <= 127, "ASDU", "ASDU package length has to <= 127 bytes");

    QByteArray payload;
    QDataStream payloadStream(&payload, QIODevice::WriteOnly);
    payloadStream.setByteOrder(QDataStream::LittleEndian);
    payloadStream << requestId;
    payloadStream << static_cast<quint8>(Zigbee::DestinationAddressModeGroup);
    payloadStream << groupAddress;
    payloadStream << static_cast<quint8>(0); // Note: group has no destination endpoint
    payloadStream << profileId;
    payloadStream << clusterId;
    payloadStream << sourceEndpoint;
    payloadStream << static_cast<quint8>(0x00); // Network and application layer security
    payloadStream << radius;
    payloadStream << static_cast<quint16>(asdu.size());
    for (int i = 0; i < asdu.size(); i++) {
        payloadStream << static_cast<quint8>(asdu.at(i));
    }

    QByteArray message;
    bumpSequenceNumber();
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandSendApsDataRequest);
    stream << static_cast<quint8>(m_sequenceNumber);
    stream << static_cast<quint16>(payload.size());
    for (int i = 0; i < payload.size(); i++) {
        stream << static_cast<quint8>(payload.at(i));
    }

    return createReply(Nxp::CommandSendApsDataRequest, m_sequenceNumber, "Request send ASP data request to group", message, this);
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestEnqueueSendDataShortAddress(quint8 requestId, quint16 shortAddress, quint8 destinationEndpoint, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius)
{
    Q_UNUSED(txOptions)
    Q_ASSERT_X(asdu.length() <= 127, "ASDU", "ASDU package length has to <= 127 bytes");

    QByteArray payload;
    QDataStream payloadStream(&payload, QIODevice::WriteOnly);
    payloadStream.setByteOrder(QDataStream::LittleEndian);
    payloadStream << requestId;
    payloadStream << static_cast<quint8>(Zigbee::DestinationAddressModeShortAddress);
    payloadStream << shortAddress;
    payloadStream << destinationEndpoint;
    payloadStream << profileId;
    payloadStream << clusterId;
    payloadStream << sourceEndpoint;
    payloadStream << static_cast<quint8>(0x00); // Network and application layer security
    payloadStream << radius;
    payloadStream << static_cast<quint16>(asdu.size());
    for (int i = 0; i < asdu.size(); i++) {
        payloadStream << static_cast<quint8>(asdu.at(i));
    }

    QByteArray message;
    bumpSequenceNumber();
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandSendApsDataRequest);
    stream << static_cast<quint8>(m_sequenceNumber);
    stream << static_cast<quint16>(payload.size());
    for (int i = 0; i < payload.size(); i++) {
        stream << static_cast<quint8>(payload.at(i));
    }

    return createReply(Nxp::CommandSendApsDataRequest, m_sequenceNumber, "Request send ASP data request to short address", message, this);
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestEnqueueSendDataIeeeAddress(quint8 requestId, ZigbeeAddress ieeeAddress, quint8 destinationEndpoint, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius)
{
    Q_UNUSED(txOptions)
    Q_ASSERT_X(asdu.length() <= 127, "ASDU", "ASDU package length has to <= 127 bytes");

    QByteArray payload;
    QDataStream payloadStream(&payload, QIODevice::WriteOnly);
    payloadStream.setByteOrder(QDataStream::LittleEndian);
    payloadStream << requestId;
    payloadStream << static_cast<quint8>(Zigbee::DestinationAddressModeIeeeAddress);
    payloadStream << ieeeAddress.toUInt64();
    payloadStream << destinationEndpoint;
    payloadStream << profileId;
    payloadStream << clusterId;
    payloadStream << sourceEndpoint;
    payloadStream << static_cast<quint8>(0x00); // Network and application layer security
    payloadStream << radius;
    payloadStream << static_cast<quint16>(asdu.size());
    for (int i = 0; i < asdu.size(); i++) {
        payloadStream << static_cast<quint8>(asdu.at(i));
    }

    QByteArray message;
    bumpSequenceNumber();
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandSendApsDataRequest);
    stream << static_cast<quint8>(m_sequenceNumber);
    stream << static_cast<quint16>(payload.size());
    for (int i = 0; i < payload.size(); i++) {
        stream << static_cast<quint8>(payload.at(i));
    }

    return createReply(Nxp::CommandSendApsDataRequest, m_sequenceNumber, "Request send ASP data request to IEEE address", message, this);
}

void ZigbeeBridgeControllerNxp::onInterfaceAvailableChanged(bool available)
{
    qCDebug(dcZigbeeController()) << "Interface available changed" << available;
    setAvailable(available);
}

void ZigbeeBridgeControllerNxp::onInterfacePackageReceived(const QByteArray &package)
{
    QDataStream stream(package);
    stream.setByteOrder(QDataStream::LittleEndian);
    quint8 commandInt = 0; quint8 sequenceNumber = 0;
    stream >> commandInt >> sequenceNumber;

    // Note: commands >= 0x7D are notifications
    if (commandInt >= 0x7D) {
        quint16 payloadLength = 0;
        stream >> payloadLength;
        QByteArray payload = package.mid(4, payloadLength);
        if (package.length() != payloadLength + 4) {
            qCWarning(dcZigbeeController()) << "Invalid package length received" << ZigbeeUtils::convertByteArrayToHexString(package) << payloadLength;
            return;
        }

        Nxp::Notification notification = static_cast<Nxp::Notification>(commandInt);
        //qCDebug(dcZigbeeController()) << "Interface notification received" << notification << "SQN:" << sequenceNumber << ZigbeeUtils::convertByteArrayToHexString(payload);
        switch (notification) {
        case Nxp::NotificationDeviceStatusChanged:
            m_controllerState = static_cast<ControllerState>(payload.at(0));
            qCDebug(dcZigbeeController()) << "Controller state changed" << m_controllerState;
            emit controllerStateChanged(m_controllerState);
            break;
        case Nxp::NotificationApsDataConfirm: {
            QDataStream stream(&payload, QIODevice::ReadOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            Zigbee::ApsdeDataConfirm confirm;
            stream >> confirm.requestId >> confirm.destinationAddressMode;
            if (confirm.destinationAddressMode == Zigbee::DestinationAddressModeGroup || confirm.destinationAddressMode == Zigbee::DestinationAddressModeShortAddress)
                stream >> confirm.destinationShortAddress;

            if (confirm.destinationAddressMode == Zigbee::DestinationAddressModeIeeeAddress)
                stream >> confirm.destinationIeeeAddress;

            stream >> confirm.sourceEndpoint >> confirm.destinationEndpoint >> confirm.zigbeeStatusCode;

            qCDebug(dcZigbeeController()) << confirm;
            qCDebug(dcZigbeeAps()) << "APSDE-DATA.confirm" << confirm;

            emit apsDataConfirmReceived(confirm);
            break;
        }
        case Nxp::NotificationApsDataIndication: {
            QDataStream stream(&payload, QIODevice::ReadOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            Zigbee::ApsdeDataIndication indication;
            quint8 status;
            stream >> status;
            stream >> indication.destinationAddressMode;
            Zigbee::DestinationAddressMode destinationAddressMode = static_cast<Zigbee::DestinationAddressMode>(indication.destinationAddressMode);
            if (destinationAddressMode == Zigbee::DestinationAddressModeGroup || destinationAddressMode == Zigbee::DestinationAddressModeShortAddress)
                stream >> indication.destinationShortAddress;

            if (destinationAddressMode == Zigbee::DestinationAddressModeIeeeAddress)
                stream >> indication.destinationIeeeAddress;

            stream >> indication.destinationEndpoint;

            stream >> indication.sourceAddressMode;
            if (indication.sourceAddressMode == Zigbee::DestinationAddressModeGroup || indication.sourceAddressMode == Zigbee::DestinationAddressModeShortAddress)
                stream >> indication.sourceShortAddress;

            if (indication.sourceAddressMode == Zigbee::DestinationAddressModeIeeeAddress)
                stream >> indication.sourceShortAddress;

            stream >> indication.sourceEndpoint;
            stream >> indication.profileId;
            stream >> indication.clusterId;
            quint16 asduLength = 0;
            stream >> asduLength;
            for (int i = 0; i < asduLength; i++) {
                quint8 byte = 0;
                stream >> byte;
                indication.asdu.append(static_cast<char>(byte));
            }
            stream >> indication.lqi;

            // FIXME: security status

            qCDebug(dcZigbeeController()) << indication;
            qCDebug(dcZigbeeAps()) << "APSDE-DATA.indication" << indication;

            emit apsDataIndicationReceived(indication);
            break;
        }
        case Nxp::NotificationDebugMessage: {
            if (payload.isEmpty()) {
                qCWarning(dcZigbeeController()) << "Received empty debug log notification";
                return;
            }
            Nxp::LogLevel logLevel = static_cast<Nxp::LogLevel>(payload.at(0));
            QString debugMessage = QString::fromLocal8Bit(payload.right(payload.length() - 1));
            if (static_cast<quint8>(logLevel) <= static_cast<quint8>(Nxp::LogLevelWarning)) {
                qCWarning(dcZigbeeController()) << "***** Controller DEBUG *****" << logLevel << debugMessage;
            } else {
                qCDebug(dcZigbeeController()) << "***** Controller DEBUG *****" << logLevel << debugMessage;
            }
            break;
        }
        default:
            emit interfaceNotificationReceived(notification, payload);
            break;
        }
    } else {
        quint8 statusInt = 0; quint16 payloadLength = 0;
        stream >> statusInt >> payloadLength;
        if (package.length() != payloadLength + 5) {
            qCWarning(dcZigbeeController()) << "Invalid package length received" << ZigbeeUtils::convertByteArrayToHexString(package) << payloadLength;
            return;
        }
        QByteArray data = package.mid(5, payloadLength);
        Nxp::Command command = static_cast<Nxp::Command>(commandInt);
        Nxp::Status status = static_cast<Nxp::Status>(statusInt);
        qCDebug(dcZigbeeController()) << "Interface response received" << command << "SQN:" << sequenceNumber << status << ZigbeeUtils::convertByteArrayToHexString(data);
        if (m_currentReply->sequenceNumber() == sequenceNumber) {
            if (m_currentReply->command() == command) {
                m_currentReply->m_status = status;
                m_currentReply->m_responseData = data;
            } else {
                qCWarning(dcZigbeeController()) << "Received interface response for a pending sequence number but the command does not match the request." << command << m_currentReply->command();
            }
            m_currentReply->setFinished();
        } else {
            qCWarning(dcZigbeeController()) << "Received a response for a non pending reply. There is no pending reply for command" << command << "SQN:" << sequenceNumber;
        }
    }
}

void ZigbeeBridgeControllerNxp::sendNextRequest()
{
    // Check if there is a reply request to send
    if (m_replyQueue.isEmpty())
        return;

    // Check if there is currently a running reply
    if (m_currentReply)
        return;

    // Send next message
    m_currentReply = m_replyQueue.dequeue();
    qCDebug(dcZigbeeController()) << "Send request" << m_currentReply;
    m_interface->sendPackage(m_currentReply->requestData());
    m_currentReply->m_timer->start();
}

bool ZigbeeBridgeControllerNxp::enable(const QString &serialPort, qint32 baudrate)
{
    return m_interface->enable(serialPort, baudrate);
}

void ZigbeeBridgeControllerNxp::disable()
{
    m_interface->disable();
}
