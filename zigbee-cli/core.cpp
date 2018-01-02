#include "core.h"
#include "loggingcategory.h"
#include "QCoreApplication"

Core::Core(const QString &serialPort, const int &channel, QObject *parent) :
    QObject(parent),
    m_serialPort(serialPort)
{
    m_manager = new ZigbeeNetworkManager(channel, m_serialPort, this);

    // Set commands
    TerminalCommand runCommand("run", "Run the zigbee controller in a normal non interactive mode.");
    QStringList optionalParams;
    optionalParams.append("router");
    optionalParams.append("coordinator");
    runCommand.setOptionalParameters(optionalParams);

    m_commands.append(runCommand);
    m_commands.append(TerminalCommand("start", "Start the zigbee network"));
    m_commands.append(TerminalCommand("reset", "Reset the zigbee controller"));
    m_commands.append(TerminalCommand("scan", "Start scanning for zigbee networks"));
    m_commands.append(TerminalCommand("version", "Print the version of the zigbee controll bridge firmware"));
    m_commands.append(TerminalCommand("list-nodes", "List all nodes and information of the current network"));
    m_commands.append(TerminalCommand("permit-join", "Permit nodes to join the network"));
    m_commands.append(TerminalCommand("touch-link", "Initiate touch link pairing"));
    m_commands.append(TerminalCommand("reset-touchlink", "Touch link factory reset"));
    m_commands.append(TerminalCommand("whitelist", "Enable the white list joining"));

    TerminalCommander::instance()->setCommands(m_commands);
    TerminalCommander::instance()->start();

    connect(TerminalCommander::instance(), &TerminalCommander::commandReceived, this, &Core::onCommandReceived);
    connect(TerminalCommander::instance(), &TerminalCommander::finished, QCoreApplication::instance(), &QCoreApplication::quit);
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

    if (command.command() == "run") {

        ZigbeeNetworkManager::NodeType nodeType = ZigbeeNetworkManager::NodeTypeCoordinator;
        if (tokens.count() > 1) {
            if (!command.optionalParameters().contains(tokens.at(1))) {
                qCWarning(dcZigbee()) << "Unknown paramter" << tokens.at(1);
                return;
            }

            if (tokens.at(1) == "router") {
                nodeType = ZigbeeNetworkManager::NodeTypeRouter;
            }
        }

        m_manager->erasePersistentData();
        m_manager->getVersion();
        m_manager->setExtendedPanId(m_manager->extendedPanId());
        m_manager->setChannelMask(0);
        m_manager->setDeviceType(nodeType);

    } else if (command.command() == "start") {
        m_manager->startNetwork();
    } else if (command.command() == "version") {
        m_manager->getVersion();
    } else if (command.command() == "scan") {
        m_manager->startScan();
    } else if (command.command() == "reset") {
        m_manager->resetController();
    } else if (command.command() == "permit-join") {
        m_manager->permitJoining();
    } else if (command.command() == "touch-link") {
        m_manager->initiateTouchLink();
    } else if (command.command() == "touch-link-reset") {
        m_manager->touchLinkFactoryReset();
    } else if (command.command() == "enable-whitelist") {
        m_manager->enableWhitelist();
    } else if (command.command() == "list-nodes") {
        qCDebug(dcZigbee()) << "--> Host:" << m_manager;

        foreach (ZigbeeNode *node, m_manager->nodeList()) {
            qCDebug(dcZigbee()) << "-->" << node;
        }
    }

}
