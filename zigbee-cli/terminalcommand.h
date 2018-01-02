#ifndef TERMINALCOMMAND_H
#define TERMINALCOMMAND_H

#include <QDebug>
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

    QString defaultParameter() const;
    void setDefaultParameters(const QString &parameter);

    bool isValid();

private:
    QString m_command;
    QString m_description;
    QString m_defaultParamter;
    QStringList m_mandatorysParameters;
    QStringList m_optionalParameters;

};

QDebug operator<<(QDebug debug, const TerminalCommand &command);


#endif // TERMINALCOMMAND_H
