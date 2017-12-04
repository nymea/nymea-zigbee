#ifndef TERMINALCOMMAND_H
#define TERMINALCOMMAND_H

#include <QObject>

class TerminalCommand : public QObject
{
    Q_OBJECT
public:
    explicit TerminalCommand(QObject *parent = nullptr);

signals:

public slots:
};

#endif // TERMINALCOMMAND_H