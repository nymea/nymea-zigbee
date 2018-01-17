#include "core.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QCoreApplication>

Core::Core(const QString &serialPort, const int &channel, QObject *parent) :
    QObject(parent),
    m_serialPort(serialPort)
{
    m_channelMask = 0;
    m_channelMask |= 1 << (channel);

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
    m_commands.append(TerminalCommand("network-info", "Print all information of the zigbee network."));
    m_commands.append(TerminalCommand("list-nodes", "List all nodes and information of the current network"));
    m_commands.append(TerminalCommand("permit-join", "Permit nodes to join the network"));
    m_commands.append(TerminalCommand("touch-link", "Initiate touch link pairing"));
    m_commands.append(TerminalCommand("reset-touchlink", "Touch link factory reset"));
    m_commands.append(TerminalCommand("whitelist", "Enable the white list joining"));
    m_commands.append(TerminalCommand("address-request", "Request network address on host node"));
    m_commands.append(TerminalCommand("matchdescriptor", "Request match descriptors"));
    m_commands.append(TerminalCommand("init-node", "Request simple descriptors"));
    m_commands.append(TerminalCommand("lqi", "Request link quality"));
    m_commands.append(TerminalCommand("toggle", "Request to toggle"));
    m_commands.append(TerminalCommand("authenticate", "Authenticate device with given IEEE address"));

    TerminalCommander::instance()->setCommands(m_commands);
    TerminalCommander::instance()->start();

    connect(TerminalCommander::instance(), &TerminalCommander::commandReceived, this, &Core::onCommandReceived);
    connect(TerminalCommander::instance(), &TerminalCommander::finished, QCoreApplication::instance(), &QCoreApplication::quit);
}

ZigbeeNode *Core::findNode(const QString &shortAddressString)
{
    foreach (ZigbeeNode *node, m_manager->nodeList()) {
        if (ZigbeeUtils::convertUint16ToHexString(node->shortAddress()) == shortAddressString) {
            return node;
        }
    }

    return nullptr;
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
        m_manager->setChannelMask(0x2108800);
        m_manager->setDeviceType(nodeType);
        // Note: this is the leaked philips ZLL master key
        m_manager->setInitialSecurity(3, 0, 1, "9F5595F10257C8A469CBF42BC93FEE31");
        m_manager->setInitialSecurity(4, 0, 1, "5A6967426565416C6C69616E63653039");

    } else if (command.command() == "start") {
        m_manager->startNetwork();
    } else if (command.command() == "version") {
        m_manager->getVersion();
    } else if (command.command() == "network-info") {
        qCDebug(dcZigbee()).nospace().noquote() << "Network controller: " << m_manager->serialPort() << ", Bridge version: " << m_manager->controllerVersion();
    } else if (command.command() == "scan") {
        m_manager->startScan();
    } else if (command.command() == "reset") {
        m_manager->resetController();
    } else if (command.command() == "permit-join") {
        m_manager->permitJoining(0xfffc, 180, true);
    } else if (command.command() == "touch-link") {
        m_manager->initiateTouchLink();
    } else if (command.command() == "reset-touchlink") {
        m_manager->touchLinkFactoryReset();
    } else if (command.command() == "whitelist") {
        m_manager->enableWhitelist();
    } else if (command.command() == "address-request") {
        if (tokens.count() == 1) {
            m_manager->networkAddressRequest(0, m_manager->extendedAddress().toUInt64());
            return;
        }
        ZigbeeNode *node = findNode(tokens.at(1));
        if (!node) {
            qCWarning(dcZigbee()) << "Could not find node with short address tokens.at(1)";
            return;
        }

        m_manager->networkAddressRequest(node->shortAddress(), node->extendedAddress().toUInt64());

    } else if (command.command() == "toggle") {
        if (tokens.count() < 3) {
            qCWarning(dcZigbee()) << "Please specify also the node short address and adressMode";
            return;
        }


        ZigbeeNode *node = findNode(tokens.at(1));
        if (!node) {
            qCWarning(dcZigbee()) << "Could not find node with short address tokens.at(1)";
            return;
        }

        int addressMode = QString(tokens.at(2)).toInt();

        node->toggle(addressMode);

    } else if (command.command() == "lqi") {
        m_manager->requestLinkQuality();
    } else if (command.command() == "init-node") {
        if (tokens.count() == 1) {
            qCWarning(dcZigbee()) << "Please specify also the node short address";
            return;
        }

        ZigbeeNode *node = findNode(tokens.at(1));
        if (!node) {
            qCWarning(dcZigbee()) << "Could not find node with short address tokens.at(1)";
            return;
        }

        node->init();

    } else if (command.command() == "matchdescriptor") {
        m_manager->requestMatchDescriptor(0x9004, Zigbee::ZigbeeProfileLightLink);
        //m_manager->requestMatchDescriptor(0xFFFD, Zigbee::ZigbeeProfileLightLink);

    } else if (command.command() == "authenticate") {
        if (tokens.count() == 1) {
            qCWarning(dcZigbee()) << "Please specify also the node short address";
            return;
        }

        m_manager->authenticateDevice(ZigbeeAddress(tokens.at(1)));

    } else if (command.command() == "list-nodes") {
        qCDebug(dcZigbee()) << "--> Host:" << m_manager;

        foreach (ZigbeeNode *node, m_manager->nodeList()) {
            qCDebug(dcZigbee()) << "    -->" << node;
        }
    }

}
