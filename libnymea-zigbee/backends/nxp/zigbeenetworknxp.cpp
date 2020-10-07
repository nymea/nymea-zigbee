#include "zigbeenetworknxp.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeNetworkNxp::ZigbeeNetworkNxp(QObject *parent) :
    ZigbeeNetwork(parent)
{
    m_controller = new ZigbeeBridgeControllerNxp(this);
    connect(m_controller, &ZigbeeBridgeControllerNxp::availableChanged, this, &ZigbeeNetworkNxp::onControllerAvailableChanged);
    connect(m_controller, &ZigbeeBridgeControllerNxp::controllerStateChanged, this, &ZigbeeNetworkNxp::onControllerStateChanged);
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
    }
}

void ZigbeeNetworkNxp::onControllerStateChanged(ZigbeeBridgeControllerNxp::ControllerState controllerState)
{
    switch (controllerState) {
    case ZigbeeBridgeControllerNxp::ControllerStateRunning: {
        qCDebug(dcZigbeeNetwork()) << "Request controller version";
        ZigbeeInterfaceNxpReply *reply = m_controller->requestVersion();
        connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
            qCDebug(dcZigbeeNetwork()) << "Version reply finished" << reply->status();
            QByteArray payload = reply->responseData();
            QDataStream stream(&payload, QIODevice::ReadOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            quint8 major = 0; quint8 minor = 0; quint8 patch = 0; quint16 sdkVersion = 0;
            stream >> major >> minor >> patch >> sdkVersion;

            QString versionString = QString ("%1.%2.%3 - %4").arg(major).arg(minor).arg(patch).arg(sdkVersion);
            qCDebug(dcZigbeeNetwork()) << "Controller version" << versionString;
            m_controller->setFirmwareVersion(versionString);

            // We are done here...

        });
        break;
    }
    case ZigbeeBridgeControllerNxp::ControllerStateStarting:
        break;
    case ZigbeeBridgeControllerNxp::ControllerStateBooting:
        break;
    case ZigbeeBridgeControllerNxp::ControllerStateRunningUninitialized: {
        qCDebug(dcZigbeeNetwork()) << "Request controller version";
        ZigbeeInterfaceNxpReply *reply = m_controller->requestVersion();
        connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
            qCDebug(dcZigbeeNetwork()) << "Version reply finished" << reply->status();
            QByteArray payload = reply->responseData();
            QDataStream stream(&payload, QIODevice::ReadOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            quint8 major = 0; quint8 minor = 0; quint8 patch = 0; quint16 sdkVersion = 0;
            stream >> major >> minor >> patch >> sdkVersion;

            QString versionString = QString ("%1.%2.%3 - %4").arg(major).arg(minor).arg(patch).arg(sdkVersion);
            qCDebug(dcZigbeeNetwork()) << "Controller version" << versionString;
            m_controller->setFirmwareVersion(versionString);

            qCDebug(dcZigbeeNetwork()) << "Set pan id" << ZigbeeUtils::convertUint64ToHexString(extendedPanId()) << extendedPanId();
            ZigbeeInterfaceNxpReply *reply = m_controller->requestSetPanId(extendedPanId());
            connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [this, reply](){
                qCDebug(dcZigbeeNetwork()) << "Set PAN ID reply finished" << reply->status();

                qCDebug(dcZigbeeNetwork()) << "Set channel mask" << channelMask() << ZigbeeUtils::convertUint32ToHexString(channelMask().toUInt32()) << channelMask().toUInt32();
                ZigbeeInterfaceNxpReply *reply = m_controller->requestSetChannelMask(channelMask().toUInt32());
                connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [reply](){
                    qCDebug(dcZigbeeNetwork()) << "Set channel mask reply finished" << reply->status();

                });
            });
        });
        break;
    }
    case ZigbeeBridgeControllerNxp::ControllerStateNotRunning:
        break;
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
    ZigbeeInterfaceNxpReply *reply = m_controller->requestFactoryResetController();
    connect(reply, &ZigbeeInterfaceNxpReply::finished, this, [](){
        qCDebug(dcZigbeeNetwork()) << "Factory reset reply finished";
    });
}
