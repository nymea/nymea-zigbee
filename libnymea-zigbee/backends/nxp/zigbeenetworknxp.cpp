#include "zigbeenetworknxp.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

ZigbeeNetworkNxp::ZigbeeNetworkNxp(QObject *parent) :
    ZigbeeNetwork(parent)
{
    m_controller = new ZigbeeBridgeControllerNxp(this);
    connect(m_controller, &ZigbeeBridgeControllerNxp::availableChanged, this, &ZigbeeNetworkNxp::onControllerAvailableChanged);
    //connect(m_controller, &ZigbeeBridgeControllerNxp::apsDataConfirmReceived, this, &ZigbeeNetworkNxp::onApsDataConfirmReceived);
    //connect(m_controller, &ZigbeeBridgeControllerNxp::apsDataIndicationReceived, this, &ZigbeeNetworkNxp::onApsDataIndicationReceived);

}

ZigbeeBridgeController *ZigbeeNetworkNxp::bridgeController() const
{
    if (!m_controller)
        return nullptr;

    return qobject_cast<ZigbeeBridgeController *>(m_controller);
}

ZigbeeNetworkReply *ZigbeeNetworkNxp::sendRequest(const ZigbeeNetworkRequest &request)
{
    Q_UNUSED(request)
    return nullptr;
}

ZigbeeNetworkReply *ZigbeeNetworkNxp::setPermitJoin(quint16 shortAddress, quint8 duration)
{
    Q_UNUSED(shortAddress)
    Q_UNUSED(duration)
    return nullptr;
}

void ZigbeeNetworkNxp::onControllerAvailableChanged(bool available)
{
    qCDebug(dcZigbeeNetwork()) << "Controller is" << (available ? "now available" : "not available any more");

    if (available) {
        reset();

//        ZigbeeInterfaceNxpReply *reply = m_controller->requestVersion();
//        connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [](){
//            qCDebug(dcZigbeeNetwork()) << "Version reply finished";
//        });
    }
}

void ZigbeeNetworkNxp::setPermitJoiningInternal(bool permitJoining)
{
    qCDebug(dcZigbeeNetwork()) << "Set permit join internal" << permitJoining;
}

void ZigbeeNetworkNxp::startNetwork()
{
    loadNetwork();

    if (!m_controller->enable(serialPortName(), serialBaudrate())) {
        m_permitJoining = false;
        emit permitJoiningChanged(m_permitJoining);
        setState(StateOffline);
        setError(ErrorHardwareUnavailable);
        return;
    }

    m_permitJoining = false;
    emit permitJoiningChanged(m_permitJoining);

    // Get current state and load information
}

void ZigbeeNetworkNxp::stopNetwork()
{

}

void ZigbeeNetworkNxp::reset()
{
    ZigbeeInterfaceNxpReply *reply = m_controller->requestSoftResetController();
    connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [](){
        qCDebug(dcZigbeeNetwork()) << "Soft reset reply finished";
    });
}

void ZigbeeNetworkNxp::factoryResetNetwork()
{

}
