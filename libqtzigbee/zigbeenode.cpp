#include "zigbeenode.h"
#include "zigbeeutils.h"
#include "loggingcategory.h"

#include <QDataStream>

quint16 ZigbeeNode::shortAddress() const
{
    return m_shortAddress;
}

ZigbeeAddress ZigbeeNode::extendedAddress() const
{
    return m_extendedAddress;
}

ZigbeeNode::NodeType ZigbeeNode::nodeType() const
{
    return m_nodeType;
}

void ZigbeeNode::init()
{
    requestNodeDescription();
    requestSimpleNodeDescription();
    requestPowerDescriptor();
}

void ZigbeeNode::requestNodeDescription()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_shortAddress;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeNodeDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNodeDescriptorRsponse);
    request.setDescription("Node descriptor request for " + ZigbeeUtils::convertUint16ToHexString(m_shortAddress));
    request.setTimoutIntervall(10000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNode::onRequestNodeDescriptionFinished);
}

void ZigbeeNode::requestSimpleNodeDescription()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_shortAddress;
    stream << quint8(1);

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSimpleDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeSimpleDescriptorResponse);
    request.setDescription("Simple node descriptor request for " + ZigbeeUtils::convertUint16ToHexString(m_shortAddress));
    request.setTimoutIntervall(10000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNode::onRequestSimpleNodeDescriptionFinished);
}

void ZigbeeNode::requestPowerDescriptor()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_shortAddress;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypePowerDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypePowerDescriptorResponse);
    request.setDescription("Node power descriptor request for " + ZigbeeUtils::convertUint16ToHexString(m_shortAddress));
    request.setTimoutIntervall(10000);

    ZigbeeInterfaceReply *reply = controller()->sendRequest(request);
    connect(reply, &ZigbeeInterfaceReply::finished, this, &ZigbeeNode::onRequestPowerDescriptorFinished);
}

ZigbeeNode::ZigbeeNode(ZigbeeBridgeController *controller, QObject *parent) :
    QObject(parent),
    m_controller(controller)
{

}

ZigbeeBridgeController *ZigbeeNode::controller()
{
    return m_controller;
}

void ZigbeeNode::setShortAddress(const quint16 &shortAddress)
{
    m_shortAddress = shortAddress;
}

void ZigbeeNode::setExtendedAddress(const ZigbeeAddress &extendedAddress)
{
    m_extendedAddress = extendedAddress;
}

void ZigbeeNode::onRequestNodeDescriptionFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";

    quint8 sequenceNumber = static_cast<quint8>(reply->additionalMessage().data().at(0));
    quint8 status = static_cast<quint8>(reply->additionalMessage().data().at(1));

    quint16 shortAddress = reply->additionalMessage().data().at(2);
    shortAddress <<= 8;
    shortAddress |= reply->additionalMessage().data().at(3);

    quint16 manufacturerCode = reply->additionalMessage().data().at(4);
    manufacturerCode <<= 8;
    manufacturerCode |= reply->additionalMessage().data().at(5);

    quint16 maximalRxSize = reply->additionalMessage().data().at(6);
    maximalRxSize <<= 8;
    maximalRxSize |= reply->additionalMessage().data().at(7);

    quint16 maximalTxSize = reply->additionalMessage().data().at(8);
    maximalTxSize <<= 8;
    maximalTxSize |= reply->additionalMessage().data().at(9);

    quint16 serverMask = reply->additionalMessage().data().at(10);
    serverMask <<= 8;
    serverMask |= reply->additionalMessage().data().at(11);

    quint8 descriptorFlag = static_cast<quint8>(reply->additionalMessage().data().at(12));
    quint8 macFlags = static_cast<quint8>(reply->additionalMessage().data().at(13));
    quint8 maxBufferSize = static_cast<quint8>(reply->additionalMessage().data().at(14));

    quint16 bitField = reply->additionalMessage().data().at(15);
    bitField <<= 8;
    bitField |= reply->additionalMessage().data().at(16);

    // TODO: find node for short address and set data

    qCDebug(dcZigbee()) << "Node descriptor:";
    qCDebug(dcZigbee()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbee()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbee()) << "    Short address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
    qCDebug(dcZigbee()) << "    Manufacturer code:" << ZigbeeUtils::convertUint16ToHexString(manufacturerCode);
    qCDebug(dcZigbee()) << "    Maximum Rx size:" << ZigbeeUtils::convertUint16ToHexString(maximalRxSize);
    qCDebug(dcZigbee()) << "    Maximum Tx size:" << ZigbeeUtils::convertUint16ToHexString(maximalTxSize);
    qCDebug(dcZigbee()) << "    Server makk:" << ZigbeeUtils::convertUint16ToHexString(serverMask);
    qCDebug(dcZigbee()) << "    Descriptor flag:" << ZigbeeUtils::convertByteToHexString(descriptorFlag);
    qCDebug(dcZigbee()) << "    MAC flags:" << ZigbeeUtils::convertByteToHexString(macFlags);
    qCDebug(dcZigbee()) << "    Maximum buffer size:" << ZigbeeUtils::convertByteToHexString(maxBufferSize);
    qCDebug(dcZigbee()) << "    Bit field:" << ZigbeeUtils::convertUint16ToHexString(bitField);
}

void ZigbeeNode::onRequestSimpleNodeDescriptionFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    qCDebug(dcZigbeeController()) << reply->additionalMessage();

    quint8 sequenceNumber = static_cast<quint8>(reply->additionalMessage().data().at(0));
    quint8 status = static_cast<quint8>(reply->additionalMessage().data().at(1));

    quint16 nwkAddress = reply->additionalMessage().data().at(2);
    nwkAddress <<= 8;
    nwkAddress |= reply->additionalMessage().data().at(3);

    quint8 length = static_cast<quint8>(reply->additionalMessage().data().at(4));
    quint8 endPoint = static_cast<quint8>(reply->additionalMessage().data().at(5));

    quint16 profileId = reply->additionalMessage().data().at(6);
    profileId <<= 8;
    profileId |= reply->additionalMessage().data().at(7);

    quint16 deviceId = reply->additionalMessage().data().at(8);
    deviceId <<= 8;
    deviceId |= reply->additionalMessage().data().at(9);

    quint8 bitField = static_cast<quint8>(reply->additionalMessage().data().at(10));

    qCDebug(dcZigbee()) << "Node simple descriptor:";
    qCDebug(dcZigbee()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbee()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbee()) << "    Nwk address:" << ZigbeeUtils::convertUint16ToHexString(nwkAddress);
    qCDebug(dcZigbee()) << "    Lenght:" << ZigbeeUtils::convertByteToHexString(length);
    qCDebug(dcZigbee()) << "    End Point:" << ZigbeeUtils::convertByteToHexString(endPoint);
    qCDebug(dcZigbee()) << "    Profile:" << ZigbeeUtils::profileIdToString((Zigbee::ZigbeeProfile)profileId);

    if (profileId == Zigbee::ZigbeeProfileLightLink) {
        qCDebug(dcZigbee()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::LightLinkDevice>(deviceId);
    } else {
        qCDebug(dcZigbee()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::HomeAutomationDevice>(deviceId);
    }

    qCDebug(dcZigbee()) << "    Bit field:" << ZigbeeUtils::convertByteToHexString(bitField);

    quint8 inputClusterCount = static_cast<quint8>(reply->additionalMessage().data().at(10));

    qCDebug(dcZigbee()) << "    Input clusters:";
    QByteArray inputClusterListData = reply->additionalMessage().data().mid(11, inputClusterCount * 2);
    for (int i = 0; i < inputClusterListData.count(); i+=2) {
        quint16 clusterId = inputClusterListData.at(i);
        clusterId <<= 8;
        clusterId |= inputClusterListData .at(i+1);

        qCDebug(dcZigbee()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
    }

    quint8 outputClusterCount = static_cast<quint8>(reply->additionalMessage().data().at(12 + inputClusterCount * 2));

    qCDebug(dcZigbee()) << "    Output clusters:";
    QByteArray outputClusterListData = reply->additionalMessage().data().mid(12 + inputClusterCount * 2, outputClusterCount * 2);
    for (int i = 0; i < outputClusterListData.count(); i+=2) {
        quint16 clusterId = outputClusterListData.at(i);
        clusterId <<= 8;
        clusterId |= outputClusterListData .at(i+1);

        qCDebug(dcZigbee()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
    }

}

void ZigbeeNode::onRequestPowerDescriptorFinished()
{
    ZigbeeInterfaceReply *reply = static_cast<ZigbeeInterfaceReply *>(sender());
    reply->deleteLater();

    if (reply->status() != ZigbeeInterfaceReply::Success) {
        qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
        return;
    }

    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
    qCDebug(dcZigbeeController()) << reply->additionalMessage();

    quint8 sequenceNumber = static_cast<quint8>(reply->additionalMessage().data().at(0));
    quint8 status = static_cast<quint8>(reply->additionalMessage().data().at(1));

    quint16 bitField = reply->additionalMessage().data().at(2);
    bitField <<= 8;
    bitField |= reply->additionalMessage().data().at(3);

    qCDebug(dcZigbee()) << "Node power descriptor:";
    qCDebug(dcZigbee()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
    qCDebug(dcZigbee()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
    qCDebug(dcZigbee()) << "    Bitfiled:" << ZigbeeUtils::convertUint16ToHexString(bitField);
}

QDebug operator<<(QDebug debug, ZigbeeNode *node)
{
    debug.nospace().noquote() << "Node(" << node->shortAddress() << ", " << node->extendedAddress().toString() << ") ";
    return debug;
}
