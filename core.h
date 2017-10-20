#ifndef CORE_H
#define CORE_H

#include <QObject>

#include "zigbeenetworkmanager.h"

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(QObject *parent = nullptr);

private:
    ZigbeeNetworkManager *m_manager;

signals:

public slots:

};

#endif // CORE_H
