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

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestVersion()
{
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandGetVersion);
    stream << static_cast<quint8>(m_sequenceNumber++);
    stream << static_cast<quint16>(0); // Frame length

    return createReply(Nxp::CommandGetVersion, m_sequenceNumber, "Request controller version", message, this);
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::requestSoftResetController()
{
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(Nxp::CommandSoftReset);
    stream << static_cast<quint8>(m_sequenceNumber++);
    stream << static_cast<quint16>(0); // Frame length

    return createReply(Nxp::CommandSoftReset, m_sequenceNumber, "Request soft reset controller", message, this);
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
    connect(reply, &ZigbeeInterfaceNxpReply::finished, reply, [reply](){
        reply->deleteLater();
    });

    m_pendingReplies.insert(sequenceNumber, reply);
    m_interface->sendPackage(requestData);
    return reply;
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
        if (package.length() < payloadLength + 4) {
            qCWarning(dcZigbeeController()) << "Invalid package length received" << ZigbeeUtils::convertByteArrayToHexString(package) << payloadLength;
            return;
        }
        Nxp::Notification notification = static_cast<Nxp::Notification>(commandInt);
        //qCDebug(dcZigbeeController()) << "Interface notification received" << notification << "SQN:" << sequenceNumber << ZigbeeUtils::convertByteArrayToHexString(data);
        if (notification == Nxp::NotificationDebugMessage) {
            if (data.isEmpty()) {
                qCWarning(dcZigbeeController()) << "Received empty debug log notification";
                return;
            }
            Nxp::LogLevel logLevel = static_cast<Nxp::LogLevel>(data.at(0));
            qCDebug(dcZigbeeController()) << "DEBUG" << logLevel << qUtf8Printable(data.right(data.length() - 1));
        }

        emit interfaceNotificationReceived(notification, data);
    } else {
        quint8 statusInt = 0; quint16 payloadLength = 0;
        stream >> statusInt >> payloadLength;
        if (package.length() < payloadLength + 5) {
            qCWarning(dcZigbeeController()) << "Invalid package length received" << ZigbeeUtils::convertByteArrayToHexString(package) << payloadLength;
            return;
        }
        QByteArray data = package.mid(5, payloadLength);
        Nxp::Command command = static_cast<Nxp::Command>(commandInt);
        Nxp::Status status = static_cast<Nxp::Status>(statusInt);
        qCDebug(dcZigbeeController()) << "Interface response received" << command << "SQN:" << sequenceNumber << status << ZigbeeUtils::convertByteArrayToHexString(data);
        if (m_pendingReplies.keys().contains(sequenceNumber)) {
            ZigbeeInterfaceNxpReply * reply = m_pendingReplies.take(sequenceNumber);
            if (reply->command() == command) {
                reply->m_status = status;
                reply->m_responseData = data;
            } else {
                qCWarning(dcZigbeeController()) << "Received interface response for a pending sequence number but the command does not match the request." << command << reply->command();
            }
            reply->finished();
        }
    }
}

bool ZigbeeBridgeControllerNxp::enable(const QString &serialPort, qint32 baudrate)
{
    return m_interface->enable(serialPort, baudrate);
}

void ZigbeeBridgeControllerNxp::disable()
{
    m_interface->disable();
}
