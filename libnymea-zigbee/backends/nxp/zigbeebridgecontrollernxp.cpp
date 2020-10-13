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
        QByteArray data = package.mid(4, payloadLength);
        if (package.length() != payloadLength + 4) {
            qCWarning(dcZigbeeController()) << "Invalid package length received" << ZigbeeUtils::convertByteArrayToHexString(package) << payloadLength;
            return;
        }

        Nxp::Notification notification = static_cast<Nxp::Notification>(commandInt);
        qCDebug(dcZigbeeController()) << "Interface notification received" << notification << "SQN:" << sequenceNumber << ZigbeeUtils::convertByteArrayToHexString(data);
        switch (notification) {
        case Nxp::NotificationDebugMessage:
            if (data.isEmpty()) {
                qCWarning(dcZigbeeController()) << "Received empty debug log notification";
                return;
            }
            qCDebug(dcZigbeeController()) << "*****DEBUG*****" << static_cast<Nxp::LogLevel>(data.at(0)) << Qt::endl << qUtf8Printable(data.right(data.length() - 1));
            break;
        case Nxp::NotificationDeviceStatusChanged:
            m_controllerState = static_cast<ControllerState>(data.at(0));
            qCDebug(dcZigbeeController()) << "Controller state changed" << m_controllerState;
            emit controllerStateChanged(m_controllerState);
            break;
        default:
            emit interfaceNotificationReceived(notification, data);
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
