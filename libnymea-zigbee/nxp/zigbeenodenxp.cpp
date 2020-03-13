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

#include "zigbeenodenxp.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"
#include "zigbeenodeendpointnxp.h"

#include <QDataStream>

/* Node initialisation steps (sequentially)
 * - Authenticate
 * - Node descriptor
 * - Power descriptor
 * - Active endpoints
 * - for each endpoint do:
 *    - Simple descriptor request
 *    - for each endpoint
 *      - read basic cluster
 */


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
                    m_uninitializedEndpoints.append(endpointId);
                    qCDebug(dcZigbeeNode()) << " - " << ZigbeeUtils::convertByteToHexString(endpointId);
                }
            }
            setInitState(InitStateSimpleDescriptors);
        });
        break;
    }
    case InitStateSimpleDescriptors:
        for (int i = 0; i < m_uninitializedEndpoints.count(); i++) {
            quint8 endpointId = m_uninitializedEndpoints.at(i);
            qCDebug(dcZigbeeNode()) << "Request active endpoints for" << this << ZigbeeUtils::convertByteToHexString(endpointId);
            ZigbeeInterfaceReply *reply = m_controller->commandSimpleDescriptorRequest(shortAddress(), endpointId);
            connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, endpointId](){
                reply->deleteLater();
                if (reply->status() != ZigbeeInterfaceReply::Success) {
                    qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
                } else {
                    // Create endpoint

                    qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
                    qCDebug(dcZigbeeController()) << reply->additionalMessage();

                    QByteArray data = reply->additionalMessage().data();

                    quint8 sequenceNumber = 0;
                    quint8 status = 0;
                    quint16 shortAddress = 0;
                    quint8 length = 0;
                    quint8 endpointId = 0;
                    quint16 profileId = 0;
                    quint16 deviceId = 0;
                    quint8 bitField = 0;
                    quint8 inputClusterCount = 0;
                    quint8 outputClusterCount = 0;

                    QDataStream stream(&data, QIODevice::ReadOnly);
                    stream >> sequenceNumber >> status >> shortAddress >> length;
                    stream >> endpointId >> profileId >> deviceId >> bitField;

                    qCDebug(dcZigbeeNetwork()) << "Node endpoint simple descriptor:";
                    qCDebug(dcZigbeeNetwork()) << "    Sequence number:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
                    qCDebug(dcZigbeeNetwork()) << "    Status:" << ZigbeeUtils::convertByteToHexString(status);
                    qCDebug(dcZigbeeNetwork()) << "    Nwk address:" << ZigbeeUtils::convertUint16ToHexString(shortAddress);
                    qCDebug(dcZigbeeNetwork()) << "    Lenght:" << ZigbeeUtils::convertByteToHexString(length);
                    qCDebug(dcZigbeeNetwork()) << "    End Point:" << ZigbeeUtils::convertByteToHexString(endpointId);
                    qCDebug(dcZigbeeNetwork()) << "    Profile:" << ZigbeeUtils::profileIdToString(static_cast<Zigbee::ZigbeeProfile>(profileId));
                    if (profileId == Zigbee::ZigbeeProfileLightLink) {
                        qCDebug(dcZigbeeNetwork()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::LightLinkDevice>(deviceId);
                    } else {
                        qCDebug(dcZigbeeNetwork()) << "    Device ID:" << ZigbeeUtils::convertUint16ToHexString(deviceId) << static_cast<Zigbee::HomeAutomationDevice>(deviceId);
                    }

                    quint8 deviceVersion = (bitField >> 4);
                    qCDebug(dcZigbeeNetwork()) << "    Bit field:" << ZigbeeUtils::convertByteToHexString(bitField) << deviceVersion;

                    ZigbeeNodeEndpointNxp *endpoint = qobject_cast<ZigbeeNodeEndpointNxp *>(getEndpoint(endpointId));
                    if (!endpoint) {
                        endpoint = new ZigbeeNodeEndpointNxp(m_controller, this, endpointId, this);
                        m_endpoints.append(endpoint);
                    }

                    endpoint->setProfile(static_cast<Zigbee::ZigbeeProfile>(profileId));
                    endpoint->setDeviceId(deviceId);
                    endpoint->setDeviceVersion(deviceVersion);

                    stream >> inputClusterCount;
                    qCDebug(dcZigbeeNetwork()) << "    Input clusters: (" << inputClusterCount << ")";
                    for (int i = 0; i < inputClusterCount; i+=1) {
                        quint16 clusterId = 0;
                        stream >> clusterId;
                        if (!endpoint->hasInputCluster(static_cast<Zigbee::ClusterId>(clusterId))) {
                            endpoint->addInputCluster(new ZigbeeCluster(static_cast<Zigbee::ClusterId>(clusterId), ZigbeeCluster::Input, endpoint));
                        }
                        qCDebug(dcZigbeeNetwork()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
                    }

                    stream >> outputClusterCount;
                    qCDebug(dcZigbeeNetwork()) << "    Output clusters: (" << outputClusterCount << ")";
                    for (int i = 0; i < outputClusterCount; i+=1) {
                        if (stream.atEnd()) {
                            qCWarning(dcZigbeeNode()) << "Data stream already at the end but more data expected. Looks like the firmware doesn't provide more data.";
                            break;
                        }

                        quint16 clusterId = 0;
                        stream >> clusterId;
                        if (!endpoint->hasInputCluster(static_cast<Zigbee::ClusterId>(clusterId))) {
                            endpoint->addOutputCluster(new ZigbeeCluster(static_cast<Zigbee::ClusterId>(clusterId), ZigbeeCluster::Output, endpoint));
                        }
                        qCDebug(dcZigbeeNetwork()) << "        Cluster ID:" << ZigbeeUtils::convertUint16ToHexString(clusterId) << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
                    }
                }

                m_uninitializedEndpoints.removeAll(endpointId);
                if (m_uninitializedEndpoints.isEmpty()) {
                    qCDebug(dcZigbeeNode()) << "All endpoints fetched.";
                    setInitState(InitStateReadClusterAttributes);
                }
            });
        }
        break;
    case InitStateReadClusterAttributes:
//        if (shortAddress() == 0x0000) {
//            qCDebug(dcZigbeeNode()) << "No need to read the endpoint baisc clusters of the coordinator node";
//            setState(StateInitialized);
//            break;
//        }

        foreach (ZigbeeNodeEndpoint *endpoint, m_endpoints) {

            // Read basic cluster
            qCDebug(dcZigbeeNode()) << "Read basic cluster for endpoint" << endpoint;

            ZigbeeCluster *basicCluster = endpoint->getInputCluster(Zigbee::ClusterIdBasic);
            if (!basicCluster) {
                qCWarning(dcZigbeeNode()) << "Failed to fetch basic cluster from" << endpoint;
                setState(StateInitialized);
                return;
            }

            QList<quint16> attributes;
            attributes.append(ZigbeeCluster::BasicAttributeZclVersion);
            attributes.append(ZigbeeCluster::BasicAttributeManufacturerName);
            // Note: some devices inform about the model identifier trough attribute report and the cluster contains different information
            // Read the model identifier only if we don't have it yet. This is out of spec.
            if (!basicCluster->hasAttribute(ZigbeeCluster::BasicAttributeModelIdentifier))
                attributes.append(ZigbeeCluster::BasicAttributeModelIdentifier);

            attributes.append(ZigbeeCluster::BasicAttributePowerSource);
            attributes.append(ZigbeeCluster::BasicAttributeSwBuildId);
            ZigbeeInterfaceReply *reply = m_controller->commandReadAttributeRequest(0x02, shortAddress(),
                                                                                    0x01, endpoint->endpointId(),
                                                                                    basicCluster,
                                                                                    attributes,
                                                                                    false,
                                                                                    manufacturerCode());

            connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, endpoint](){
                reply->deleteLater();
                if (reply->status() != ZigbeeInterfaceReply::Success) {
                    qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
                }

                qCDebug(dcZigbeeNode()) << "Reading basic cluster attributes finished successfully for" << endpoint;
                qCDebug(dcZigbeeNode()) << "The device should response with multiple attribute read notifications.";
                setState(StateInitialized);
            });

            break;
        }
    }
}

void ZigbeeNodeNxp::startInitialization()
{
    qCDebug(dcZigbeeNode()) << "Start initialization" << this;
    setState(StateInitializing);
    setInitState(InitStateNodeDescriptor);
}

ZigbeeNodeEndpoint *ZigbeeNodeNxp::createNodeEndpoint(quint8 endpointId, QObject *parent)
{
    return new ZigbeeNodeEndpointNxp(m_controller, this, endpointId, parent);
}

void ZigbeeNodeNxp::setClusterAttributeReport(const ZigbeeClusterAttributeReport &report)
{
    if (report.attributeStatus != Zigbee::ZigbeeStatusSuccess) {
        qCWarning(dcZigbeeNode()) << this << "Got incalid status report" << report.endpointId << report.clusterId << report.attributeId << report.attributeStatus;
        return;
    }

    ZigbeeNodeEndpointNxp *endpoint = qobject_cast<ZigbeeNodeEndpointNxp *>(getEndpoint(report.endpointId));
    if (!endpoint) {
        qCDebug(dcZigbeeNetwork()) << "Recived attribute report but there is no endpoint on this node yet. Create it...";
        endpoint = new ZigbeeNodeEndpointNxp(m_controller, this, report.endpointId, this);
        m_endpoints.append(endpoint);
    }

    endpoint->setClusterAttribute(report.clusterId, ZigbeeClusterAttribute(report.attributeId, report.dataType, report.data));
}
