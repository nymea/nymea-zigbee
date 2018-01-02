#include "terminalcommand.h"

TerminalCommand::TerminalCommand()
{

}

TerminalCommand::TerminalCommand(const QString &command, const QString &description) :
    m_command(command),
    m_description(description)
{

}

QString TerminalCommand::command() const
{
    return m_command;
}

void TerminalCommand::setCommand(const QString &command)
{
    m_command = command;
}

QString TerminalCommand::description() const
{
    return m_description;
}

void TerminalCommand::setDescription(const QString &description)
{
    m_description = description;
}

QStringList TerminalCommand::mandatoryParameters() const
{
    return m_mandatorysParameters;
}

void TerminalCommand::setMandatoryParameters(const QStringList &parameters)
{
    m_mandatorysParameters = parameters;
}

QStringList TerminalCommand::optionalParameters() const
{
    return m_optionalParameters;
}

void TerminalCommand::setOptionalParameters(const QStringList &parameters)
{
    m_optionalParameters = parameters;
}

QString TerminalCommand::defaultParameter() const
{
    return m_defaultParamter;
}

void TerminalCommand::setDefaultParameters(const QString &parameter)
{
    m_defaultParamter = parameter;
}

bool TerminalCommand::isValid()
{
    return !m_command.isEmpty() && !m_description.isEmpty();
}

QDebug operator<<(QDebug debug, const TerminalCommand &command)
{
    debug << command.command() << command.description();
    return debug;
}
