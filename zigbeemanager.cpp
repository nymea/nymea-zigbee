#include "zigbeemanager.h"
#include "loggingcategory.h"

ZigbeeManager::ZigbeeManager(const QString &serialPort, QObject *parent) :
    QObject(parent),
    m_serialPort(serialPort)
{
    m_interface = new ZigbeeInterface(this);

    connect(m_interface, &ZigbeeInterface::messageReceived, this, &ZigbeeManager::onMessageReceived);


    if (!m_interface->enable(m_serialPort)) {
        qCWarning(dcZigbee()) << "Could not enable ZigbeeInterface on" << m_serialPort;
        return;
    }

    initController();

}

QString ZigbeeManager::serialPort() const
{
    return m_serialPort;
}

void ZigbeeManager::setSerialPort(const QString &serialPort)
{
    if (m_serialPort == serialPort)
        return;

    m_serialPort = serialPort;
    m_interface->disable();
    m_interface->enable(m_serialPort);
}

void ZigbeeManager::initController()
{
    ZigbeeInterfaceMessage message;
    message.setMessageType(Zigbee::DataManagerAvailableResponse);
    message.setData(QByteArray::fromRawData("\x00\x00", 2));

    m_interface->sendMessage(message);
}

void ZigbeeManager::onMessageReceived(const ZigbeeInterfaceMessage &message)
{
    qCDebug(dcZigbee()) << message;
}
