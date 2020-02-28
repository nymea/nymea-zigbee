#include "zigbeenodenxp.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeNodeNxp::ZigbeeNodeNxp(ZigbeeBridgeControllerNxp *controller, QObject *parent):
    ZigbeeNode(parent),
    m_controller(controller)
{

}

void ZigbeeNodeNxp::setInitState(ZigbeeNodeNxp::InitState initState)
{
    m_initState = initState;

    switch (m_initState) {
    case InitStateNone:
        break;
    case InitStateNodeDescriptor: {
        qCDebug(dcZigbeeNode()) << "Request node descriptor for" << this;
        ZigbeeInterfaceReply *reply = m_controller->commandNodeDescriptorRequest(shortAddress());
        connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply](){
            reply->deleteLater();

            if (reply->status() != ZigbeeInterfaceReply::Success) {
                qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            }

            setNodeDescriptorRawData(reply->additionalMessage().data());
            setInitState(InitStatePowerDescriptor);
        });
        break;
    }
    case InitStatePowerDescriptor: {
        qCDebug(dcZigbeeNode()) << "Request power descriptor for" << this;
        ZigbeeInterfaceReply *reply = m_controller->commandPowerDescriptorRequest(shortAddress());
        connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply](){
            reply->deleteLater();

            if (reply->status() != ZigbeeInterfaceReply::Success) {
                qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            }

            QByteArray data = reply->additionalMessage().data();
            quint8 sequenceNumber = 0;
            quint8 status = 0;
            quint16 powerDescriptorFlag = 0;

            QDataStream stream(&data, QIODevice::ReadOnly);
            stream >> sequenceNumber >> status >> powerDescriptorFlag;
            setPowerDescriptorFlag(powerDescriptorFlag);
            setInitState(InitStateActiveEndpoints);
        });
        break;
    }
    case InitStateActiveEndpoints: {
        qCDebug(dcZigbeeNode()) << "Request active endpoints for" << this;
        ZigbeeInterfaceReply *reply = m_controller->commandActiveEndpointsRequest(shortAddress());
        connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply](){
            reply->deleteLater();

            if (reply->status() != ZigbeeInterfaceReply::Success) {
                qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            } else {

                QByteArray data = reply->additionalMessage().data();
                quint8 sequenceNumber = 0;
                quint8 status = 0;
                quint16 shortAddress = 0;
                quint8 endpointCount = 0;
                QList<quint8> endpointIds;

                QDataStream stream(&data, QIODevice::ReadOnly);
                stream >> sequenceNumber >> status >> shortAddress >> endpointCount;

                qCDebug(dcZigbeeNode()) << "Active endpoint list received:";
                qCDebug(dcZigbeeNode()) << "Sequence number" << sequenceNumber;
                qCDebug(dcZigbeeNode()) << "Status:" << status;
                qCDebug(dcZigbeeNode()) << "Short address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
                qCDebug(dcZigbeeNode()) << "Endpoint count:" << endpointCount;
                for (int i = 0; i < endpointCount; i++) {
                    quint8 endpointId = 0;
                    stream >> endpointId;
                    endpointIds.append(endpointId);
                    qCDebug(dcZigbeeNode()) << " - " << ZigbeeUtils::convertByteToHexString(endpointId);
                }
            }
            setState(StateInitialized);
        });
        break;
    }
    case InitStateSimpleDescriptors:

        break;
    }
}

void ZigbeeNodeNxp::startInitialization()
{
    qCDebug(dcZigbeeNode()) << "Start initialization" << this;
    setState(StateInitializing);
    setInitState(InitStateNodeDescriptor);
}
