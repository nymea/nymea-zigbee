#include "zigbeebridgecontrollernxp.h"
#include "loggingcategory.h"

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
    stream << static_cast<quint8>(m_sequenceNumber);
    stream << static_cast<quint16>(0); // Frame length

    m_sequenceNumber++;
    return createReply(Nxp::CommandGetVersion, "Request controller version", message, this);
}

ZigbeeInterfaceNxpReply *ZigbeeBridgeControllerNxp::createReply(Nxp::Command command, const QString &requestName, const QByteArray &requestData, QObject *parent)
{
    // Create the reply
    ZigbeeInterfaceNxpReply *reply = new ZigbeeInterfaceNxpReply(command, parent);
    reply->m_requestName = requestName;
    reply->m_requestData = requestData;

    // Make sure we clean up on timeout
    connect(reply, &ZigbeeInterfaceNxpReply::timeout, this, [reply](){
        qCWarning(dcZigbeeController()) << "Reply timeout" << reply;
    });

    // Auto delete the object on finished
    connect(reply, &ZigbeeInterfaceNxpReply::finished, reply, [reply](){
        reply->deleteLater();
    });

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
    qCDebug(dcZigbeeController()) << "Interface package received" << package;
}

bool ZigbeeBridgeControllerNxp::enable(const QString &serialPort, qint32 baudrate)
{
    return m_interface->enable(serialPort, baudrate);
}

void ZigbeeBridgeControllerNxp::disable()
{
    m_interface->disable();
}
