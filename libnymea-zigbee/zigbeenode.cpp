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

bool ZigbeeNode::reachable() const
{
    return m_reachable;
}

QUuid ZigbeeNode::networkUuid() const
{
    return m_network->networkUuid();
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

QString ZigbeeNode::manufacturerName() const
{
    return m_manufacturerName;
}

QString ZigbeeNode::modelName() const
{
    return m_modelName;
}

QString ZigbeeNode::version() const
{
    return m_version;
}

quint8 ZigbeeNode::lqi() const
{
    return m_lqi;
}

QDateTime ZigbeeNode::lastSeen() const
{
    return m_lastSeen;
}

ZigbeeDeviceProfile::NodeDescriptor ZigbeeNode::nodeDescriptor() const
{
    return m_nodeDescriptor;
}

bool ZigbeeNode::nodeDescriptorAvailable() const
{
    return m_nodeDescriptorAvailable;
}

ZigbeeDeviceProfile::MacCapabilities ZigbeeNode::macCapabilities() const
{
    return m_macCapabilities;
}

ZigbeeDeviceProfile::PowerDescriptor ZigbeeNode::powerDescriptor() const
{
    return m_powerDescriptor;
}

bool ZigbeeNode::powerDescriptorAvailable() const
{
    return m_powerDescriptorAvailable;
}

QList<ZigbeeDeviceProfile::BindingTableListRecord> ZigbeeNode::bindingTableRecords() const
{
    return m_bindingTableRecords;
}

QList<ZigbeeDeviceProfile::NeighborTableListRecord> ZigbeeNode::neighborTableRecords() const
{
    return m_neighborTableRecords;
}

void ZigbeeNode::setState(ZigbeeNode::State state)
{
    if (m_state == state)
        return;

    qCDebug(dcZigbeeNode()) << "State changed" << this << state;
    m_state = state;
    emit stateChanged(m_state);
}

void ZigbeeNode::setReachable(bool reachable)
{
    if (m_reachable == reachable)
        return;

    if (!reachable) {
        qCWarning(dcZigbeeNode()) << "Reachable changed"  << this << reachable;
    } else {
        qCDebug(dcZigbeeNode()) << "Reachable changed"  << this << reachable;
    }

    m_reachable = reachable;
    emit reachableChanged(m_reachable);
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

ZigbeeReply *ZigbeeNode::removeAllBindings()
{
    ZigbeeReply *reply = new ZigbeeReply(this);

    ZigbeeReply *readBindingsReply = readBindingTableEntries();
    connect(readBindingsReply, &ZigbeeReply::finished, reply, [=](){
        if (readBindingsReply->error()) {
            qCWarning(dcZigbeeNode()) << "Failed to remove all bindings because the current bindings could not be fetched from node" << this;
            reply->finishReply(readBindingsReply->error());
            return;
        }

        qCDebug(dcZigbeeNode()) << "Current binding table records:";
        foreach (const ZigbeeDeviceProfile::BindingTableListRecord &binding, m_bindingTableRecords) {
            qCDebug(dcZigbeeNode()) << binding;
        }

        // Remove bindings sequentially and finish reply if error occures or all bindings removed
        removeNextBinding(reply);
    });
    return reply;
}

ZigbeeReply *ZigbeeNode::readBindingTableEntries()
{
    ZigbeeReply *reply = new ZigbeeReply(this);
    ZigbeeDeviceObjectReply *zdoReply = deviceObject()->requestMgmtBind();
    connect(zdoReply, &ZigbeeDeviceObjectReply::finished, this, [=](){
        if (zdoReply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Failed to read binding table" << zdoReply->error();
            reply->finishReply(ZigbeeReply::ErrorZigbeeError);
            return;
        }

        qCDebug(dcZigbeeDeviceObject()) << "Bind table payload" << ZigbeeUtils::convertByteArrayToHexString(zdoReply->responseData());
        QByteArray response = zdoReply->responseData();
        QDataStream stream(&response, QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        quint8 sqn; quint8 statusInt; quint8 entriesCount; quint8 startIndex; quint8 bindingTableListCount;
        stream >> sqn >> statusInt >> entriesCount >> startIndex >> bindingTableListCount;
        ZigbeeDeviceProfile::Status status = static_cast<ZigbeeDeviceProfile::Status>(statusInt);
        qCDebug(dcZigbeeDeviceObject()) << "SQN:" << sqn << status << "entries:" << entriesCount << "index:" << startIndex << "list count:" << bindingTableListCount;

        m_bindingTableRecords.clear();
        for (int i = 0; i < bindingTableListCount; i++) {
            quint64 sourceAddress; quint8 addressMode;
            ZigbeeDeviceProfile::BindingTableListRecord record;
            stream >> sourceAddress;
            record.sourceAddress = ZigbeeAddress(sourceAddress);

            stream >> record.sourceEndpoint >> record.clusterId >> addressMode;
            record.destinationAddressMode = static_cast<Zigbee::DestinationAddressMode>(addressMode);

            if (addressMode == Zigbee::DestinationAddressModeGroup) {
                stream >> record.destinationAddressShort;
            } else if (addressMode == Zigbee::DestinationAddressModeIeeeAddress) {
                quint64 destinationAddressIeee;
                stream >> destinationAddressIeee >> record.destinationEndpoint;
                record.destinationAddress = ZigbeeAddress(destinationAddressIeee);
            } else {
                qCWarning(dcZigbeeDeviceObject()) << "Invalid destination address mode in binding table record.";
                break;
            }
            qCDebug(dcZigbeeDeviceObject()) << record;
            m_bindingTableRecords << record;
        }

        // TODO: continue reading if there are more entries

        emit bindingTableRecordsChanged();
        reply->finishReply();
    });
    return reply;
}

ZigbeeReply *ZigbeeNode::updateNeighborTableEntries()
{
    // Start clean
    m_temporaryNeighborTableRecords.clear();

    qCDebug(dcZigbeeNode()) << "Start updating neighbor table records from" << this;
    ZigbeeReply *reply = new ZigbeeReply(this);
    readNextNeigborTableRecords(reply);
    return reply;
}

void ZigbeeNode::initNodeDescriptor()
{
    qCDebug(dcZigbeeNode()) << "Request node descriptor from" << this;
    ZigbeeDeviceObjectReply *reply = deviceObject()->requestNodeDescriptor();
    connect(reply, &ZigbeeDeviceObjectReply::finished, this, [this, reply](){
        if (reply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read node descriptor" << reply->error();
            m_requestRetry++;
            if (m_requestRetry < m_requestRetriesMax) {
                qCDebug(dcZigbeeNode()) << "Retry to request node descriptor" << m_requestRetry << "/" << m_requestRetriesMax;
                QTimer::singleShot(500, this, [=](){ initNodeDescriptor(); });
            } else {
                qCWarning(dcZigbeeNode()) << "Failed to read node descriptor from" << this << "after" << m_requestRetriesMax << "attempts.";
                m_requestRetry = 0;
                qCWarning(dcZigbeeNode()) << this << "is out of spec. A device must implement the node descriptor. Continue anyways with the power decriptor...";
                initPowerDescriptor();
            }
            return;
        }

        qCDebug(dcZigbeeNode()) << this << "reading node descriptor finished successfully.";
        m_nodeDescriptor = ZigbeeDeviceProfile::parseNodeDescriptor(reply->responseAdpu().payload);
        qCDebug(dcZigbeeNode()) << m_nodeDescriptor;
        m_nodeDescriptorAvailable = true;
        m_requestRetry = 0;

        // Continue with the power descriptor
        initPowerDescriptor();
    });
}

void ZigbeeNode::initPowerDescriptor()
{
    qCDebug(dcZigbeeNode()) << "Request power descriptor from" << this;
    ZigbeeDeviceObjectReply *reply = deviceObject()->requestPowerDescriptor();
    connect(reply, &ZigbeeDeviceObjectReply::finished, this, [this, reply](){
        if (reply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read power descriptor" << reply->error();
            if (m_requestRetry < m_requestRetriesMax) {
                m_requestRetry++;
                qCDebug(dcZigbeeNode()) << "Retry to request power descriptor from" << this << m_requestRetry << "/" << m_requestRetriesMax << "attempts.";
                QTimer::singleShot(500, this, [=](){ initPowerDescriptor(); });
            } else {
                qCWarning(dcZigbeeNode()) << "Failed to read power descriptor from" << this << "after" << m_requestRetriesMax << "attempts. Giving up reading power descriptor.";
                qCWarning(dcZigbeeNode()) << this << "is out of spec. A device must implement the power descriptor. Continue anyways with the endpoint initialization...";
                initEndpoints();
            }
            return;
        }

        qCDebug(dcZigbeeNode()) << this << "reading power descriptor finished successfully.";
        QDataStream stream(reply->responseAdpu().payload);
        stream.setByteOrder(QDataStream::LittleEndian);
        quint16 powerDescriptorFlag = 0;
        stream >> powerDescriptorFlag;
        m_powerDescriptor = ZigbeeDeviceProfile::parsePowerDescriptor(powerDescriptorFlag);
        qCDebug(dcZigbeeNode()) << m_powerDescriptor;
        m_powerDescriptorAvailable = true;
        m_requestRetry = 0;

        // Continue with endpoint fetching
        initEndpoints();
    });
}

void ZigbeeNode::initEndpoints()
{
    qCDebug(dcZigbeeNode()) << "Request active endpoints from" << this;
    ZigbeeDeviceObjectReply *reply = deviceObject()->requestActiveEndpoints();
    connect(reply, &ZigbeeDeviceObjectReply::finished, this, [this, reply](){
        if (reply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Error occured during initialization of" << this << "Failed to read active endpoints" << reply->error();
            if (m_requestRetry < m_requestRetriesMax) {
                m_requestRetry++;
                qCDebug(dcZigbeeNode()) << "Retry to request active endpoints from" << this << m_requestRetry << "/" << m_requestRetriesMax << "attempts.";
                QTimer::singleShot(500, this, [=](){ initEndpoints(); });
            } else {
                qCWarning(dcZigbeeNode()) << "Failed to read active endpoints from" << this << "after" << m_requestRetriesMax << "attempts. Giving up reading endpoints.";
                m_requestRetry = 0;
                setState(StateInitialized);
            }
            return;
        }

        qCDebug(dcZigbeeNode()) << this << "reading active endpoints finished successfully.";
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

        m_requestRetry = 0;

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
            if (m_requestRetry < m_requestRetriesMax) {
                m_requestRetry++;
                qCDebug(dcZigbeeNode()) << "Retry to request simple descriptor from" << this << ZigbeeUtils::convertByteToHexString(endpointId) << m_requestRetry << "/" << m_requestRetriesMax << "attempts.";
                QTimer::singleShot(500, this, [=](){ initEndpoint(endpointId); });
            } else {
                qCWarning(dcZigbeeNode()) << "Failed to read simple descriptor from" << this << ZigbeeUtils::convertByteToHexString(endpointId) << "after" << m_requestRetriesMax << "attempts. Giving up initializing endpoint" << endpointId;
                m_requestRetry = 0;
                if (m_uninitializedEndpoints.isEmpty()) {
                    // Continue with the basic cluster attributes
                    initBasicCluster();
                } else {
                    // Fetch next endpoint
                    initEndpoint(m_uninitializedEndpoints.first());
                }
            }
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

        setupEndpointInternal(endpoint);

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

void ZigbeeNode::removeNextBinding(ZigbeeReply *reply)
{
    // If we have no bindings left, finish the given reply
    if (m_bindingTableRecords.isEmpty()) {
        reply->finishReply();
        return;
    }

    ZigbeeDeviceProfile::BindingTableListRecord record = m_bindingTableRecords.last();
    ZigbeeDeviceObjectReply *zdoReply = m_deviceObject->requestUnbind(record);
    connect(zdoReply, &ZigbeeDeviceObjectReply::finished, reply, [=](){
        if (zdoReply->error() != ZigbeeDeviceObjectReply::ErrorNoError) {
            qCWarning(dcZigbeeNode()) << "Failed to remove" << record << zdoReply->error();
            reply->finishReply(ZigbeeReply::ErrorZigbeeError);
            return;
        }

        // Successfully removed
        m_bindingTableRecords.removeLast();
        emit bindingTableRecordsChanged();

        removeNextBinding(reply);
    });
}

void ZigbeeNode::readNextNeigborTableRecords(ZigbeeReply *reply, quint8 startIndex)
{
    ZigbeeDeviceObjectReply *zdoReply = m_deviceObject->requestMgmtLqi(startIndex);
    connect(zdoReply, &ZigbeeDeviceObjectReply::finished, this, [this, zdoReply, reply](){
        if (zdoReply->error()) {
            qCWarning(dcZigbeeDeviceObject()) << "Failed to request neighbor table entries" << zdoReply->error();
            reply->finishReply(ZigbeeReply::ErrorZigbeeError);
            return;
        }

        qCDebug(dcZigbeeNode()) << ZigbeeUtils::convertByteArrayToHexString(zdoReply->responseData());
        // I | ZigbeeNode: "0x01 0xac 0x0a 0x05 0xff 0xff 0x2e 0x21 0x00 0xac 0x0a 0x05 0xff 0xff 0x2e 0x21 0x00 0x00 0x00 0x04 0x02 0x00 0xfc"

        // Parse the neighbor response
        ZigbeeDeviceProfile::NeighborTableList listRecords = ZigbeeDeviceProfile::parseNeighborTableListRecord(zdoReply->responseData());
        // Append the neighbor table entries and continue reading if required
        m_temporaryNeighborTableRecords.append(listRecords.neighborTableListRecords);
        if (m_temporaryNeighborTableRecords.count() == listRecords.tableSize) {
            // We are done, replace the current neighbor table records
            m_neighborTableRecords.clear();
            m_neighborTableRecords = m_temporaryNeighborTableRecords;
            m_temporaryNeighborTableRecords.clear();

            // Finished successfully
            reply->finishReply();
            emit neighborTableRecordsChanged();

            qCDebug(dcZigbeeNode()) << "Neighbor table records changed:";
            foreach (const ZigbeeDeviceProfile::NeighborTableListRecord &record, m_neighborTableRecords) {
                // Get the associated neighbor
                ZigbeeNode *neighbor = m_network->getZigbeeNode(record.ieeeAddress);
                if (neighbor) {
                    qCDebug(dcZigbeeNode()) << "-->" << neighbor;
                    qCDebug(dcZigbeeNode()) << "    " << record;
                } else {
                    qCWarning(dcZigbeeNode()) << "--> Unknown node" << record;
                }
            }
            return;
        }

        // Continue reading at index until we fetched all neighbors
        readNextNeigborTableRecords(reply, m_temporaryNeighborTableRecords.size() - 1);
    });
}

void ZigbeeNode::setupEndpointInternal(ZigbeeNodeEndpoint *endpoint)
{
    // Connect after initialization for out of spec nodes
    connect(endpoint, &ZigbeeNodeEndpoint::inputClusterAdded, this, &ZigbeeNode::clusterAdded);
    connect(endpoint, &ZigbeeNodeEndpoint::outputClusterAdded, this, &ZigbeeNode::clusterAdded);
    connect(endpoint, &ZigbeeNodeEndpoint::clusterAttributeChanged, this, [this, endpoint](ZigbeeCluster *cluster, const ZigbeeClusterAttribute &attribute){
        if (cluster->clusterId() == ZigbeeClusterLibrary::ClusterIdBasic && attribute.id() == ZigbeeClusterBasic::AttributeManufacturerName) {
            bool valueOk = false;
            QString manufacturerName = attribute.dataType().toString(&valueOk);
            if (valueOk) {
                if (m_manufacturerName != manufacturerName) {
                    m_manufacturerName = manufacturerName;
                    emit manufacturerNameChanged(m_manufacturerName);
                    endpoint->m_manufacturerName = manufacturerName;
                    emit endpoint->manufacturerNameChanged(m_manufacturerName);
                }
            } else {
                qCWarning(dcZigbeeNode()) << "Could not convert manufacturer name attribute data to string" << attribute.dataType();
            }
        }

        if (cluster->clusterId() == ZigbeeClusterLibrary::ClusterIdBasic && attribute.id() == ZigbeeClusterBasic::AttributeModelIdentifier) {
            bool valueOk = false;
            QString modelName = attribute.dataType().toString(&valueOk);
            if (valueOk) {
                if (m_modelName != modelName) {
                    m_modelName = modelName;
                    emit modelNameChanged(m_modelName);
                    endpoint->m_modelIdentifier = modelName;
                    emit endpoint->modelIdentifierChanged(m_modelName);
                }
            } else {
                qCWarning(dcZigbeeNode()) << "Could not convert model identifier attribute data to string" << attribute.dataType();
            }
        }

        if (cluster->clusterId() == ZigbeeClusterLibrary::ClusterIdBasic && attribute.id() == ZigbeeClusterBasic::AttributeSwBuildId) {
            bool valueOk = false;
            QString version = attribute.dataType().toString(&valueOk);
            if (valueOk) {
                if (m_version != version) {
                    m_version = version;
                    emit versionChanged(m_version);
                    endpoint->m_softwareBuildId = version;
                    emit endpoint->softwareBuildIdChanged(m_version);
                }
            } else {
                qCWarning(dcZigbeeNode()) << "Could not convert software build id attribute data to string" << attribute.dataType();
            }
        }

        emit endpointClusterAttributeChanged(endpoint, cluster, attribute);
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
        // Set the device initialized any ways since this ist just for convenience
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
            m_manufacturerName = manufacturerName;
            emit manufacturerNameChanged(m_manufacturerName);
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
            if (m_requestRetry < m_requestRetriesMax) {
                m_requestRetry++;
                qCDebug(dcZigbeeNode()) << "Retry to read manufacturer name from" << this << basicCluster << m_requestRetry << "/" << m_requestRetriesMax << "attempts.";
                QTimer::singleShot(500, this, [=](){ readManufacturerName(basicCluster); });
            } else {
                qCWarning(dcZigbeeNode()) << "Failed to read manufacturer name from" << this << basicCluster << "after" << m_requestRetriesMax << "attempts. Giving up and continue...";
                m_requestRetry = 0;
                readModelIdentifier(basicCluster);
            }
            return;
        }

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
                m_manufacturerName = manufacturerName;
                emit manufacturerNameChanged(m_manufacturerName);
            } else {
                qCWarning(dcZigbeeNode()) << "Could not convert manufacturer name attribute data to string" << attributeStatusRecord.dataType;
            }
        }

        // Continue eiterh way with attribute reading
        m_requestRetry = 0;
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
            m_modelName = modelIdentifier;
            emit modelNameChanged(m_modelName);
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
            if (m_requestRetry < m_requestRetriesMax) {
                m_requestRetry++;
                qCDebug(dcZigbeeNode()) << "Retry to read model identifier from" << this << basicCluster << m_requestRetry << "/" << m_requestRetriesMax << "attempts.";
                QTimer::singleShot(500, this, [=](){ readModelIdentifier(basicCluster); });
            } else {
                qCWarning(dcZigbeeNode()) << "Failed to read model identifier from" << this << basicCluster << "after" << m_requestRetriesMax << "attempts. Giving up and continue...";
                m_requestRetry = 0;
                readSoftwareBuildId(basicCluster);
            }
            return;
        }

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
                m_modelName = modelIdentifier;
                emit modelNameChanged(m_modelName);
            } else {
                qCWarning(dcZigbeeNode()) << "Could not convert model identifier attribute data to string" << attributeStatusRecord.dataType;
            }
        }

        // Continue eiterh way with attribute reading
        m_requestRetry = 0;
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
            if (m_requestRetry < m_requestRetriesMax) {
                m_requestRetry++;
                qCDebug(dcZigbeeNode()) << "Retry to read model identifier from" << this << basicCluster << m_requestRetry << "/" << m_requestRetriesMax << "attempts.";
                QTimer::singleShot(500, this, [=](){ readSoftwareBuildId(basicCluster); });
            } else {
                qCWarning(dcZigbeeNode()) << "Failed to read model identifier from" << this << basicCluster << "after" << m_requestRetriesMax << "attempts. Giving up and continue...";
                m_requestRetry = 0;
                setState(StateInitialized);
            }
            return;
        }

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
                m_version = softwareBuildId;
                emit versionChanged(m_version);
            } else {
                qCWarning(dcZigbeeNode()) << "Could not convert software build id attribute data to string" << attributeStatusRecord.dataType;
            }
        }

        // Finished with reading basic cluster, the node is initialized.
        setState(StateInitialized);
    });
}

void ZigbeeNode::handleDataIndication(const Zigbee::ApsdeDataIndication &indication)
{
    if (indication.lqi != m_lqi) {
        m_lqi = indication.lqi;
        emit lqiChanged(m_lqi);
    }

    // Data received from this node, it is reachable for sure
    setReachable(true);

    // Update the UTC timestamp of last seen for reachable verification
    if (m_lastSeen != QDateTime::currentDateTimeUtc()) {
        m_lastSeen = QDateTime::currentDateTimeUtc();
        emit lastSeenChanged(m_lastSeen);
    }

    // Check if this indocation is related to any pending reply
    if (indication.profileId == Zigbee::ZigbeeProfileDevice) {
        deviceObject()->processApsDataIndication(indication);
        return;
    }

    // Let the clusters handle this indication
    handleZigbeeClusterLibraryIndication(indication);
}

void ZigbeeNode::handleZigbeeClusterLibraryIndication(const Zigbee::ApsdeDataIndication &indication)
{
    qCDebug(dcZigbeeNode()) << "Processing ZCL indication" << indication;

    // Get the endpoint
    ZigbeeNodeEndpoint *endpoint = getEndpoint(indication.sourceEndpoint);
    if (!endpoint) {
        qCDebug(dcZigbeeNetwork()) << "Received a ZCL indication for an unrecognized endpoint. There is no such endpoint on" << this << "Creating the uninitialized endpoint";
        // Create the uninitialized endpoint for now and fetch information later
        endpoint = new ZigbeeNodeEndpoint(m_network, this, indication.sourceEndpoint, this);
        endpoint->setProfile(static_cast<Zigbee::ZigbeeProfile>(indication.profileId));
        // Note: the endpoint is not initializd yet, but keep it anyways
        setupEndpointInternal(endpoint);
        m_endpoints.append(endpoint);
    }

    endpoint->handleZigbeeClusterLibraryIndication(indication);
}

QDebug operator<<(QDebug debug, ZigbeeNode *node)
{
    debug.nospace().noquote() << "ZigbeeNode(" << ZigbeeUtils::convertUint16ToHexString(node->shortAddress());
    debug.nospace().noquote() << ", " << node->extendedAddress().toString();
    if (!node->manufacturerName().isEmpty())
        debug.nospace().noquote() << ", " << node->manufacturerName() << " (" << ZigbeeUtils::convertUint16ToHexString(node->nodeDescriptor().manufacturerCode) << ")";
    else
        debug.nospace().noquote() << ", " << ZigbeeUtils::convertUint16ToHexString(node->nodeDescriptor().manufacturerCode);

    if (!node->modelName().isEmpty())
        debug.nospace().noquote() << ", " << node->modelName();

    switch (node->nodeDescriptor().nodeType) {
    case ZigbeeDeviceProfile::NodeTypeCoordinator:
        debug.nospace().noquote() << ", Coordinator";
        break;
    case ZigbeeDeviceProfile::NodeTypeRouter:
        debug.nospace().noquote() << ", Router";
        break;
    case ZigbeeDeviceProfile::NodeTypeEndDevice:
        debug.nospace().noquote() << ", End device";
        break;
    }

    debug.nospace().noquote() << ", RxOn:" << node->macCapabilities().receiverOnWhenIdle;
    debug.nospace().noquote() << ")";
    return debug.space().quote();
}
