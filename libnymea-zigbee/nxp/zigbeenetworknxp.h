#ifndef ZIGBEENETWORKNXP_H
#define ZIGBEENETWORKNXP_H

#include <QObject>
#include "../zigbeenetwork.h"
#include "zigbeebridgecontrollernxp.h"

class ZigbeeNetworkNxp : public ZigbeeNetwork
{
    Q_OBJECT
public:
    explicit ZigbeeNetworkNxp(QObject *parent = nullptr);

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
        StartingStateReadPowerDescriptor
    };

    ZigbeeBridgeControllerNxp *m_controller = nullptr;
    bool m_networkRunning = false;
    bool m_factoryResetting = false;
    StartingState m_startingState = StartingStateNone;

    void setStartingState(StartingState state);
    void readControllerVersion();
    void readPermitJoinStatus();

private slots:
    void onMessageReceived(const ZigbeeInterfaceMessage &message);
    void onControllerAvailableChanged(bool available);

    // Controller command finished slots
    void onCommandResetControllerFinished();
    void onCommandSoftResetControllerFinished();
    void onCommandErasePersistentDataFinished();
    void onCommandSetExtendedPanIdFinished();
    void onCommandSetChannelMaskFinished();
    void onCommandSetNodeTypeFinished();
    void onCommandStartNetworkFinished();
    void onCommandStartScanFinished();
    void onCommandEnableWhitelistFinished();

//    void onCommandNodeDescriptorRequestFinished();
//    void onCommandSimpleDescriptorRequestFinished();
//    void onCommandPowerDescriptorRequestFinished();

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
    void processRouterDiscoveryConfirm(const ZigbeeInterfaceMessage &message);

public slots:
    void startNetwork() override;
    void stopNetwork() override;

    void factoryResetNetwork() override;

};

#endif // ZIGBEENETWORKNXP_H
