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

#include "zigbeeutils.h"
#include "zigbeenodedeconz.h"
#include "zigbeedeviceprofile.h"
#include "zigbeenetworkdeconz.h"
#include "loggingcategory.h"

#include <QDataStream>

ZigbeeNodeDeconz::ZigbeeNodeDeconz(ZigbeeNetworkDeconz *network, QObject *parent) :
    ZigbeeNode(parent),
    m_network(network)
{

}

ZigbeeNetworkReply *ZigbeeNodeDeconz::requestNodeDescriptor()
{
    // Get the node descriptor
    ZigbeeNetworkRequest request;
    request.setRequestId(m_network->generateSequenceNumber());
    request.setDestinationAddressMode(Zigbee::DestinationAddressModeShortAddress);
    request.setDestinationShortAddress(shortAddress());
    request.setDestinationEndpoint(0); // ZDO
    request.setProfileId(Zigbee::ZigbeeProfileDevice); // ZDP
    request.setClusterId(ZigbeeDeviceProfile::NodeDescriptorRequest);
    request.setSourceEndpoint(0); // ZDO

    // Build ASDU
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << request.requestId() << request.destinationShortAddress();
    request.setAsdu(asdu);

    // We expect an indication with the response and the confirmation
    request.setExpectIndication(true);
    return m_network->sendRequest(request);
}

ZigbeeNetworkReply *ZigbeeNodeDeconz::requestPowerDescriptor()
{
    // Get the power descriptor
    ZigbeeNetworkRequest request;
    request.setRequestId(m_network->generateSequenceNumber());
    request.setDestinationAddressMode(Zigbee::DestinationAddressModeShortAddress);
    request.setDestinationShortAddress(shortAddress());
    request.setDestinationEndpoint(0); // ZDO
    request.setProfileId(Zigbee::ZigbeeProfileDevice); // ZDP
    request.setClusterId(ZigbeeDeviceProfile::PowerDescriptorRequest);
    request.setSourceEndpoint(0); // ZDO

    // Build ASDU
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << request.requestId() << request.destinationShortAddress();
    request.setAsdu(asdu);

    // We expect an indication with the response and the confirmation
    request.setExpectIndication(true);
    return m_network->sendRequest(request);
}

ZigbeeNetworkReply *ZigbeeNodeDeconz::requestActiveEndpoints()
{
    // Get the power descriptor
    ZigbeeNetworkRequest request;
    request.setRequestId(m_network->generateSequenceNumber());
    request.setDestinationAddressMode(Zigbee::DestinationAddressModeShortAddress);
    request.setDestinationShortAddress(shortAddress());
    request.setDestinationEndpoint(0); // ZDO
    request.setProfileId(Zigbee::ZigbeeProfileDevice); // ZDP
    request.setClusterId(ZigbeeDeviceProfile::ActiveEndpointsRequest);
    request.setSourceEndpoint(0); // ZDO

    // Build ASDU
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << request.requestId() << request.destinationShortAddress();
    request.setAsdu(asdu);

    // We expect an indication with the response and the confirmation
    request.setExpectIndication(true);
    return m_network->sendRequest(request);
}

ZigbeeNetworkReply *ZigbeeNodeDeconz::requestSimpleDescriptor(quint8 endpoint)
{
    // Get the power descriptor
    ZigbeeNetworkRequest request;
    request.setRequestId(m_network->generateSequenceNumber());
    request.setDestinationAddressMode(Zigbee::DestinationAddressModeShortAddress);
    request.setDestinationShortAddress(shortAddress());
    request.setDestinationEndpoint(0); // ZDO
    request.setProfileId(Zigbee::ZigbeeProfileDevice); // ZDP
    request.setClusterId(ZigbeeDeviceProfile::SimpleDescriptorRequest);
    request.setSourceEndpoint(0); // ZDO

    // Build ASDU
    QByteArray asdu;
    QDataStream stream(&asdu, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << request.requestId() << request.destinationShortAddress() << endpoint;
    request.setAsdu(asdu);

    // We expect an indication with the response and the confirmation
    request.setExpectIndication(true);
    return m_network->sendRequest(request);
}

void ZigbeeNodeDeconz::leaveNetworkRequest(bool rejoin, bool removeChildren)
{
    Q_UNUSED(rejoin)
    Q_UNUSED(removeChildren)
}

void ZigbeeNodeDeconz::setClusterAttributeReport(const ZigbeeClusterAttributeReport &report)
{
    Q_UNUSED(report)
}

void ZigbeeNodeDeconz::startInitialization()
{
    setState(StateInitializing);

    /* Node initialisation steps (sequentially)
     * - Node descriptor
     * - Power descriptor
     * - Active endpoints
     * - for each endpoint do:
     *    - Simple descriptor request
     *    - for each endpoint
     *      - read basic cluster
     */


    ZigbeeNetworkReply *reply = requestNodeDescriptor();
    connect(reply, &ZigbeeNetworkReply::finished, this, [this, reply](){
        // TODO: check reply error

        ZigbeeDeviceProfileAdpu adpu = ZigbeeDeviceProfile::parseAdpu(reply->responseData());
        qCDebug(dcZigbeeNode()) << "Node descriptor request finished" << adpu;
        QDataStream stream(adpu.payload);
        stream.setByteOrder(QDataStream::LittleEndian);
        quint8 typeDescriptorFlag = 0; quint8 frequencyFlag = 0; quint8 macCapabilities = 0;
        quint16 serverMask = 0;
        quint8 descriptorCapabilities = 0;

        stream >> typeDescriptorFlag >> frequencyFlag >> macCapabilities >> m_manufacturerCode >> m_maximumBufferSize;
        stream >> m_maximumRxSize >> serverMask >> m_maximumTxSize >> descriptorCapabilities;

        // 0-2 Bit = logical type, 0 = coordinator, 1 = router, 2 = end device
        if (!ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 0) && !ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 1)) {
            m_nodeType = NodeTypeCoordinator;
        } else if (!ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 0) && ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 1)) {
            m_nodeType = NodeTypeRouter;
        } else if (ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 0) && !ZigbeeUtils::checkBitUint8(typeDescriptorFlag, 1)) {
            m_nodeType = NodeTypeEndDevice;
        }

        m_complexDescriptorAvailable = (typeDescriptorFlag >> 3) & 0x0001;
        m_userDescriptorAvailable = (typeDescriptorFlag >> 4) & 0x0001;

        // Frequency band, 5 bits
        if (ZigbeeUtils::checkBitUint8(frequencyFlag, 3)) {
            m_frequencyBand = FrequencyBand868Mhz;
        } else if (ZigbeeUtils::checkBitUint8(frequencyFlag, 5)) {
            m_frequencyBand = FrequencyBand902Mhz;
        } else if (ZigbeeUtils::checkBitUint8(frequencyFlag, 6)) {
            m_frequencyBand = FrequencyBand2400Mhz;
        }

        setMacCapabilitiesFlag(macCapabilities);
        setServerMask(serverMask);
        setDescriptorFlag(descriptorCapabilities);

        qCDebug(dcZigbeeNode()) << "Node descriptor:" << ZigbeeUtils::convertUint16ToHexString(shortAddress()) << extendedAddress().toString();
        qCDebug(dcZigbeeNode()) << "    Node type:" << nodeType();
        qCDebug(dcZigbeeNode()) << "    Complex desciptor available:" << complexDescriptorAvailable();
        qCDebug(dcZigbeeNode()) << "    User desciptor available:" << userDescriptorAvailable();
        qCDebug(dcZigbeeNode()) << "    Frequency band:" << frequencyBand();
        qCDebug(dcZigbeeNode()) << "    Manufacturer code:" << ZigbeeUtils::convertUint16ToHexString(m_manufacturerCode);
        qCDebug(dcZigbeeNode()) << "    Maximum Rx size:" << ZigbeeUtils::convertUint16ToHexString(m_maximumRxSize) << "(" <<  m_maximumRxSize << ")";
        qCDebug(dcZigbeeNode()) << "    Maximum Tx size:" << ZigbeeUtils::convertUint16ToHexString(m_maximumTxSize) << "(" << m_maximumTxSize << ")";
        qCDebug(dcZigbeeNode()) << "    Maximum buffer size:" << ZigbeeUtils::convertByteToHexString(m_maximumBufferSize) << "(" << m_maximumBufferSize << ")";
        qCDebug(dcZigbeeNode()) << "    Server mask:" << ZigbeeUtils::convertUint16ToHexString(serverMask);
        qCDebug(dcZigbeeNode()) << "        Primary Trust center:" << isPrimaryTrustCenter();
        qCDebug(dcZigbeeNode()) << "        Backup Trust center:" << isBackupTrustCenter();
        qCDebug(dcZigbeeNode()) << "        Primary Binding cache:" << isPrimaryBindingCache();
        qCDebug(dcZigbeeNode()) << "        Backup Binding cache:" << isBackupBindingCache();
        qCDebug(dcZigbeeNode()) << "        Primary Discovery cache:" << isPrimaryDiscoveryCache();
        qCDebug(dcZigbeeNode()) << "        Backup Discovery cache:" << isBackupDiscoveryCache();
        qCDebug(dcZigbeeNode()) << "        Network Manager:" << isNetworkManager();
        qCDebug(dcZigbeeNode()) << "    Descriptor flag:" << ZigbeeUtils::convertByteToHexString(descriptorCapabilities);
        qCDebug(dcZigbeeNode()) << "        Extended active endpoint list available:" << extendedActiveEndpointListAvailable();
        qCDebug(dcZigbeeNode()) << "        Extended simple descriptor list available:" << extendedSimpleDescriptorListAvailable();
        qCDebug(dcZigbeeNode()) << "    MAC flags:" << ZigbeeUtils::convertByteToHexString(macCapabilities);
        qCDebug(dcZigbeeNode()) << "        Alternate PAN coordinator:" << alternatePanCoordinator();
        qCDebug(dcZigbeeNode()) << "        Device type:" << deviceType();
        qCDebug(dcZigbeeNode()) << "        Power source flag main power:" << powerSourceFlagMainPower();
        qCDebug(dcZigbeeNode()) << "        Receiver on when idle:" << receiverOnWhenIdle();
        qCDebug(dcZigbeeNode()) << "        Security capability:" << securityCapability();
        qCDebug(dcZigbeeNode()) << "        Allocate address:" << allocateAddress();


        // Power descriptor
        ZigbeeNetworkReply *reply = requestPowerDescriptor();
        connect(reply, &ZigbeeNetworkReply::finished, this, [this, reply](){
            // TODO: check reply error

            ZigbeeDeviceProfileAdpu adpu = ZigbeeDeviceProfile::parseAdpu(reply->responseData());
            qCDebug(dcZigbeeNode()) << "Power descriptor request finished" << this << adpu;
            QDataStream stream(adpu.payload);
            stream.setByteOrder(QDataStream::LittleEndian);
            quint16 powerDescriptorFlag = 0;
            stream >> powerDescriptorFlag;
            setPowerDescriptorFlag(powerDescriptorFlag);


            ZigbeeNetworkReply *reply = requestActiveEndpoints();
            connect(reply, &ZigbeeNetworkReply::finished, this, [this, reply](){
                // TODO: check reply error

                ZigbeeDeviceProfileAdpu adpu = ZigbeeDeviceProfile::parseAdpu(reply->responseData());
                qCDebug(dcZigbeeNode()) << "Active endpoints request finished" << this << adpu;
                QDataStream stream(adpu.payload);
                stream.setByteOrder(QDataStream::LittleEndian);
                quint8 endpointCount = 0;
                m_uninitializedEndpoints.clear();
                stream >> endpointCount;
                for (int i = 0; i < endpointCount; i++) {
                    quint8 endpoint = 0;
                    stream >> endpoint;
                    m_uninitializedEndpoints.append(endpoint);
                }

                qCDebug(dcZigbeeNode()) << "Endpoints" << endpointCount;
                for (int i = 0; i < m_uninitializedEndpoints.count(); i++) {
                    qCDebug(dcZigbeeNode()) << " -" << ZigbeeUtils::convertByteToHexString(m_uninitializedEndpoints.at(i));
                }

                // Read simple descriptor for each endpoint
                if (m_uninitializedEndpoints.isEmpty()) {
                    setState(StateInitialized);
                }

                for (int i = 0; i < m_uninitializedEndpoints.count(); i++) {
                    quint8 endpointId = m_uninitializedEndpoints.at(i);
                    qCDebug(dcZigbeeNode()) << "Read simple descriptor of endpoint" << ZigbeeUtils::convertByteToHexString(endpointId);
                    ZigbeeNetworkReply *reply = requestSimpleDescriptor(endpointId);
                    connect(reply, &ZigbeeNetworkReply::finished, this, [this, reply, endpointId](){
                        // TODO: check reply error
                        ZigbeeDeviceProfileAdpu adpu = ZigbeeDeviceProfile::parseAdpu(reply->responseData());
                        qCDebug(dcZigbeeNode()) << "Simple descriptor request finished" << this << endpointId << adpu;

                        QDataStream stream(adpu.payload);
                        stream.setByteOrder(QDataStream::LittleEndian);
                        quint8 length = 0;
                        quint8 endpoint = 0;
                        quint16 profileId = 0;
                        quint16 deviceId = 0;
                        quint8 deviceVersion = 0;
                        quint8 inputClusterCount = 0;
                        quint8 outputClusterCount = 0;

                        QList<quint16> inputClusters;
                        QList<quint16> outputClusters;

                        stream >> length >> endpoint >> profileId >> deviceId >> deviceVersion >> inputClusterCount;

                        qCDebug(dcZigbeeNode()) << "Node endpoint simple descriptor:";
                        qCDebug(dcZigbeeNode()) << "    Lenght:" << ZigbeeUtils::convertByteToHexString(length);
                        qCDebug(dcZigbeeNode()) << "    End Point:" << ZigbeeUtils::convertByteToHexString(endpoint);
                        qCDebug(dcZigbeeNode()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));
                        if (profileId == Zigbee::ZigbeeProfileLightLink) {
                            qCDebug(dcZigbeeNode()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::LightLinkDevice>(deviceId);
                        } else if (profileId == Zigbee::ZigbeeProfileHomeAutomation) {
                            qCDebug(dcZigbeeNode()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::HomeAutomationDevice>(deviceId);
                        } else if (profileId == Zigbee::ZigbeeProfileGreenPower) {
                            qCDebug(dcZigbeeNode()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::GreenPowerDevice>(deviceId);
                        }

                        qCDebug(dcZigbeeNode()) << "    Device version:" << ZigbeeUtils::convertByteToHexString(deviceVersion);


                        qCDebug(dcZigbeeNode()) << "    Input clusters: (" << inputClusterCount << ")";

                        for (int i = 0; i < inputClusterCount; i++) {
                            quint16 clusterId = 0;
                            stream >> clusterId;
                            inputClusters.append(clusterId);
                            qCDebug(dcZigbeeNode()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));

                        }
                        stream >> outputClusterCount;

                        qCDebug(dcZigbeeNode()) << "    Output clusters: (" << outputClusterCount << ")";
                        for (int i = 0; i < outputClusterCount; i++) {
                            quint16 clusterId = 0;
                            stream >> clusterId;
                            outputClusters.append(clusterId);
                            qCDebug(dcZigbeeNode()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
                        }

                        m_uninitializedEndpoints.removeAll(endpointId);

                        // Create endpoint


                        if (m_uninitializedEndpoints.isEmpty()) {
                            setState(StateInitialized);
                        }
                    });
                }
            });
        });
    });
}

ZigbeeNodeEndpoint *ZigbeeNodeDeconz::createNodeEndpoint(quint8 endpointId, QObject *parent)
{
    Q_UNUSED(endpointId)
    Q_UNUSED(parent)
    return nullptr;
}
