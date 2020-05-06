#include "zigbeeclusterbasic.h"

ZigbeeClusterBasic::ZigbeeClusterBasic(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, Zigbee::ClusterIdBasic, direction, parent)
{

}
