#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <readline/readline.h>

#include "zigbeenetworkmanager.h"

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(const int &channel, QObject *parent = nullptr);

private:
    ZigbeeNetworkManager *m_manager;
    QTimer *m_testTimer;

signals:

private slots:
    void onTimeout();
    void onCommandReceived(const QStringList &tokens);
};

#endif // CORE_H
