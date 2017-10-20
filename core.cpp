#include "core.h"
#include <QDebug>

Core::Core(QObject *parent) :
    QObject(parent)
{
    m_manager = new ZigbeeNetworkManager("/dev/ttyS0", this);
}
