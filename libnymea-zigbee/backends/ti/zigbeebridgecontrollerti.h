/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2022, nymea GmbH
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

#ifndef ZIGBEEBRIDGECONTROLLERTI_H
#define ZIGBEEBRIDGECONTROLLERTI_H

#include <QHash>
#include <QTimer>
#include <QQueue>
#include <QObject>

#include "zigbee.h"
#include "zigbeenetwork.h"
#include "zigbeeaddress.h"
#include "zigbeenetworkkey.h"
#include "zigbeenetworkrequest.h"
#include "zigbeebridgecontroller.h"

#include "interface/ti.h"
#include "interface/zigbeeinterfaceti.h"
#include "interface/zigbeeinterfacetireply.h"

typedef struct TiNetworkConfiguration {
    ZigbeeAddress ieeeAddress; // R
    quint16 panId = 0; // R
    quint16 shortAddress = 0; // R
    quint64 extendedPanId = 0; // R
    ZigbeeChannelMask channelMask = 0; // RW
    ZigbeeNetworkKey networkKey; // RW
    quint8 currentChannel = 0; // R
    Ti::ZnpVersion znpVersion = Ti::zStack12;
} TiNetworkConfiguration;

class ZigbeeBridgeControllerTi : public ZigbeeBridgeController
{
    Q_OBJECT

    friend class ZigbeeNetworkTi;

public:
    enum ControllerState {
        ControllerStateDown,
        ControllerStateInitialized,
        ControllerStateRunning,
    };
    Q_ENUM(ControllerState)

    explicit ZigbeeBridgeControllerTi(QObject *parent = nullptr);
    ~ZigbeeBridgeControllerTi() override;

    ControllerState state() const;

    ZigbeeInterfaceTiReply *init();
    ZigbeeInterfaceTiReply *commission(Ti::DeviceLogicalType deviceType, quint16 panId, const ZigbeeChannelMask &channelMask);
    ZigbeeInterfaceTiReply *start();
    ZigbeeInterfaceTiReply *reset();
    ZigbeeInterfaceTiReply *factoryReset();

    // Network config will be available after initialisation.
    TiNetworkConfiguration networkConfiguration() const;

    // Anything else is available once running
    ZigbeeInterfaceTiReply *setLed(bool on);

    ZigbeeInterfaceTiReply *requestPermitJoin(quint8 seconds, const quint16 &networkAddress);
    ZigbeeInterfaceTiReply *registerEndpoint(quint8 endpointId, Zigbee::ZigbeeProfile profile, quint16 deviceId, quint8 deviceVersion, const QList<quint16> &inputClusters = QList<quint16>(), const QList<quint16> &outputClusters = QList<quint16>());
    ZigbeeInterfaceTiReply *addEndpointToGroup(quint8 endpointId, quint16 groupId);

    // Send APS request data
    ZigbeeInterfaceTiReply *requestSendRequest(const ZigbeeNetworkRequest &request);

public slots:
    bool enable(const QString &serialPort, qint32 baudrate);
    void disable();

signals:
    void controllerStateChanged(ControllerState state);

    void permitJoinStateChanged(qint8 duration);
    void deviceIndication(quint16 shortAddress, const ZigbeeAddress &ieeeAddress, quint8 macCapabilities);
    void nodeLeft(const ZigbeeAddress &ieeeAddress, bool request, bool remove, bool rejoin);

    void deviceProfileIndicationReceived(const Zigbee::ApsdeDataIndication &indication);
    void clusterLibraryIndicationReceived(const Zigbee::ApsdeDataIndication &indication);

private slots:
    void onInterfaceAvailableChanged(bool available);
    void onInterfacePacketReceived(Ti::SubSystem subSystem, Ti::CommandType commandType, quint8 command, const QByteArray &payload);
    void sendNextRequest();

    void initPhase2(ZigbeeInterfaceTiReply* initReply, int attempt);
    void postStartup();

private:
    ZigbeeInterfaceTiReply *sendCommand(Ti::SubSystem subSystem, quint8 command, const QByteArray &payload = QByteArray(), int timeout = 5000);
    ZigbeeInterfaceTiReply *readNvItem(Ti::NvItemId itemId, quint16 offset = 0);
    ZigbeeInterfaceTiReply *writeNvItem(Ti::NvItemId itemId, const QByteArray &data, quint16 offset = 0);
    ZigbeeInterfaceTiReply *deleteNvItem(Ti::NvItemId itemId);
    void retrieveHugeMessage(const Zigbee::ApsdeDataIndication &pendingIndication, quint32 timestamp, quint16 dataLength);

    void waitFor(ZigbeeInterfaceTiReply *reply, Ti::SubSystem subSystem, quint8 command);
    void waitFor(ZigbeeInterfaceTiReply *reply, Ti::SubSystem subSystem, quint8 command, const QByteArray &payload);
    struct WaitData {
        Ti::SubSystem subSystem;
        quint8 command;
        QByteArray payload;
        bool comparePayload = false;
    };
    QHash<ZigbeeInterfaceTiReply*, WaitData> m_waitFors;

    ZigbeeInterfaceTi *m_interface = nullptr;

    TiNetworkConfiguration m_networkConfiguration;
    ControllerState m_controllerState = ControllerStateDown;

    ZigbeeInterfaceTiReply *m_currentReply = nullptr;

    QQueue<ZigbeeInterfaceTiReply *> m_replyQueue;

    QTimer m_permitJoinTimer;

    QList<int> m_registeredEndpointIds;

    void finishRequest(Ti::StatusCode statusCode = Ti::StatusCodeSuccess);
};

QDebug operator<<(QDebug debug, const TiNetworkConfiguration &configuration);


#endif // ZIGBEEBRIDGECONTROLLERTI_H
