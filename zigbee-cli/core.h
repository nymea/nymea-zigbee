#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <readline/readline.h>

#include "terminalcommander.h"
#include "zigbeenetworkmanager.h"

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(const QString &serialPort, const int &channel, QObject *parent = nullptr);

private:
    ZigbeeNetworkManager *m_manager = nullptr;
    QList<TerminalCommand> m_commands;
    QString m_serialPort;

signals:

private slots:
    void onCommandReceived(const QStringList &tokens);

};

#endif // CORE_H
