#include "core.h"
#include <QDebug>

Core::Core(QObject *parent) :
    QObject(parent)
{
    m_manager = new ZigbeeManager("/dev/ttyS0", this);
}
