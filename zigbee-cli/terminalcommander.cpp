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

QStringList TerminalCommander::commands() const
{
    return m_commands;
}

TerminalCommander::TerminalCommander(QObject *parent) :
    QThread(parent)
{
    m_commands.append("start");
    m_commands.append("stop");
    m_commands.append("explode");
}

TerminalCommander::~TerminalCommander()
{
    mutex.lock();
    m_abort = true;
    mutex.unlock();

    wait();
}

void TerminalCommander::printToTerminal(const QString &message)
{
    rl_printf("%s", message.toStdString().data());
}

void TerminalCommander::stopProcess()
{
    mutex.lock();
    m_abort = true;
    mutex.unlock();
}

void TerminalCommander::run()
{
    m_abort = false;
    rl_attempted_completion_function = commandCompletion;

    rl_set_prompt(QString("%1[zigbee]%2 ").arg(terminalColorBoldGray).arg(terminalColorNormal).toStdString().data());
    rl_redisplay();

    rl_bind_key('\t',rl_complete);

    while (true) {
        char *buffer = readline(QString("%1[zigbee]%2 ").arg(terminalColorBoldGray).arg(terminalColorNormal).toStdString().data());
        rl_bind_key('\t',rl_complete);
        if (buffer) {
            if (!QString(buffer).isEmpty()) {
                QStringList tokens = QString(buffer).split(" ");
                emit commandReceived(tokens);
                add_history(buffer);
                free(buffer);
            }
        }
    }

    if (m_abort)
        return;

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

    while (list_index < TerminalCommander::instance()->commands().count() && (name = TerminalCommander::instance()->commands().at(list_index).toStdString().data())) {
        list_index++;
        if (strncmp (name, text, len) == 0) return strdup (name);
    }

    // If no names matched, then return NULL.
    return ((char *) NULL);
}
