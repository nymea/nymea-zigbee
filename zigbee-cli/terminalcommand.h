#ifndef TERMINALCOMMAND_H
#define TERMINALCOMMAND_H

#include <QString>
#include <QStringList>

class TerminalCommand
{
public:
    explicit TerminalCommand();
    explicit TerminalCommand(const QString &command, const QString &description);

    QString command() const;
    void setCommand(const QString &command);

    QString description() const;
    void setDescription(const QString &description);

    QStringList mandatoryParameters() const;
    void setMandatoryParameters(const QStringList &parameters);

    QStringList optionalParameters() const;
    void setOptionalParameters(const QStringList &parameters);

    bool isValid();

private:
    QString m_command;
    QString m_description;
    QStringList m_mandatorysParameters;
    QStringList m_optionalParameters;

};

#endif // TERMINALCOMMAND_H
