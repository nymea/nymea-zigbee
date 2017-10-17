#include "zigbeemanager.h"

ZigbeeManager::ZigbeeManager(const QString &serialPort, QObject *parent) :
    QObject(parent),
    m_serialPort(serialPort)
{
    m_interface = new ZigbeeInterface(this);


    m_interface->enable(m_serialPort);
    m_interface->sendCommand(ZigbeeInterface::DataManagerAvailableResponse, QByteArray::fromRawData("\x00\x00", 2));
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
