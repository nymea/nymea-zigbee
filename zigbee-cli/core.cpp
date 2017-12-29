#include "core.h"
#include "loggingcategory.h"
#include "QCoreApplication"

Core::Core(const int &channel, QObject *parent) :
    QObject(parent)
{
    m_manager = new ZigbeeNetworkManager(channel, "/dev/ttyS0", this);

    // Set commands
    m_commands.append(TerminalCommand("run", "Run the zigbee controller in a normal non interactive mode."));
    m_commands.append(TerminalCommand("start", "Start the zigbee network"));
    m_commands.append(TerminalCommand("stop", "Stop the zigbee network"));
    m_commands.append(TerminalCommand("reset", "Reset the zigbee controller"));
    m_commands.append(TerminalCommand("scan", "Start scanning for zigbee networks"));
    m_commands.append(TerminalCommand("version", "Print the version of the zigbee controll bridge firmware"));
    m_commands.append(TerminalCommand("permit-join", "Permit nodes to join the network"));

    TerminalCommander::instance()->setCommands(m_commands);

    TerminalCommander::instance()->start();

    m_testTimer = new QTimer(this);
    m_testTimer->setInterval(5000);
    m_testTimer->setSingleShot(false);

    connect(m_testTimer, &QTimer::timeout, this, &Core::onTimeout);
    //m_testTimer->start();

    connect(TerminalCommander::instance(), &TerminalCommander::commandReceived, this, &Core::onCommandReceived);
    connect(TerminalCommander::instance(), &TerminalCommander::finished, QCoreApplication::instance(), &QCoreApplication::quit);
}

void Core::onTimeout()
{
    qCDebug(dcZigbee()) << "[+] This is a test message from the zigbee thread";
}

void Core::onCommandReceived(const QStringList &tokens)
{
    TerminalCommand command;
    foreach (const TerminalCommand &terminalCommand, m_commands) {
        if (tokens.first() == terminalCommand.command()) {
            command = terminalCommand;
            break;
        }
    }

    if (!command.isValid()) {
        qCWarning(dcZigbee()) << "Unknown command" << tokens;
        return;
    }

    // TODO: Process command
    qCDebug(dcZigbee()) << "Executing" << tokens.join(" ");

    if (command.command() == "start") {
        //m_manager->startNetwork();
    } else if (command.command() == "version") {
        //m_manager->getVersion();
    }

}
