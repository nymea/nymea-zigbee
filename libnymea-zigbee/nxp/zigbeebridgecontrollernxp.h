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

#ifndef ZIGBEEBRIDGECONTROLLERNXP_H
#define ZIGBEEBRIDGECONTROLLERNXP_H

#include <QObject>
#include <QQueue>

#include "zigbee.h"
#include "zigbeenode.h"
#include "zigbeebridgecontroller.h"

#include "interface/zigbeeinterface.h"
#include "interface/zigbeeinterfacereply.h"
#include "interface/zigbeeinterfacerequest.h"
#include "interface/zigbeeinterfacemessage.h"

class ZigbeeBridgeControllerNxp : public ZigbeeBridgeController
{
    Q_OBJECT
public:
    explicit ZigbeeBridgeControllerNxp(QObject *parent = nullptr);
    ~ZigbeeBridgeControllerNxp() override;

    void setFirmwareVersionString(const QString &firmwareVersion);

    // Direct commands
    ZigbeeInterfaceReply *commandResetController();
    ZigbeeInterfaceReply *commandSoftResetController();
    ZigbeeInterfaceReply *commandErasePersistantData();
    ZigbeeInterfaceReply *commandGetVersion();
    ZigbeeInterfaceReply *commandSetExtendedPanId(quint64 extendedPanId);
    ZigbeeInterfaceReply *commandSetChannelMask(quint32 channelMask = 0x07fff800);
    ZigbeeInterfaceReply *commandSetNodeType(ZigbeeNode::NodeType nodeType);
    ZigbeeInterfaceReply *commandStartNetwork();
    ZigbeeInterfaceReply *commandStartScan();
    ZigbeeInterfaceReply *commandPermitJoin(quint16 targetAddress = 0xfffc, const quint8 advertisingIntervall = 180, bool tcSignificance = false);

    ZigbeeInterfaceReply *commandGetPermitJoinStatus();
    ZigbeeInterfaceReply *commandRequestLinkQuality(quint16 shortAddress);

    ZigbeeInterfaceReply *commandEnableWhiteList();
    ZigbeeInterfaceReply *commandInitiateTouchLink();
    ZigbeeInterfaceReply *commandTouchLinkFactoryReset();

    ZigbeeInterfaceReply *commandActiveEndpointsRequest(quint16 shortAddress);
    ZigbeeInterfaceReply *commandNetworkAddressRequest(quint16 targetAddress, quint64 extendedAddress);
    ZigbeeInterfaceReply *commandSetSecurityStateAndKey(quint8 keyState, quint8 keySequence, quint8 keyType, const QByteArray &key);
    ZigbeeInterfaceReply *commandAuthenticateDevice(const ZigbeeAddress &ieeeAddress, const QByteArray &key);
    ZigbeeInterfaceReply *commandNodeDescriptorRequest(quint16 shortAddress);
    ZigbeeInterfaceReply *commandSimpleDescriptorRequest(quint16 shortAddress, quint8 endpoint);
    ZigbeeInterfaceReply *commandPowerDescriptorRequest(quint16 shortAddress);
    ZigbeeInterfaceReply *commandUserDescriptorRequest(quint16 shortAddress, quint16 address);

    ZigbeeInterfaceReply *commandReadAttributeRequest(quint8 addressMode, quint16 shortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, ZigbeeCluster *cluster, QList<quint16> attributes, bool manufacturerSpecific, quint16 manufacturerId);
    ZigbeeInterfaceReply *commandConfigureReportingRequest(quint8 addressMode, quint16 shortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, ZigbeeCluster *cluster, QList<quint16> attributes, bool manufacturerSpecific, quint16 manufacturerId);
    ZigbeeInterfaceReply *commandIdentify(quint8 addressMode, quint16 shortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint16 duration);

    ZigbeeInterfaceReply *commandBindGroup(const ZigbeeAddress &sourceAddress, quint8 sourceEndpoint, quint16 clusterId, quint16 destinationAddress, quint8 destinationEndpoint);
    ZigbeeInterfaceReply *commandBindUnicast(const ZigbeeAddress &sourceAddress, quint8 sourceEndpoint, quint16 clusterId, const ZigbeeAddress &destinationAddress, quint8 destinationEndpoint);

    ZigbeeInterfaceReply *commandBindLocalGroup(quint16 clusterId, quint8 sourceEndpoint, quint16 groupAddress);


    // On/Off
    ZigbeeInterfaceReply *commandOnOffNoEffects(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, ZigbeeCluster::OnOffClusterCommand command);

    // Group
    ZigbeeInterfaceReply *commandAddGroup(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint16 groupAddress);

    // Level
    ZigbeeInterfaceReply *commandMoveToLevel(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint8 onOff, quint8 level, quint16 transitionTime);

    // Level
    ZigbeeInterfaceReply *commandMoveToColourTemperature(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint16 colourTemperature, quint16 transitionTime);
    ZigbeeInterfaceReply *commandMoveToColor(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint16 x, quint16 y, quint16 transitionTime);
    ZigbeeInterfaceReply *commandMoveToHueSaturation(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint8 hue, quint8 saturation, quint16 transitionTime);
    ZigbeeInterfaceReply *commandMoveToHue(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint8 hue, quint16 transitionTime);
    ZigbeeInterfaceReply *commandMoveToSaturation(quint8 addressMode, quint16 targetShortAddress, quint8 sourceEndpoint, quint8 destinationEndpoint, quint8 saturation, quint16 transitionTime);


private:
    ZigbeeInterface *m_interface = nullptr;
    ZigbeeInterfaceReply *m_currentReply = nullptr;

    QQueue<ZigbeeInterfaceReply *> m_replyQueue;

    void sendMessage(ZigbeeInterfaceReply *reply);

signals:
    void messageReceived(const ZigbeeInterfaceMessage &message);

private slots:
    void onInterfaceAvailableChanged(bool available);
    void onMessageReceived(const ZigbeeInterfaceMessage &message);
    void onReplyTimeout();

public slots:
    bool enable(const QString &serialPort, qint32 baudrate);
    void disable();

    ZigbeeInterfaceReply *sendRequest(const ZigbeeInterfaceRequest &request);

};

#endif // ZIGBEEBRIDGECONTROLLERNXP_H
