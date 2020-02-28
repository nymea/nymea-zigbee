#ifndef ZIGBEENODENXP_H
#define ZIGBEENODENXP_H

#include <QObject>
#include "../zigbeenode.h"

#include "zigbeebridgecontrollernxp.h"

class ZigbeeNodeNxp : public ZigbeeNode
{
    Q_OBJECT

    friend class ZigbeeNetworkNxp;

public:
    enum InitState {
        InitStateNone,
        InitStateNodeDescriptor,
        InitStatePowerDescriptor,
        InitStateActiveEndpoints,
        InitStateSimpleDescriptors
    };
    Q_ENUM(InitState)

    explicit ZigbeeNodeNxp(ZigbeeBridgeControllerNxp *controller, QObject *parent = nullptr);

private:
    ZigbeeBridgeControllerNxp *m_controller = nullptr;
    InitState m_initState = InitStateNone;

    void setInitState(InitState initState);

protected:
    void startInitialization() override;

signals:

};

#endif // ZIGBEENODENXP_H
