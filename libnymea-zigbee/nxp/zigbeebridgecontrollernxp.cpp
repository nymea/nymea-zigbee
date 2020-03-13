/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea-zigbee.
* This project including source code and documentation is protected by copyright law, and
* remains the property of nymea GmbH. All rights, including reproduction, publication,
* editing and translation, are reserved. The use of this project is subject to the terms of a
* license agreement to be concluded with nymea GmbH in accordance with the terms
* of use of nymea GmbH, available under https://nymea.io/license
*
* GNU Lesser General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the terms of the GNU
* Lesser General Public License as published by the Free Software Foundation; version 3.
* this project is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License along with this project.
* If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under contact@nymea.io
* or see our FAQ/Licensing Information on https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "zigbeebridgecontrollernxp.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeBridgeControllerNxp::ZigbeeBridgeControllerNxp(QObject *parent) :
    ZigbeeBridgeController(parent)
{
    m_interface = new ZigbeeInterface(this);
    connect(m_interface, &ZigbeeInterface::availableChanged, this, &ZigbeeBridgeControllerNxp::onInterfaceAvailableChanged);
    connect(m_interface, &ZigbeeInterface::messageReceived, this, &ZigbeeBridgeControllerNxp::onMessageReceived);
}

ZigbeeBridgeControllerNxp::~ZigbeeBridgeControllerNxp()
{
    qCDebug(dcZigbeeController()) << "Destroy controller";
}

void ZigbeeBridgeControllerNxp::setFirmwareVersionString(const QString &firmwareVersion)
{
    setFirmwareVersion(firmwareVersion);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandResetController()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeReset, QByteArray()));
    request.setDescription("Reset controller");
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandSoftResetController()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeZllFactoryNew, QByteArray()));
    request.setDescription("Soft reset controller");
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandErasePersistantData()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeErasePersistentData, QByteArray()));
    request.setDescription("Erase persistent data");

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandGetVersion()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeGetVersion, QByteArray()));
    request.setDescription("Get version");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeVersionList);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandSetExtendedPanId(quint64 extendedPanId)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << extendedPanId;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSetExtendetPanId, data));
    request.setDescription("Set extended PAN id " + QString::number(extendedPanId) + " " + ZigbeeUtils::convertUint64ToHexString(extendedPanId));

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandSetChannelMask(quint32 channelMask)
{
    // Note: 10 < value < 27 -> using sinle channel value
    //       0x07fff800 select from all channels 11 - 26
    //       0x2108800 primary zigbee light link channels 11, 15, 20, 25

    //Zigbee::ZigbeeChannels channels = (Zigbee::ZigbeeChannel11 | Zigbee::ZigbeeChannel15 | Zigbee::ZigbeeChannel20 | Zigbee::ZigbeeChannel25);

    qCDebug(dcZigbeeController()) << "Set channel mask" << ZigbeeUtils::convertUint32ToHexString(channelMask);
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << channelMask;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSetChannelMask, data));
    request.setDescription("Set channel mask" + ZigbeeUtils::convertByteArrayToHexString(data));

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandSetNodeType(ZigbeeNode::NodeType nodeType)
{
    quint8 deviceTypeValue = 0;
    if (nodeType == ZigbeeNode::NodeTypeEndDevice) {
        qCWarning(dcZigbeeController()) << "Set the controller as EndDevice is not allowed. Default to coordinator node type.";
        deviceTypeValue = static_cast<quint8>(ZigbeeNode::NodeTypeCoordinator);
    } else {
        deviceTypeValue = static_cast<quint8>(nodeType);
    }

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << deviceTypeValue;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSetDeviceType, data));

    switch (nodeType) {
    case ZigbeeNode::NodeTypeCoordinator:
        request.setDescription("Set device type coordinator");
        break;
    case ZigbeeNode::NodeTypeRouter:
        request.setDescription("Set device type router");
        break;
    default:
        break;
    }

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandStartNetwork()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeStartNetwork, QByteArray()));
    request.setDescription("Start network");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNetworkJoinedFormed);
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandStartScan()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeStartScan, QByteArray()));
    request.setDescription("Start scan");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNetworkJoinedFormed);
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandPermitJoin(quint16 targetAddress, const quint8 advertisingIntervall, bool tcSignificance)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << targetAddress;
    stream << advertisingIntervall;
    stream << static_cast<quint8>(tcSignificance);

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypePermitJoiningRequest, data));
    request.setDescription("Permit joining request on " + ZigbeeUtils::convertUint16ToHexString(targetAddress) + " for " + QString::number(advertisingIntervall) + "[s]");

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandGetPermitJoinStatus()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeGetPermitJoining, QByteArray()));
    request.setDescription("Get permit joining status");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeGetPermitJoiningResponse);
    request.setTimoutIntervall(1000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandActiveEndpointsRequest(quint16 shortAddress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeActiveEndpointRequest, data));
    request.setDescription("Get active endpoints");
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeActiveEndpointResponse);
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandRequestLinkQuality(quint16 shortAddress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;
    stream << static_cast<quint8>(0);

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeManagementLqiRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeManagementLqiResponse);
    request.setDescription("Request link quality request for " + ZigbeeUtils::convertUint16ToHexString(shortAddress));
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandEnableWhiteList()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeNetworkWhitelistEnable, QByteArray()));
    request.setDescription("Enable whitelist");

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandInitiateTouchLink()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeInitiateTouchlink, QByteArray()));
    request.setDescription("Initiate touch link");

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandTouchLinkFactoryReset()
{
    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeTouchlinkFactoryReset, QByteArray()));
    request.setDescription("Touch link factory reset");

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandNetworkAddressRequest(quint16 targetAddress, quint64 extendedAddress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << targetAddress;
    stream << extendedAddress;
    stream << static_cast<quint8>(1);
    stream << static_cast<quint8>(0);

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeNetworkAdressRequest, data));
    request.setDescription("Network address request on " + ZigbeeUtils::convertUint16ToHexString(targetAddress));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNetworkAdressResponse);
    request.setTimoutIntervall(1000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandSetSecurityStateAndKey(quint8 keyState, quint8 keySequence, quint8 keyType, const QByteArray &key)
{
    // Note: calls ZPS_vAplSecSetInitialSecurityState

    // Key state:
    //      ZPS_ZDO_PRECONFIGURED_LINK_KEY = 3
    //          This key will be used to encrypt the network key. This is the master or manufacturer key

    //      ZPS_ZDO_ZLL_LINK_KEY = 4
    //          This key will be generated by the trust center.

    // Key Type:
    //      ZPS_APS_UNIQUE_LINK_KEY =

    qCDebug(dcZigbeeController()) << "Set security" << ZigbeeUtils::convertByteArrayToHexString(key) << qUtf8Printable(key) << key;

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << keyState;
    stream << keySequence;
    stream << keyType;
    for (int i = 0; i < key.count(); i++) {
        stream << static_cast<quint8>(key.at(i));
    }

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSetSecurity, data));
    request.setDescription("Set security configuration");

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandAuthenticateDevice(const ZigbeeAddress &ieeeAddress, const QByteArray &key)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << ieeeAddress.toUInt64();
    stream << key;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeAuthenticateDeviceRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeAuthenticateDeviceResponse);
    request.setDescription(QString("Authenticate device %1").arg(ieeeAddress.toString()));
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandNodeDescriptorRequest(quint16 shortAddress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeNodeDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeNodeDescriptorRsponse);
    request.setDescription("Node descriptor request for " + ZigbeeUtils::convertUint16ToHexString(shortAddress));
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandSimpleDescriptorRequest(quint16 shortAddress, quint8 endpoint)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;
    stream << endpoint;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeSimpleDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeSimpleDescriptorResponse);
    request.setDescription("Simple node descriptor request for " + ZigbeeUtils::convertUint16ToHexString(shortAddress) + " endpoint " + QString::number(endpoint));
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandPowerDescriptorRequest(quint16 shortAddress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypePowerDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypePowerDescriptorResponse);
    request.setDescription("Node power descriptor request for " + ZigbeeUtils::convertUint16ToHexString(shortAddress));
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandUserDescriptorRequest(quint16 shortAddress, quint16 address)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << shortAddress;
    stream << address;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeUserDescriptorRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeUserDescriptorResponse);
    request.setDescription("Node user descriptor request for " + ZigbeeUtils::convertUint16ToHexString(shortAddress) + " " + ZigbeeUtils::convertUint16ToHexString(address));
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandFactoryResetNode(quint16 shortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << static_cast<quint8>(0x02);
    stream << shortAddress;
    stream << sourceEndpoint;
    stream << destinationEndpoint;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageBasicResetFactoryDefaults, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageBasicResetFactoryDefaultsResponse);
    request.setDescription("Factory reset node " + ZigbeeUtils::convertUint16ToHexString(shortAddress) + " " + ZigbeeUtils::convertByteToHexString(destinationEndpoint));
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandReadAttributeRequest(quint8 addressMode, quint16 shortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, ZigbeeCluster *cluster, QList<quint16> attributes, bool manufacturerSpecific, quint16 manufacturerId)
{
    // Address mode: TODO

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << addressMode;
    stream << shortAddress;
    stream << sourceEndpoint;
    stream << destinationEndpoint;
    stream << static_cast<quint16>(cluster->clusterId());

    // 0: server -> client, 1: client -> server
    if (cluster->direction() == ZigbeeCluster::Input) {
        stream << static_cast<quint8>(0);
    } else {
        stream << static_cast<quint8>(1);
    }

    if (manufacturerSpecific) {
        stream << static_cast<quint8>(1);
    } else {
        stream << static_cast<quint8>(0);
    }

    stream << manufacturerId;
    stream << static_cast<quint8>(attributes.count());
    for (int i = 0; i < attributes.count(); i++) {
        stream << attributes.at(i);
    }

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeReadAttributeRequest, data));
    //request.setExpectedAdditionalMessageType(Zigbee::MessageTypeReadAttributeResponse);
    request.setDescription("Read attribute request for " + ZigbeeUtils::convertUint16ToHexString(shortAddress));
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandConfigureReportingRequest(quint8 addressMode, quint16 shortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, ZigbeeCluster *cluster, quint8 direction, bool manufacturerSpecific, quint16 manufacturerId, QList<ZigbeeClusterReportConfigurationRecord> reportConfigurations)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << addressMode; // [0]
    stream << shortAddress; // [1] [2]
    stream << sourceEndpoint; // [3]
    stream << destinationEndpoint; // [4]
    stream << static_cast<quint16>(cluster->clusterId()); // [5] [6]

    // 0: server -> client, 1: client -> server
    stream << direction;// [7]
    stream << static_cast<quint8>(manufacturerSpecific); // [8]
    stream << manufacturerId; // [9] [10]
    stream << static_cast<quint8>(reportConfigurations.count()); // [11]
    for (int i = 0; i < reportConfigurations.count(); i++) {
        // Configuration report
        stream << reportConfigurations.at(i).direction;
        stream << static_cast<quint8>(reportConfigurations.at(i).dataType);
        stream << reportConfigurations.at(i).attributeId;
        stream << reportConfigurations.at(i).minInterval;
        stream << reportConfigurations.at(i).maxInterval;
        stream << reportConfigurations.at(i).timeout;
        stream << reportConfigurations.at(i).change;
    }

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeConfigReportingRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeConfigReportingResponse);
    request.setDescription("Configure reporting request for " + ZigbeeUtils::convertUint16ToHexString(shortAddress));
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandIdentify(quint8 addressMode, quint16 shortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint16 duration)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << addressMode;
    stream << shortAddress;
    stream << sourceEndpoint;
    stream << destinationEndpoint;
    stream << duration;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeIdentifySend, data));
    request.setDescription("Identify request for " + ZigbeeUtils::convertUint16ToHexString(shortAddress));
    request.setTimoutIntervall(12000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandBindGroup(const ZigbeeAddress &sourceAddress, quint8 sourceEndpoint, quint16 clusterId, quint16 destinationAddress, quint8 destinationEndpoint)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << sourceAddress.toUInt64();
    stream << sourceEndpoint;
    stream << clusterId;
    stream << static_cast<quint8>(Zigbee::DestinationAddressModeGroup);
    stream << destinationAddress;
    stream << destinationEndpoint;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeBind, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeBindResponse);
    request.setDescription("Bind group request");
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandBindUnicast(const ZigbeeAddress &sourceAddress, quint8 sourceEndpoint, quint16 clusterId, const ZigbeeAddress &destinationAddress, quint8 destinationEndpoint)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << sourceAddress.toUInt64();
    stream << sourceEndpoint;
    stream << clusterId;
    stream << static_cast<quint8>(Zigbee::DestinationAddressModeUnicastIeee);
    stream << destinationAddress.toUInt64();
    stream << destinationEndpoint;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeBind, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeBindResponse);
    request.setDescription("Bind unicast request");
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandBindLocalGroup(quint16 clusterId, quint8 sourceEndpoint, quint16 groupAddress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << clusterId;
    stream << sourceEndpoint;
    stream << groupAddress;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeBindGroup, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeBindGroupResponse);
    request.setDescription("Bind local group request");
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandOnOffNoEffects(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, ZigbeeCluster::OnOffClusterCommand command)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << addressMode;
    stream << targetShortAddress;
    stream << sourceEndpoint;
    stream << destinationEndpoint;
    stream << static_cast<quint8>(command);

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeCluserOnOff, data));
    request.setDescription("Send on/off cluster command");
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandAddGroup(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint16 groupAddress)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << addressMode;
    stream << targetShortAddress;
    stream << sourceEndpoint;
    stream << destinationEndpoint;
    stream << groupAddress;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeAddGroupRequest, data));
    request.setExpectedAdditionalMessageType(Zigbee::MessageTypeAddGroupResponse);
    request.setDescription("Add group request");
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandMoveToLevel(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint8 onOff, quint8 level, quint16 transitionTime)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << addressMode;
    stream << targetShortAddress;
    stream << sourceEndpoint;
    stream << destinationEndpoint;
    stream << static_cast<quint8>(onOff);
    stream << level;
    stream << transitionTime;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeMoveToLevelOnOff, data));
    //request.setExpectedAdditionalMessageType(Zigbee::MessageTypeDefaultResponse);
    request.setDescription(QString("Move to level on/off %1").arg(level));
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandMoveToColourTemperature(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint16 colourTemperature, quint16 transitionTime)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << addressMode;
    stream << targetShortAddress;
    stream << sourceEndpoint;
    stream << destinationEndpoint;
    stream << colourTemperature;
    stream << transitionTime;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeMoveToColorTemperature, data));
    //request.setExpectedAdditionalMessageType(Zigbee::MessageTypeDefaultResponse);
    request.setDescription(QString("Move to colour temperature %1").arg(colourTemperature));
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandMoveToColor(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint16 x, quint16 y, quint16 transitionTime)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << addressMode;
    stream << targetShortAddress;
    stream << sourceEndpoint;
    stream << destinationEndpoint;
    stream << x << y;
    stream << transitionTime;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeMoveToColor, data));
    //request.setExpectedAdditionalMessageType(Zigbee::MessageTypeDefaultResponse);
    request.setDescription(QString("Move to colour %1, %2").arg(x).arg(y));
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandMoveToHueSaturation(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint8 hue, quint8 saturation, quint16 transitionTime)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << addressMode;
    stream << targetShortAddress;
    stream << sourceEndpoint;
    stream << destinationEndpoint;
    stream << hue << saturation;
    stream << transitionTime;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeMoveToHueSaturation, data));
    //request.setExpectedAdditionalMessageType(Zigbee::MessageTypeDefaultResponse);
    request.setDescription(QString("Move to hue %1 saturation %2").arg(hue).arg(saturation));
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandMoveToHue(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint8 hue, quint16 transitionTime)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << addressMode;
    stream << targetShortAddress;
    stream << sourceEndpoint;
    stream << destinationEndpoint;
    stream << hue;
    stream << transitionTime;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeMoveToHue, data));
    //request.setExpectedAdditionalMessageType(Zigbee::MessageTypeDefaultResponse);
    request.setDescription(QString("Move to hue %1").arg(hue));
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::commandMoveToSaturation(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint8 saturation, quint16 transitionTime)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << addressMode;
    stream << targetShortAddress;
    stream << sourceEndpoint;
    stream << destinationEndpoint;
    stream << saturation;
    stream << transitionTime;

    ZigbeeInterfaceRequest request(ZigbeeInterfaceMessage(Zigbee::MessageTypeMoveToSaturation, data));
    //request.setExpectedAdditionalMessageType(Zigbee::MessageTypeDefaultResponse);
    request.setDescription(QString("Move to saturation %1").arg(saturation));
    request.setTimoutIntervall(5000);

    return sendRequest(request);
}

void ZigbeeBridgeControllerNxp::sendMessage(ZigbeeInterfaceReply *reply)
{
    if (!reply)
        return;

    m_currentReply = reply;
    qCDebug(dcZigbeeController()) << "Sending request:" << reply->request().description();

    m_interface->sendMessage(reply->request().message());
    reply->startTimer(reply->request().timeoutIntervall());
}

void ZigbeeBridgeControllerNxp::onInterfaceAvailableChanged(bool available)
{
    setAvailable(available);
}

void ZigbeeBridgeControllerNxp::onMessageReceived(const ZigbeeInterfaceMessage &message)
{
    // Check if we have a current reply
    if (m_currentReply) {
        if (message.messageType() == Zigbee::MessageTypeStatus) {
            // We have a status message for the current reply
            m_currentReply->setStatusMessage(message);
            qCDebug(dcZigbeeController()) << "Current request" << m_currentReply->request().description() << "status message received";
            // TODO: check if success, if not, finish reply
        } else if (m_currentReply->request().expectsAdditionalMessage() &&
                   message.messageType() == m_currentReply->request().expectedAdditionalMessageType()) {
            m_currentReply->setAdditionalMessage(message);
            qCDebug(dcZigbeeController()) << "Current request" << m_currentReply->request().description() << "additional message received";
        } else {
            // Not a reply related message
            qCDebug(dcZigbeeController()) << "Current request" << m_currentReply->request().description() << "but not related message received";
            emit messageReceived(message);
            return;
        }

        // Check if request is complete
        if (m_currentReply->isComplete()) {
            qCDebug(dcZigbeeController()) << "Current request" << m_currentReply->request().description() << "is complete!";
            m_currentReply->setFinished();

            // Note: the request class has to take care about the reply object
            m_currentReply = nullptr;

            if (!m_replyQueue.isEmpty())
                sendMessage(m_replyQueue.dequeue());

            return;
        }
    } else {
        // Not a reply message
        emit messageReceived(message);
    }

}

void ZigbeeBridgeControllerNxp::onReplyTimeout()
{
    m_currentReply->setFinished();
    m_currentReply = nullptr;

    if (!m_replyQueue.isEmpty())
        sendMessage(m_replyQueue.dequeue());

}

bool ZigbeeBridgeControllerNxp::enable(const QString &serialPort, qint32 baudrate)
{
    return m_interface->enable(serialPort, baudrate);
}

void ZigbeeBridgeControllerNxp::disable()
{
    m_interface->disable();
}

ZigbeeInterfaceReply *ZigbeeBridgeControllerNxp::sendRequest(const ZigbeeInterfaceRequest &request)
{
    // Create Reply
    ZigbeeInterfaceReply *reply = new ZigbeeInterfaceReply(request);
    connect(reply, &ZigbeeInterfaceReply::timeout, this, &ZigbeeBridgeControllerNxp::onReplyTimeout);

    // If reply running, enqueue, else send request
    if (m_currentReply) {
        m_replyQueue.enqueue(reply);
    } else {
        sendMessage(reply);
    }

    return reply;
}
