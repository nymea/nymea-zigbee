#include "terminalcommander.h"
#include "loggingcategory.h"

#include <QStringList>

static char **commandCompletion(const char* text, int start, int end);
static char *commandCompletionGenerator(const char *text, int state);

static TerminalCommander *s_instance = nullptr;

TerminalCommander *TerminalCommander::instance()
{
    if (!s_instance)
        s_instance = new TerminalCommander();

    return s_instance;
}

void TerminalCommander::destroy()
{
    qCDebug(dcZigbee()) << "Shut down terminal commander. Have a nice day.b";
    stopProcess();
}

QList<TerminalCommand> TerminalCommander::commands() const
{
    return m_commands;
}

void TerminalCommander::setCommands(QList<TerminalCommand> commands)
{
    m_commands.append(commands);
}

TerminalCommander::TerminalCommander(QObject *parent) :
    QThread(parent)
{
    // Native terminal commands
    m_commands.append(TerminalCommand("exit", "Exit this application"));
    m_commands.append(TerminalCommand("quit", "Quit this application"));
    m_commands.append(TerminalCommand("help", "Print this help message"));
}

TerminalCommander::~TerminalCommander()
{
    destroy();
    wait();
}

void TerminalCommander::printToTerminal(const QString &message)
{
    rl_printf("%s", message.toStdString().data());
}

void TerminalCommander::stopProcess()
{
    QMutexLocker locker(&m_mutex);
    m_abort = true;
}

void TerminalCommander::run()
{
    m_abort = false;
    rl_attempted_completion_function = commandCompletion;

    rl_set_prompt(QString("%1[zigbee]%2 ").arg(terminalColorBoldGray).arg(terminalColorNormal).toStdString().data());
    rl_redisplay();
    rl_bind_key('\t',rl_complete);

    rl_clear_signals();
    rl_catch_signals = 1;

    while (true) {
        char *buffer = readline(QString("%1[zigbee]%2 ").arg(terminalColorBoldGray).arg(terminalColorNormal).toStdString().data());
        rl_bind_key('\t',rl_complete);
        if (buffer) {
            if (!QString(buffer).isEmpty()) {
                QStringList tokens = QString(buffer).split(" ");

                // Check quit
                if (tokens.count() == 1 && (tokens.first() == "quit" || tokens.first() == "exit")) {
                    qDebug() << "";
                    rl_on_new_line();
                    rl_replace_line("", 0);
                    rl_redisplay();
                    qCDebug(dcZigbee()) << "Terminal thread stopped";
                    free(buffer);
                    stopProcess();
                    return;

                } else if (tokens.count() == 1 && (tokens.first() == "?" || tokens.first() == "help")) {
                    printHelpMessage();
                    add_history(buffer);
                    free(buffer);
                } else {
                    emit commandReceived(tokens);
                    add_history(buffer);
                    free(buffer);
                }
            }
        }

        QMutexLocker locker(&m_mutex);
        if (m_abort) {
            qDebug() << "";
            rl_on_new_line();
            rl_replace_line("", 0);
            rl_redisplay();
            qCDebug(dcZigbee()) << "Terminal thread stopped";
            return;
        }
    }

    msleep(10);
}

void TerminalCommander::rl_printf(const char *fmt, ...)
{
    va_list args;
    bool save_input;
    char *saved_line;
    int saved_point;

    save_input = !RL_ISSTATE(RL_STATE_DONE);

    if (save_input) {
        saved_point = rl_point;
        saved_line = rl_copy_text(0, rl_end);
        rl_save_prompt();
        rl_replace_line("", 0);
        rl_redisplay();
    }

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    if (save_input) {
        rl_restore_prompt();
        rl_replace_line(saved_line, 0);
        rl_point = saved_point;
        rl_redisplay();
        free(saved_line);
    }
}

void TerminalCommander::signalHandler(int status)
{
    Q_UNUSED(status)
    qCDebug(dcZigbee()) << "Terminal thread stopped";
    TerminalCommander::instance()->stopProcess();
}

void TerminalCommander::printHelpMessage()
{
    for (int i = 0; i < m_commands.count(); i++) {
        QString command = QString("\t%1 %2").arg(m_commands.at(i).command(), -20).arg(m_commands.at(i).description());
        rl_printf("%s\n",command.toStdString().data());
    }
}

char **commandCompletion(const char *text, int start, int end)
{
    Q_UNUSED(end)
    rl_completion_append_character = '\0';

    char **matches = (char **) NULL;
    if (start == 0) {
        matches = rl_completion_matches ((char *) text, &commandCompletionGenerator);
    }

    return matches;
}

char *commandCompletionGenerator(const char *text, int state)
{
    //qCDebug(dcZigbee) << text << state << TerminalCommander::instance()->commands();

    static int list_index, len;
    const char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while (list_index < TerminalCommander::instance()->commands().count() && (name = TerminalCommander::instance()->commands().at(list_index).command().toStdString().data())) {
        list_index++;
        if (strncmp (name, text, len) == 0) return strdup (name);
    }

    // If no names matched, then return NULL.
    return ((char *) NULL);
}
