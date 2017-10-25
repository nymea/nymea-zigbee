#include "core.h"
#include <QDebug>

Core::Core(const int &channel, QObject *parent) :
    QObject(parent)
{
    m_manager = new ZigbeeNetworkManager(channel, "/dev/ttyS0", this);
}
