#include "zigbeedeviceobject.h"
#include "zigbeenetwork.h"

ZigbeeDeviceObject::ZigbeeDeviceObject(ZigbeeNetwork *network, QObject *parent) :
    QObject(parent),
    m_network(network)
{

}
