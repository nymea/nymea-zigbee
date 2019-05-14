#ifndef ZIGBEEMANAGER_H
#define ZIGBEEMANAGER_H

#include <QObject>

#include "zigbeenetwork.h"
#include "zigbeeaddress.h"
#include "zigbeebridgecontroller.h"
#include "zigbeesecurityconfiguration.h"

class ZigbeeNetworkManager : public ZigbeeNetwork
{
    Q_OBJECT
public:
    explicit ZigbeeNetworkManager(QObject *parent = nullptr);

    QString controllerFirmwareVersion() const;

    // Note: temporary public available for debugging
    ZigbeeBridgeController *controller() const;

    bool networkRunning() const;

    // Note: Follwoing methods should be abstract
    bool permitJoining() const;
    void setPermitJoining(bool permitJoining);


private:
    enum StartingState {
        StartingStateNone,
        StartingStateErase,
        StartingStateReset,
        StartingStateGetVersion,
        StartingStateSetPanId,
        StartingStateSetChannel,
        StartingStateSetSecurity,
        StartingStateSetNodeType,
        StartingStateStartNetwork,
        StartingStateGetPermitJoinStatus,
        StartingStateReadeNodeDescriptor,
        StartingStateReadSimpleDescriptor,
        StartingStateReadPowerDescriptor
    };

    ZigbeeBridgeController *m_controller = nullptr;
    QString m_controllerFirmwareVersion;

    StartingState m_startingState = StartingStateNone;
    void setStartingState(StartingState state);

    QList<ZigbeeNode *> m_uninitializedNodes;

    bool m_permitJoining = false;
    bool m_networkRunning = false;
    bool m_factoryResetting = false;

signals:
    void permitJoiningChanged(bool permitJoining);

private slots:
    void onMessageReceived(const ZigbeeInterfaceMessage &message);
    void onControllerAvailableChanged(bool available);

    // Controller command finished slots
    void onCommandResetControllerFinished();
    void onCommandSoftResetControllerFinished();
    void onCommandErasePersistentDataFinished();
    void onCommandGetVersionFinished();
    void onCommandSetExtendedPanIdFinished();
    void onCommandSetChannelMaskFinished();
    void onCommandSetNodeTypeFinished();
    void onCommandStartNetworkFinished();
    void onCommandStartScanFinished();
    void onCommandGetPermitJoiningStatusFinished();
    void onCommandPermitJoiningFinished();
    void onCommandEnableWhitelistFinished();

    void onCommandNodeDescriptorRequestFinished();
    void onCommandSimpleDescriptorRequestFinished();
    void onCommandPowerDescriptorRequestFinished();

    void onCommandInitiateTouchLinkFinished();
    void onCommandTouchLinkFactoryResetFinished();
    void onCommandRequestLinkQualityFinished();

    void onCommandRequestMatchDescriptorFinished();
    void onCommandSetSecurityFinished();
    void onCommandNetworkAddressRequestFinished();
    void onCommandAuthenticateDeviceFinished();

    // Process controller notifications/messages
    void processNetworkFormed(const ZigbeeInterfaceMessage &message);
    void processLoggingMessage(const ZigbeeInterfaceMessage &message);
    void processFactoryNewRestart(const ZigbeeInterfaceMessage &message);
    void processNodeClusterList(const ZigbeeInterfaceMessage &message);
    void processNodeAttributeList(const ZigbeeInterfaceMessage &message);
    void processNodeCommandIdList(const ZigbeeInterfaceMessage &message);
    void processDeviceAnnounce(const ZigbeeInterfaceMessage &message);
    void processAttributeReport(const ZigbeeInterfaceMessage &message);
    void processLeaveIndication(const ZigbeeInterfaceMessage &message);
    void processRestartProvisioned(const ZigbeeInterfaceMessage &message);

public slots:
    void startNetwork() override;
    void stopNetwork() override;
    void factoryResetNetwork() override;

};

#endif // ZIGBEEMANAGER_H
