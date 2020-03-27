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

#include "zigbeenodeendpointnxp.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

ZigbeeNodeEndpointNxp::ZigbeeNodeEndpointNxp(ZigbeeBridgeControllerNxp *controller, ZigbeeNode *node, quint8 endpoint, QObject *parent) :
    ZigbeeNodeEndpoint(node, endpoint, parent),
    m_controller(controller)
{

}

ZigbeeNetworkReply *ZigbeeNodeEndpointNxp::readAttribute(ZigbeeCluster *cluster, QList<quint16> attributes)
{
    qCDebug(dcZigbeeNode()) << "Read" << node() << cluster << attributes;

    ZigbeeNetworkReply *networkReply = createNetworkReply();
    ZigbeeInterfaceReply *reply = m_controller->commandReadAttributeRequest(0x02, node()->shortAddress(), 0x01,
                                                                            endpointId(), cluster, attributes,
                                                                            false, node()->manufacturerCode());
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, networkReply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            finishNetworkReply(networkReply, ZigbeeNetworkReply::ErrorUnknown);
            return;
        }

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        if (!reply->additionalMessage().data().isEmpty()) {
            ZigbeeClusterAttributeReport report = ZigbeeUtils::parseAttributeReport(reply->additionalMessage().data());
            setClusterAttribute(report.clusterId, ZigbeeClusterAttribute(report.attributeId, report.dataType, report.data));
        }

        finishNetworkReply(networkReply);
    });

    return networkReply;
}

ZigbeeNetworkReply *ZigbeeNodeEndpointNxp::configureReporting(ZigbeeCluster *cluster, QList<ZigbeeClusterReportConfigurationRecord> reportConfigurations)
{
    qCDebug(dcZigbeeNode()) << "Configure reporting" << node();

    // FIXME: check the report configuration and the direction field according to specs

    ZigbeeNetworkReply *networkReply = createNetworkReply();
    ZigbeeInterfaceReply *reply = m_controller->commandConfigureReportingRequest(0x02, node()->shortAddress(), 0x01,
                                                                            endpointId(), cluster, 0x00,
                                                                            false, node()->manufacturerCode(), reportConfigurations);
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, networkReply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            finishNetworkReply(networkReply, ZigbeeNetworkReply::ErrorUnknown);
            return;
        }

        // Fixme: read configure response

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        finishNetworkReply(networkReply);
    });

    return networkReply;
}

ZigbeeNetworkReply *ZigbeeNodeEndpointNxp::identify(quint16 seconds)
{
    qCDebug(dcZigbeeNode()) << "Identify" << node() << seconds << seconds;
    ZigbeeNetworkReply *networkReply = createNetworkReply();
    ZigbeeInterfaceReply *reply = m_controller->commandIdentify(0x02, node()->shortAddress(), 0x01, endpointId(), seconds);
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, networkReply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            finishNetworkReply(networkReply, ZigbeeNetworkReply::ErrorUnknown);
            return;
        }

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        finishNetworkReply(networkReply);
    });

    return networkReply;
}

ZigbeeNetworkReply *ZigbeeNodeEndpointNxp::factoryReset()
{
    qCDebug(dcZigbeeNode()) << "Factory reset" << this;
    ZigbeeNetworkReply *networkReply = createNetworkReply();
    ZigbeeInterfaceReply *reply = m_controller->commandFactoryResetNode(node()->shortAddress(), 0x01, endpointId());
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, networkReply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            finishNetworkReply(networkReply, ZigbeeNetworkReply::ErrorUnknown);
            return;
        }

        // Fixme: read default response

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        finishNetworkReply(networkReply);
    });

    return networkReply;
}

ZigbeeNetworkReply *ZigbeeNodeEndpointNxp::bindGroup(Zigbee::ClusterId clusterId, quint16 destinationAddress, quint8 destinationEndpoint)
{
    qCDebug(dcZigbeeNode()) << "Bind group" << node() << clusterId << ZigbeeUtils::convertUint16ToHexString(destinationAddress) << ZigbeeUtils::convertByteToHexString(destinationEndpoint);
    ZigbeeNetworkReply *networkReply = createNetworkReply();
    ZigbeeInterfaceReply *reply = m_controller->commandBindGroup(node()->extendedAddress(), endpointId(), clusterId, destinationAddress, destinationEndpoint);
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, networkReply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            finishNetworkReply(networkReply, ZigbeeNetworkReply::ErrorUnknown);
            return;
        }

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        qCDebug(dcZigbeeController()) << reply->additionalMessage().data();
        finishNetworkReply(networkReply);
    });

    return networkReply;
}

ZigbeeNetworkReply *ZigbeeNodeEndpointNxp::bindUnicast(Zigbee::ClusterId clusterId, const ZigbeeAddress &destinationAddress, quint8 destinationEndpoint)
{
    qCDebug(dcZigbeeNode()) << "Bind unicast" << node() << clusterId << destinationAddress.toString() << ZigbeeUtils::convertByteToHexString(destinationEndpoint);
    ZigbeeNetworkReply *networkReply = createNetworkReply();
    ZigbeeInterfaceReply *reply = m_controller->commandBindUnicast(node()->extendedAddress(), endpointId(), clusterId, destinationAddress, destinationEndpoint);
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, networkReply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            finishNetworkReply(networkReply, ZigbeeNetworkReply::ErrorUnknown);
            return;
        }

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        qCDebug(dcZigbeeController()) << reply->additionalMessage().data();
        finishNetworkReply(networkReply);
    });

    return networkReply;
}

ZigbeeNetworkReply *ZigbeeNodeEndpointNxp::sendOnOffClusterCommand(ZigbeeCluster::OnOffClusterCommand command)
{
    qCDebug(dcZigbeeNode()) << "Send on/off cluster command" << node() << command;
    ZigbeeNetworkReply *networkReply = createNetworkReply();
    ZigbeeInterfaceReply *reply = m_controller->commandOnOffNoEffects(0x02, node()->shortAddress(), 0x01, endpointId(), command);
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, networkReply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            finishNetworkReply(networkReply, ZigbeeNetworkReply::ErrorUnknown);
            return;
        }

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        finishNetworkReply(networkReply);
    });

    return networkReply;
}

ZigbeeNetworkReply *ZigbeeNodeEndpointNxp::addGroup(quint8 destinationEndpoint, quint16 groupAddress)
{
    qCDebug(dcZigbeeNode()) << "Add group request" << node() << destinationEndpoint << groupAddress;
    ZigbeeNetworkReply *networkReply = createNetworkReply();
    ZigbeeInterfaceReply *reply = m_controller->commandAddGroup(0x02, node()->shortAddress(), 0x01, endpointId(), groupAddress);
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, networkReply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            finishNetworkReply(networkReply, ZigbeeNetworkReply::ErrorUnknown);
            return;
        }

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        qCDebug(dcZigbeeController()) << reply->additionalMessage().data();
        finishNetworkReply(networkReply);
    });

    return networkReply;
}

ZigbeeNetworkReply *ZigbeeNodeEndpointNxp::sendLevelCommand(ZigbeeCluster::LevelClusterCommand command, quint8 level, bool triggersOnOff, quint16 transitionTime)
{
    qCDebug(dcZigbeeNode()) << "Move to level request" << node() << command << level;
    ZigbeeNetworkReply *networkReply = createNetworkReply();
    ZigbeeInterfaceReply *reply = m_controller->commandMoveToLevel(0x02, node()->shortAddress(), 0x01, endpointId(), triggersOnOff, level, transitionTime);
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, networkReply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            finishNetworkReply(networkReply, ZigbeeNetworkReply::ErrorUnknown);
            return;
        }

        // Fixme: read default response

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        finishNetworkReply(networkReply);
    });

    return networkReply;
}

ZigbeeNetworkReply *ZigbeeNodeEndpointNxp::sendMoveToColorTemperature(quint16 colourTemperature, quint16 transitionTime)
{
    qCDebug(dcZigbeeNode()) << "Move to level request" << node() << colourTemperature << transitionTime;
    ZigbeeNetworkReply *networkReply = createNetworkReply();
    ZigbeeInterfaceReply *reply = m_controller->commandMoveToColourTemperature(0x02, node()->shortAddress(), 0x01, endpointId(), colourTemperature, transitionTime);
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, networkReply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            finishNetworkReply(networkReply, ZigbeeNetworkReply::ErrorUnknown);
            return;
        }

        // Fixme: read default response

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        finishNetworkReply(networkReply);
    });

    return networkReply;
}

ZigbeeNetworkReply *ZigbeeNodeEndpointNxp::sendMoveToColor(double x, double y, quint16 transitionTime)
{
    qCDebug(dcZigbeeNode()) << "Move to color request" << node() << x << y << transitionTime;

    quint16 normalizedX = static_cast<quint16>(qRound(x * 65536));
    quint16 normalizedY = static_cast<quint16>(qRound(y * 65536));

    ZigbeeNetworkReply *networkReply = createNetworkReply();
    ZigbeeInterfaceReply *reply = m_controller->commandMoveToColor(0x02, node()->shortAddress(), 0x01, endpointId(), normalizedX, normalizedY, transitionTime);
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, networkReply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            finishNetworkReply(networkReply, ZigbeeNetworkReply::ErrorUnknown);
            return;
        }

        // Fixme: read default response

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        finishNetworkReply(networkReply);
    });

    return networkReply;
}

ZigbeeNetworkReply *ZigbeeNodeEndpointNxp::sendMoveToHueSaturation(quint8 hue, quint8 saturation, quint16 transitionTime)
{
    qCDebug(dcZigbeeNode()) << "Move to hue saturation request" << node() << hue << saturation << transitionTime;
    ZigbeeNetworkReply *networkReply = createNetworkReply();
    ZigbeeInterfaceReply *reply = m_controller->commandMoveToHueSaturation(0x02, node()->shortAddress(), 0x01, endpointId(), hue, saturation, transitionTime);
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, networkReply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            finishNetworkReply(networkReply, ZigbeeNetworkReply::ErrorUnknown);
            return;
        }

        // Fixme: read default response

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        finishNetworkReply(networkReply);
    });

    return networkReply;
}

ZigbeeNetworkReply *ZigbeeNodeEndpointNxp::sendMoveToHue(quint8 hue, quint16 transitionTime)
{
    qCDebug(dcZigbeeNode()) << "Move to hue request" << node() << hue << transitionTime;
    ZigbeeNetworkReply *networkReply = createNetworkReply();
    ZigbeeInterfaceReply *reply = m_controller->commandMoveToHue(0x02, node()->shortAddress(), 0x01, endpointId(), hue, transitionTime);
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, networkReply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            finishNetworkReply(networkReply, ZigbeeNetworkReply::ErrorUnknown);
            return;
        }

        // Fixme: read default response

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        finishNetworkReply(networkReply);
    });

    return networkReply;
}

ZigbeeNetworkReply *ZigbeeNodeEndpointNxp::sendMoveToSaturation(quint8 saturation, quint16 transitionTime)
{
    qCDebug(dcZigbeeNode()) << "Move to saturation request" << node() << saturation << transitionTime;
    ZigbeeNetworkReply *networkReply = createNetworkReply();
    ZigbeeInterfaceReply *reply = m_controller->commandMoveToSaturation(0x02, node()->shortAddress(), 0x01, endpointId(), saturation, transitionTime);
    connect(reply, &ZigbeeInterfaceReply::finished, this, [this, reply, networkReply](){
        reply->deleteLater();

        if (reply->status() != ZigbeeInterfaceReply::Success) {
            qCWarning(dcZigbeeController()) << "Could not" << reply->request().description() << reply->status() << reply->statusErrorMessage();
            finishNetworkReply(networkReply, ZigbeeNetworkReply::ErrorUnknown);
            return;
        }

        // Fixme: read default response

        qCDebug(dcZigbeeController()) << reply->request().description() << "finished successfully";
        finishNetworkReply(networkReply);
    });

    return networkReply;
}

void ZigbeeNodeEndpointNxp::setClusterAttribute(Zigbee::ClusterId clusterId, const ZigbeeClusterAttribute &attribute)
{
    // Check if this cluster is an input cluster
    if (hasInputCluster(clusterId)) {
        ZigbeeCluster *cluster = getInputCluster(clusterId);
        cluster->setAttribute(attribute);
        emit clusterAttributeChanged(cluster, attribute);
        return;
    }

    // Check if this cluster is an output cluster
    if (hasOutputCluster(clusterId)) {
        ZigbeeCluster *cluster = getOutputCluster(clusterId);
        cluster->setAttribute(attribute);
        emit clusterAttributeChanged(cluster, attribute);
        return;
    }

    // There is no cluster yet. Create it as output cluster if this is not the basic cluster
    ZigbeeCluster *cluster = nullptr;
    if (clusterId == Zigbee::ClusterIdBasic) {
        cluster = new ZigbeeCluster(clusterId, ZigbeeCluster::Input, this);
        addInputCluster(cluster);
    } else {
        cluster = new ZigbeeCluster(clusterId, ZigbeeCluster::Output, this);
        addOutputCluster(cluster);
    }
    cluster->setAttribute(attribute);
    emit clusterAttributeChanged(cluster, attribute);

}
