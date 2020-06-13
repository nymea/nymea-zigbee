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

#include "zigbeenode.h"
#include "zigbeeutils.h"
#include "zigbeenetwork.h"
#include "loggingcategory.h"

#include <QDataStream>

ZigbeeNode::ZigbeeNode(ZigbeeNetwork *network, quint16 shortAddress, const ZigbeeAddress &extendedAddress, QObject *parent) :
    QObject(parent),
    m_network(network),
    m_shortAddress(shortAddress),
    m_extendedAddress(extendedAddress)
{
    m_deviceObject = new ZigbeeDeviceObject(m_network, this, this);
}

ZigbeeNode::State ZigbeeNode::state() const
{
    return m_state;
}

bool ZigbeeNode::connected() const
{
    return m_connected;
}

ZigbeeDeviceObject *ZigbeeNode::deviceObject() const
{
    return m_deviceObject;
}

quint16 ZigbeeNode::shortAddress() const
{
    return m_shortAddress;
}

ZigbeeAddress ZigbeeNode::extendedAddress() const
{
    return m_extendedAddress;
}

QList<ZigbeeNodeEndpoint *> ZigbeeNode::endpoints() const
{
    return m_endpoints;
}

bool ZigbeeNode::hasEndpoint(quint8 endpointId) const
{
    return getEndpoint(endpointId) != nullptr;
}

ZigbeeNodeEndpoint *ZigbeeNode::getEndpoint(quint8 endpointId) const
{
    foreach (ZigbeeNodeEndpoint *ep, m_endpoints) {
        if (ep->endpointId()== endpointId) {
            return ep;
        }
    }

    return nullptr;
}

ZigbeeDeviceProfile::NodeDescriptor ZigbeeNode::nodeDescriptor() const
{
    return m_nodeDescriptor;
}

ZigbeeDeviceProfile::MacCapabilities ZigbeeNode::macCapabilities() const
{
    return m_macCapabilities;
}

ZigbeeDeviceProfile::PowerDescriptor ZigbeeNode::powerDescriptor() const
{
    return m_powerDescriptor;
}

void ZigbeeNode::setState(ZigbeeNode::State state)
{
    if (m_state == state)
        return;

    qCDebug(dcZigbeeNode()) << "State changed" << this << state;
    m_state = state;
    emit stateChanged(m_state);
}

void ZigbeeNode::setConnected(bool connected)
{
    if (m_connected == connected)
        return;

    qCDebug(dcZigbeeNode()) << "Connected changed"  << this << connected;
    m_connected = connected;
    emit connectedChanged(m_connected);
}

void ZigbeeNode::startInitialization()
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

    initNodeDescriptor();
}

void ZigbeeNode::initNodeDescriptor()
{
    qCDebug(dcZigbeeNode()) << "Requst node descriptor from" << this;
    ZigbeeDeviceObjectReply *reply = deviceObject()->requestNodeDescriptor();
    connect(reply, &ZigbeeDeviceObjectReply::finished, this, [this, reply](){
        if (reply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read node descriptor" << reply->error();
            m_requestRetry++;
            if (m_requestRetry < 3) {
                qCDebug(dcZigbeeNode()) << "Retry to request node descriptor" << m_requestRetry << "/" << "3";
                initNodeDescriptor();
            } else {
                qCWarning(dcZigbeeNode()) << "Failed to read node descriptor from" << this << "after 3 attempts. Giving up.";
                m_requestRetry = 0;
                emit nodeInitializationFailed();
            }
            return;
        }

        // The request finished, but we received a ZDP error.
        if (reply->responseAdpu().status != ZigbeeDeviceProfile::StatusSuccess) {
            qCWarning(dcZigbeeNode()) << this << "failed to read node descriptor" << reply->responseAdpu().status;
            // FIXME: decide what to do, remove the node again from network
            return;
        }

        qCDebug(dcZigbeeNode()) << this << "reading node descriptor finished successfully.";
        m_nodeDescriptor = ZigbeeDeviceProfile::parseNodeDescriptor(reply->responseAdpu().payload);
        qCDebug(dcZigbeeNode()) << m_nodeDescriptor;
        m_requestRetry = 0;

        // Continue with the power descriptor
        initPowerDescriptor();
    });
}

void ZigbeeNode::initPowerDescriptor()
{
    ZigbeeDeviceObjectReply *reply = deviceObject()->requestPowerDescriptor();
    connect(reply, &ZigbeeDeviceObjectReply::finished, this, [this, reply](){
        if (reply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read power descriptor" << reply->error();
            if (m_requestRetry < 3) {
                m_requestRetry++;
                qCDebug(dcZigbeeNode()) << "Retry to request power descriptor from" << this << m_requestRetry << "/" << "3 attempts.";
                initPowerDescriptor();
            } else {
                qCWarning(dcZigbeeNode()) << "Failed to read power descriptor from" << this << "after 3 attempts. Giving up.";
                m_requestRetry = 0;
                emit nodeInitializationFailed();
            }
            return;
        }

        ZigbeeDeviceProfile::Adpu adpu = reply->responseAdpu();
        if (adpu.status != ZigbeeDeviceProfile::StatusSuccess) {
            qCWarning(dcZigbeeNode()) << "Failed to read power descriptor from" << this << adpu.status;
            // FIXME: decide what to do, remove the node again from network or continue without powerdescriptor
            return;
        }

        qCDebug(dcZigbeeNode()) << this << "reading power descriptor finished successfully.";
        m_requestRetry = 0;

        QDataStream stream(adpu.payload);
        stream.setByteOrder(QDataStream::LittleEndian);
        quint16 powerDescriptorFlag = 0;
        stream >> powerDescriptorFlag;
        m_powerDescriptor = ZigbeeDeviceProfile::parsePowerDescriptor(powerDescriptorFlag);
        qCDebug(dcZigbeeNode()) << m_powerDescriptor;

        // Continue with endpoint fetching
        initEndpoints();
    });
}

void ZigbeeNode::initEndpoints()
{
    ZigbeeDeviceObjectReply *reply = deviceObject()->requestActiveEndpoints();
    connect(reply, &ZigbeeDeviceObjectReply::finished, this, [this, reply](){
        if (reply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read active endpoints" << reply->error();
            if (m_requestRetry < 3) {
                m_requestRetry++;
                qCDebug(dcZigbeeNode()) << "Retry to request active endpoints from" << this << m_requestRetry << "/" << "3 attempts.";
                initEndpoints();
            } else {
                qCWarning(dcZigbeeNode()) << "Failed to read active endpoints from" << this << "after 3 attempts. Giving up.";
                m_requestRetry = 0;
                emit nodeInitializationFailed();
            }
            return;
        }

        if (reply->responseAdpu().status != ZigbeeDeviceProfile::StatusSuccess) {
            qCWarning(dcZigbeeNode()) << "Failed to read active endpoints" << reply->responseAdpu().status;
            // FIXME: decide what to do, retry or stop initialization
            return;
        }

        qCDebug(dcZigbeeNode()) << this << "reading active endpoints finished successfully.";
        m_requestRetry = 0;

        QDataStream stream(reply->responseAdpu().payload);
        stream.setByteOrder(QDataStream::LittleEndian);
        quint8 endpointCount = 0;
        m_uninitializedEndpoints.clear();
        stream >> endpointCount;
        for (int i = 0; i < endpointCount; i++) {
            quint8 endpoint = 0;
            stream >> endpoint;
            m_uninitializedEndpoints.append(endpoint);
        }

        qCDebug(dcZigbeeNode()) << "Endpoints (" << endpointCount << ")";
        for (int i = 0; i < m_uninitializedEndpoints.count(); i++) {
            qCDebug(dcZigbeeNode()) << " -" << ZigbeeUtils::convertByteToHexString(m_uninitializedEndpoints.at(i));
        }

        // If there a no endpoints or all endpoints have already be initialized, continue with reading the basic cluster information
        if (m_uninitializedEndpoints.isEmpty()) {
            initBasicCluster();
            return;
        }

        // Start reading simple descriptors sequentially
        initEndpoint(m_uninitializedEndpoints.first());
    });
}


void ZigbeeNode::initEndpoint(quint8 endpointId)
{
    qCDebug(dcZigbeeNode()) << "Read simple descriptor of endpoint" << ZigbeeUtils::convertByteToHexString(endpointId);
    ZigbeeDeviceObjectReply *reply = deviceObject()->requestSimpleDescriptor(endpointId);
    connect(reply, &ZigbeeDeviceObjectReply::finished, this, [this, reply, endpointId](){
        if (reply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read simple descriptor for endpoint" << endpointId << reply->error();
            if (m_requestRetry < 3) {
                m_requestRetry++;
                qCDebug(dcZigbeeNode()) << "Retry to request simple descriptor from" << this << ZigbeeUtils::convertByteToHexString(endpointId) << m_requestRetry << "/" << "3 attempts.";
                initEndpoint(endpointId);
            } else {
                qCWarning(dcZigbeeNode()) << "Failed to read simple descriptor from" << this << ZigbeeUtils::convertByteToHexString(endpointId) << "after 3 attempts. Giving up.";
                m_requestRetry = 0;
                emit nodeInitializationFailed();
            }
            return;
        }

        if (reply->responseAdpu().status != ZigbeeDeviceProfile::StatusSuccess) {
            qCWarning(dcZigbeeNode()) << this << "failed to read simple descriptor from endpoint" << endpointId << reply->responseAdpu().status;
            // FIXME: decide what to do, retry or stop initialization
            return;
        }

        qCDebug(dcZigbeeNode()) << this << "reading simple descriptor for endpoint" << endpointId << "finished successfully.";
        m_requestRetry = 0;

        quint8 length = 0; quint8 endpointId = 0; quint16 profileId = 0; quint16 deviceId = 0; quint8 deviceVersion = 0;
        quint8 inputClusterCount = 0; quint8 outputClusterCount = 0;
        QList<quint16> inputClusters;
        QList<quint16> outputClusters;

        QDataStream stream(reply->responseAdpu().payload);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> length >> endpointId >> profileId >> deviceId >> deviceVersion >> inputClusterCount;

        qCDebug(dcZigbeeNode()) << "Node endpoint simple descriptor:";
        qCDebug(dcZigbeeNode()) << "    Lenght:" << ZigbeeUtils::convertByteToHexString(length);
        qCDebug(dcZigbeeNode()) << "    End Point:" << ZigbeeUtils::convertByteToHexString(endpointId);
        qCDebug(dcZigbeeNode()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));
        if (profileId == Zigbee::ZigbeeProfileLightLink) {
            qCDebug(dcZigbeeNode()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::LightLinkDevice>(deviceId);
        } else if (profileId == Zigbee::ZigbeeProfileHomeAutomation) {
            qCDebug(dcZigbeeNode()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::HomeAutomationDevice>(deviceId);
        } else if (profileId == Zigbee::ZigbeeProfileGreenPower) {
            qCDebug(dcZigbeeNode()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::GreenPowerDevice>(deviceId);
        }

        qCDebug(dcZigbeeNode()) << "    Device version:" << ZigbeeUtils::convertByteToHexString(deviceVersion);

        // Create endpoint
        ZigbeeNodeEndpoint *endpoint = nullptr;
        if (!hasEndpoint(endpointId)) {
            endpoint = new ZigbeeNodeEndpoint(m_network, this, endpointId, this);
            m_endpoints.append(endpoint);
        } else {
            endpoint = getEndpoint(endpointId);
        }
        endpoint->setProfile(static_cast<Zigbee::ZigbeeProfile>(profileId));
        endpoint->setDeviceId(deviceId);
        endpoint->setDeviceVersion(deviceVersion);

        // Parse and add server clusters
        qCDebug(dcZigbeeNode()) << "    Input clusters: (" << inputClusterCount << ")";
        for (int i = 0; i < inputClusterCount; i++) {
            quint16 clusterId = 0;
            stream >> clusterId;
            if (!endpoint->hasInputCluster(static_cast<ZigbeeClusterLibrary::ClusterId>(clusterId))) {
                endpoint->addInputCluster(endpoint->createCluster(static_cast<ZigbeeClusterLibrary::ClusterId>(clusterId), ZigbeeCluster::Server));
            }
            qCDebug(dcZigbeeNode()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<ZigbeeClusterLibrary::ClusterId>(clusterId));
        }

        // Parse and add client clusters
        stream >> outputClusterCount;
        qCDebug(dcZigbeeNode()) << "    Output clusters: (" << outputClusterCount << ")";
        for (int i = 0; i < outputClusterCount; i++) {
            quint16 clusterId = 0;
            stream >> clusterId;
            if (!endpoint->hasOutputCluster(static_cast<ZigbeeClusterLibrary::ClusterId>(clusterId))) {
                endpoint->addOutputCluster(endpoint->createCluster(static_cast<ZigbeeClusterLibrary::ClusterId>(clusterId), ZigbeeCluster::Client));
            }
            qCDebug(dcZigbeeNode()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<ZigbeeClusterLibrary::ClusterId>(clusterId));
        }

        m_uninitializedEndpoints.removeAll(endpointId);
        endpoint->m_initialized = true;

        if (m_uninitializedEndpoints.isEmpty()) {
            // Note: if we are initializing the coordinator, we can stop here
            if (m_shortAddress == 0) {
                setState(StateInitialized);
                return;
            }

            // Continue with the basic cluster attributes
            initBasicCluster();
        } else {
            // Fetch next endpoint
            initEndpoint(m_uninitializedEndpoints.first());
        }
    });
}

void ZigbeeNode::initBasicCluster()
{
    // Get the first endpoint which implements the basic cluster
    ZigbeeNodeEndpoint *endpoint = nullptr;
    foreach (ZigbeeNodeEndpoint *ep, endpoints()) {
        if (ep->hasInputCluster(ZigbeeClusterLibrary::ClusterIdBasic)) {
            endpoint = ep;
            break;
        }
    }

    if (!endpoint) {
        qCWarning(dcZigbeeNode()) << "Could not find any endpoint contiaining the basic cluster on" << this << "Set the node to initialized anyways.";
        setState(StateInitialized);
        return;
    }

    ZigbeeClusterBasic *basicCluster = endpoint->inputCluster<ZigbeeClusterBasic>(ZigbeeClusterLibrary::ClusterIdBasic);
    if (!basicCluster) {
        qCWarning(dcZigbeeNode()) << "Could not find basic cluster on" << this << "Set the node to initialized anyways.";
        // Set the device initialized any ways since this ist just for convinience
        setState(StateInitialized);
        return;
    }

    // Start reading basic cluster attributes sequentially
    readManufacturerName(basicCluster);
}

void ZigbeeNode::readManufacturerName(ZigbeeClusterBasic *basicCluster)
{
    ZigbeeClusterBasic::Attribute attributeId = ZigbeeClusterBasic::AttributeManufacturerName;
    if (basicCluster->hasAttribute(attributeId)) {
        // Note: only read the basic cluster information if we don't have them already from an indication.
        // Some devices (Lumi/Aquara) send cluster information containing different payload than a read attribute returns.
        // This is bad device stack implementation, but we want to make it work either way without destroying the correct
        // workflow as specified by the stack.
        qCDebug(dcZigbeeNode()) << "The manufacturer name has already been set" << this << "Continue with model identifier";
        bool valueOk = false;
        QString manufacturerName = basicCluster->attribute(attributeId).dataType().toString(&valueOk);
        if (valueOk) {
            endpoints().first()->m_manufacturerName = manufacturerName;
        } else {
            qCWarning(dcZigbeeNode()) << "Could not convert manufacturer name attribute data to string" << basicCluster->attribute(attributeId).dataType();
        }

        readModelIdentifier(basicCluster);
        return;
    }

    qCDebug(dcZigbeeNode()) << "Reading attribute" << attributeId;
    ZigbeeClusterReply *reply = basicCluster->readAttributes({static_cast<quint16>(attributeId)});
    connect(reply, &ZigbeeClusterReply::finished, this, [this, basicCluster, reply, attributeId](){
        if (reply->error() != ZigbeeClusterReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read basic cluster attribute" << attributeId << reply->error();
        } else {
            qCDebug(dcZigbeeNode()) << "Reading basic cluster attributes finished successfully";
            QList<ZigbeeClusterLibrary::ReadAttributeStatusRecord> attributeStatusRecords = ZigbeeClusterLibrary::parseAttributeStatusRecords(reply->responseFrame().payload);
            if (!attributeStatusRecords.isEmpty()) {
                ZigbeeClusterLibrary::ReadAttributeStatusRecord attributeStatusRecord = attributeStatusRecords.first();
                qCDebug(dcZigbeeNode()) << attributeStatusRecord;
                basicCluster->setAttribute(ZigbeeClusterAttribute(static_cast<quint16>(attributeId), attributeStatusRecord.dataType));
                bool valueOk = false;
                QString manufacturerName = attributeStatusRecord.dataType.toString(&valueOk);
                if (valueOk) {
                    endpoints().first()->m_manufacturerName = manufacturerName;
                } else {
                    qCWarning(dcZigbeeNode()) << "Could not convert manufacturer name attribute data to string" << attributeStatusRecord.dataType;
                }
            }
        }

        // Continue eiterh way with attribute reading
        readModelIdentifier(basicCluster);
    });
}

void ZigbeeNode::readModelIdentifier(ZigbeeClusterBasic *basicCluster)
{
    ZigbeeClusterBasic::Attribute attributeId = ZigbeeClusterBasic::AttributeModelIdentifier;
    if (basicCluster->hasAttribute(attributeId)) {
        // Note: only read the basic cluster information if we don't have them already from an indication.
        // Some devices (Lumi/Aquara) send cluster information containing different payload than a read attribute returns.
        // This is bad device stack implementation, but we want to make it work either way without destroying the correct
        // workflow as specified by the stack.
        qCDebug(dcZigbeeNode()) << "The model identifier has already been set" << this << "Continue with software build ID.";
        bool valueOk = false;
        QString modelIdentifier = basicCluster->attribute(attributeId).dataType().toString(&valueOk);
        if (valueOk) {
            endpoints().first()->m_modelIdentifier= modelIdentifier;
        } else {
            qCWarning(dcZigbeeNode()) << "Could not convert model identifier attribute data to string" << basicCluster->attribute(attributeId).dataType();
        }

        readSoftwareBuildId(basicCluster);
        return;
    }

    qCDebug(dcZigbeeNode()) << "Reading attribute" << attributeId;
    ZigbeeClusterReply *reply = basicCluster->readAttributes({static_cast<quint16>(attributeId)});
    connect(reply, &ZigbeeClusterReply::finished, this, [this, basicCluster, reply, attributeId](){
        if (reply->error() != ZigbeeClusterReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read basic cluster attribute" << attributeId << reply->error();
        } else {
            qCDebug(dcZigbeeNode()) << "Reading basic cluster attributes finished successfully";
            QList<ZigbeeClusterLibrary::ReadAttributeStatusRecord> attributeStatusRecords = ZigbeeClusterLibrary::parseAttributeStatusRecords(reply->responseFrame().payload);
            if (!attributeStatusRecords.isEmpty()) {
                ZigbeeClusterLibrary::ReadAttributeStatusRecord attributeStatusRecord = attributeStatusRecords.first();
                qCDebug(dcZigbeeNode()) << attributeStatusRecord;
                basicCluster->setAttribute(ZigbeeClusterAttribute(static_cast<quint16>(attributeId), attributeStatusRecord.dataType));
                bool valueOk = false;
                QString modelIdentifier = attributeStatusRecord.dataType.toString(&valueOk);
                if (valueOk) {
                    endpoints().first()->m_modelIdentifier = modelIdentifier;
                } else {
                    qCWarning(dcZigbeeNode()) << "Could not convert model identifier attribute data to string" << attributeStatusRecord.dataType;
                }
            }
        }

        // Continue eiterh way with attribute reading
        readSoftwareBuildId(basicCluster);
    });
}

void ZigbeeNode::readSoftwareBuildId(ZigbeeClusterBasic *basicCluster)
{
    ZigbeeClusterBasic::Attribute attributeId = ZigbeeClusterBasic::AttributeSwBuildId;
    qCDebug(dcZigbeeNode()) << "Reading attribute" << attributeId;
    ZigbeeClusterReply *reply = basicCluster->readAttributes({static_cast<quint16>(attributeId)});
    connect(reply, &ZigbeeClusterReply::finished, this, [this, basicCluster, reply, attributeId](){
        if (reply->error() != ZigbeeClusterReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read basic cluster attribute" << attributeId << reply->error();
        } else {
            qCDebug(dcZigbeeNode()) << "Reading basic cluster attributes finished successfully";
            QList<ZigbeeClusterLibrary::ReadAttributeStatusRecord> attributeStatusRecords = ZigbeeClusterLibrary::parseAttributeStatusRecords(reply->responseFrame().payload);
            if (!attributeStatusRecords.isEmpty()) {
                ZigbeeClusterLibrary::ReadAttributeStatusRecord attributeStatusRecord = attributeStatusRecords.first();
                qCDebug(dcZigbeeNode()) << attributeStatusRecord;
                basicCluster->setAttribute(ZigbeeClusterAttribute(static_cast<quint16>(attributeId), attributeStatusRecord.dataType));
                bool valueOk = false;
                QString softwareBuildId = attributeStatusRecord.dataType.toString(&valueOk);
                if (valueOk) {
                    endpoints().first()->m_softwareBuildId = softwareBuildId;
                } else {
                    qCWarning(dcZigbeeNode()) << "Could not convert software build id attribute data to string" << attributeStatusRecord.dataType;
                }
            }
        }

        // Finished with reading basic cluster, the node is initialized.
        // TODO: read other interesting cluster information


        // Bind client clusters to the sensor group
        // Configure reporting

        setState(StateInitialized);
    });
}

void ZigbeeNode::handleZigbeeClusterLibraryIndication(const Zigbee::ApsdeDataIndication &indication)
{
    qCDebug(dcZigbeeNode()) << "Processing ZCL indication" << indication;

    // Get the endpoint
    ZigbeeNodeEndpoint *endpoint = getEndpoint(indication.sourceEndpoint);
    if (!endpoint) {
        qCWarning(dcZigbeeNetwork()) << "Received a ZCL indication for an unrecognized endpoint. There is no such endpoint on" << this << "Creating the uninitialized endpoint";
        // Create the uninitialized endpoint for now and fetch information later
        endpoint = new ZigbeeNodeEndpoint(m_network, this, indication.sourceEndpoint, this);
        endpoint->setProfile(static_cast<Zigbee::ZigbeeProfile>(indication.profileId));
        // Note: the endpoint is not initializd yet, but keep it anyways
        m_endpoints.append(endpoint);
    }

    endpoint->handleZigbeeClusterLibraryIndication(indication);
}

QDebug operator<<(QDebug debug, ZigbeeNode *node)
{
    debug.nospace().noquote() << "ZigbeeNode(" << ZigbeeUtils::convertUint16ToHexString(node->shortAddress());
    debug.nospace().noquote() << ", " << node->extendedAddress().toString();
    debug.nospace().noquote() << ")";
    return debug.space();
}
