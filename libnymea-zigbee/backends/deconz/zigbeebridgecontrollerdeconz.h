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

#ifndef ZIGBEEBRIDGECONTROLLERDECONZ_H
#define ZIGBEEBRIDGECONTROLLERDECONZ_H

#include <QHash>
#include <QTimer>
#include <QQueue>
#include <QObject>

#include "zigbee.h"
#include "zigbeeaddress.h"
#include "zigbeenetworkkey.h"
#include "zigbeenetworkrequest.h"
#include "zigbeebridgecontroller.h"

#include "interface/deconz.h"
#include "interface/zigbeeinterfacedeconz.h"
#include "interface/zigbeeinterfacedeconzreply.h"

// This struct describes the current deCONZ network configuration parameters
typedef struct DeconzNetworkConfiguration {
    ZigbeeAddress ieeeAddress; // R
    quint16 panId = 0; // R
    quint16 shortAddress = 0; // R
    quint64 extendedPanId = 0; // R
    Deconz::NodeType nodeType = Deconz::NodeTypeCoordinator; // RW
    quint32 channelMask = 0; // RW
    quint64 apsExtendedPanId = 0; // RW
    ZigbeeAddress trustCenterAddress; // RW
    Deconz::SecurityMode securityMode = Deconz::SecurityModeNoMasterButTrustCenterKey; // RW
    ZigbeeNetworkKey networkKey; // RW
    quint8 currentChannel = 0; // R
    quint16 protocolVersion = 0; // R
    quint8 networkUpdateId = 0; // RW
    quint32 watchdogTimeout = 85; // RW
} DeconzNetworkConfiguration;


// This struct describes the deCONZ device state
typedef struct DeconzDeviceState {
    Deconz::NetworkState networkState = Deconz::NetworkStateOffline;
    bool apsDataConfirm = false;
    bool apsDataIndication = false;
    bool configurationChanged = false;
    bool apsDataRequestFreeSlots = false;
} DeconzDeviceState;

class ZigbeeBridgeControllerDeconz : public ZigbeeBridgeController
{
    Q_OBJECT

    friend class ZigbeeNetworkDeconz;

public:
    explicit ZigbeeBridgeControllerDeconz(QObject *parent = nullptr);
    ~ZigbeeBridgeControllerDeconz() override;

    DeconzNetworkConfiguration networkConfiguration() const;
    void setFirmwareVersionString(const QString &firmwareVersion);

    Deconz::NetworkState networkState() const;

    // Controllere requests
    ZigbeeInterfaceDeconzReply *requestVersion();
    ZigbeeInterfaceDeconzReply *requestDeviceState();
    ZigbeeInterfaceDeconzReply *requestReadParameter(Deconz::Parameter parameter);
    ZigbeeInterfaceDeconzReply *requestWriteParameter(Deconz::Parameter parameter, const QByteArray &data);
    ZigbeeInterfaceDeconzReply *requestChangeNetworkState(Deconz::NetworkState networkState);

    // Send APS request data
    ZigbeeInterfaceDeconzReply *requestSendRequest(const ZigbeeNetworkRequest &request);

private:
    ZigbeeInterfaceDeconz *m_interface = nullptr;
    quint8 m_sequenceNumber = 0;
    quint32 m_watchdogTimeout = 300;
    int m_watchdogResetTimout = 280;

    DeconzNetworkConfiguration m_networkConfiguration;
    Deconz::NetworkState m_networkState = Deconz::NetworkStateOffline;
    QTimer *m_watchdogTimer = nullptr;

    // APS request queue
    bool m_apsFreeSlotsAvailable = true;
    ZigbeeInterfaceDeconzReply *m_currentReply = nullptr;
    ZigbeeInterfaceDeconzReply *m_readConfirmReply = nullptr;
    ZigbeeInterfaceDeconzReply *m_readIndicationReply = nullptr;

    QQueue<ZigbeeInterfaceDeconzReply *> m_replyQueue;

    quint8 generateSequenceNumber();

    ZigbeeInterfaceDeconzReply *createReply(Deconz::Command command, const QString &requestName, const QByteArray &requestData, QObject *parent);

    // Send data depending on the request destination address mode
    QByteArray buildRequestEnqueueSendDataGroupMessage(quint8 requestId, quint16 groupAddress, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius = 0);
    ZigbeeInterfaceDeconzReply *requestEnqueueSendDataGroup(quint8 requestId, quint16 groupAddress, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius = 0);
    ZigbeeInterfaceDeconzReply *requestEnqueueSendDataShortAddress(quint8 requestId, quint16 shortAddress, quint8 destinationEndpoint, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius = 0);
    ZigbeeInterfaceDeconzReply *requestEnqueueSendDataIeeeAddress(quint8 requestId, ZigbeeAddress ieeeAddress, quint8 destinationEndpoint, quint16 profileId, quint16 clusterId, quint8 sourceEndpoint, const QByteArray &asdu, Zigbee::ZigbeeTxOptions txOptions, quint8 radius = 0);

    // Receive data
    ZigbeeInterfaceDeconzReply *requestReadReceivedDataIndication(Deconz::SourceAddressMode sourceAddressMode = Deconz::SourceAddressModeShortSourceAddress);
    ZigbeeInterfaceDeconzReply *requestQuerySendDataConfirm();

    // Note: this method reads all parameters individual. The returned reply it self will not send or receive any data.
    // The data can be fetched from m_networkConfiguration on success.
    ZigbeeInterfaceDeconzReply *readNetworkParameters();

    // Device state helper
    DeconzDeviceState parseDeviceStateFlag(quint8 deviceStateFlag);

    void readDataIndication();
    void readDataConfirm();

    void processDeviceState(DeconzDeviceState deviceState);
    void processDataIndication(const QByteArray &data);
    void processDataConfirm(const QByteArray &data);

signals:
    void networkStateChanged(Deconz::NetworkState networkState);
    void networkConfigurationParameterChanged(const DeconzNetworkConfiguration &networkConfiguration);

    void apsDataConfirmReceived(const Zigbee::ApsdeDataConfirm &confirm);
    void apsDataIndicationReceived(const Zigbee::ApsdeDataIndication &indication);

private slots:
    void onInterfaceAvailableChanged(bool available);
    void onInterfacePackageReceived(const QByteArray &package);
    void resetControllerWatchdog();
    void sendNextRequest();

public slots:
    bool enable(const QString &serialPort, qint32 baudrate);
    void disable();
};

QDebug operator<<(QDebug debug, const DeconzDeviceState &deviceState);
QDebug operator<<(QDebug debug, const DeconzNetworkConfiguration &configuration);


#endif // ZIGBEEBRIDGECONTROLLERDECONZ_H
