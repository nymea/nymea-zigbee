#ifndef TERMINALCOMMANDER_H
#define TERMINALCOMMANDER_H

#include <QObject>
#include <QThread>
#include <QMutex>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "terminalcommand.h"

static const char *const terminalColorNormal = "\033[0m";
static const char *const terminalColorYellow = "\x1B[0;93m";
static const char *const terminalColorRed = "\x1B[0;91m";
static const char *const terminalColorBlue = "\x1B[0;94m";
static const char *const terminalColorGreen = "\e[31m";
static const char *const terminalColorBoldGray = "\x1B[1;30m";
static const char *const terminalColorBoldNormal = "\x1B[1;37m";

class TerminalCommander : public QThread
{
    Q_OBJECT
public:
    static TerminalCommander* instance();
    void destroy();

    QList<TerminalCommand> commands() const;
    void setCommands(QList<TerminalCommand> commands);

    void printToTerminal(const QString &message);

private:
    explicit TerminalCommander(QObject *parent = nullptr);
    ~TerminalCommander();

    QList<TerminalCommand> m_commands;

    QMutex m_mutex;
    bool m_abort = false;

    static void rl_printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
    static void signalHandler(int status);

    void printHelpMessage();

signals:
    void commandReceived(const QStringList &tokens);

protected:
    void run() override;

public slots:
    void stopProcess();

};

#endif // TERMINALCOMMANDER_H
