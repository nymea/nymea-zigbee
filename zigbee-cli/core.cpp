#include "core.h"
#include "terminalcommander.h"
#include "loggingcategory.h"

Core::Core(const int &channel, QObject *parent) :
    QObject(parent)
{
    m_manager = new ZigbeeNetworkManager(channel, "/dev/ttyS0", this);

    TerminalCommander::instance();

    // Set commands

    TerminalCommander::instance()->start();

    m_testTimer = new QTimer(this);
    m_testTimer->setInterval(5000);
    m_testTimer->setSingleShot(false);

    connect(m_testTimer, &QTimer::timeout, this, &Core::onTimeout);
    m_testTimer->start();

    connect(TerminalCommander::instance(), &TerminalCommander::commandReceived, this, &Core::onCommandReceived);
}

void Core::onTimeout()
{
    qCDebug(dcZigbee()) << "[+] This is a test message from the zigbee thread";
}

void Core::onCommandReceived(const QStringList &tokens)
{
    qCDebug(dcZigbee()) << "Command received:" << tokens;

}
