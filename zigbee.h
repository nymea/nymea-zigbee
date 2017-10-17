#ifndef ZIGBEE_H
#define ZIGBEE_H

#include <QObject>
#include <QString>
#include <QByteArray>

class Zigbee
{
    Q_GADGET

public:
    enum InterfaceMessageType {
        /* Common Commands */
        Status                              = 0x8000,
        Logging                             = 0x8001,

        DataIndication                      = 0x8002,

        NodeClusterList                     = 0x8003,
        NodeAttributeList                   = 0x8004,
        NodeCommandIdList                   = 0x8005,
        RestartProvisioned                  = 0x8006,
        RestartFactoryNew                   = 0x8007,
        GetVersion                          = 0x0010,
        VersionList                         = 0x8010,

        SetExtendetPanId                    = 0x0020,
        SetChannelMask                      = 0x0021,
        SetSecurity                         = 0x0022,
        SetDeviceType                       = 0x0023,
        StartNetwork                        = 0x0024,
        StartScan                           = 0x0025,
        NetworkJoinedFormed                 = 0x8024,
        NetworkRemoveDevice                 = 0x0026,
        NetworkWhitelistEnable              = 0x0027,
        AuthenticateDeviceRequest           = 0x0028,
        AuthenticateDeviceResponse          = 0x8028,
        OutOfBandCommisioningDataRequest    = 0x0029,
        OutOfBandCommisioningDataResponse   = 0x8029,

        Reset                               = 0x0011,
        ErasePersistentData                 = 0x0012,
        ZllFactoryNew                       = 0x0013,
        GetPermitJoin                       = 0x0014,
        GetPermitJoinResponse               = 0x8014,
        Bind                                = 0x0030,
        BindResponse                        = 0x8030,
        Unbind                              = 0x0031,
        UnbindResponse                      = 0x8031,

        NetworkAdressRequest                = 0x0040,
        NetworkAdressResponse               = 0x8040,
        IeeeAddressResponse                 = 0x0041,
        IeeeAddressRequest                  = 0x8041,
        NodeDescriptorRequest               = 0x0042,
        NodeDescriptorRsponse               = 0x8042,
        SimpleDescriptorRequest             = 0x0043,
        SimpleDescriptorResponse            = 0x8043,
        PowerDescriptorRequest              = 0x0044,
        PowerDescriptorResponse             = 0x8044,
        ActiveEndpointRequest               = 0x0045,
        ActiveEndpointResponse              = 0x8045,
        MatchDescriptorRequest              = 0x0046,
        MatchDescriptorResponse             = 0x8046,
        ManagementLeaveRequest              = 0x0047,
        ManagementLeaveResponse             = 0x8047,
        LeaveIndication                     = 0x8048,
        PermitJoiningRequest                = 0x0049,
        ManagementNetworkUpdateRequest      = 0x004A,
        ManagementNetworkUpdateResponse     = 0x804A,
        SystemServerDiscoveryRequest        = 0x004B,
        SystemServerDiscoveryResponse       = 0x804B,
        DeviceAnnounce                      = 0x004D,
        ManagementLqiRequest                = 0x004E,
        ManagementLqiResponse               = 0x804E,

        /* Group Cluster */
        AddGroupRequest                     = 0x0060,
        AddGroupResponse                    = 0x8060,
        ViewGroupRequest                    = 0x0061,
        ViewGroupResponse                   = 0x8061,
        GetGroupMembershipRequest           = 0x0062,
        GetGroupMembershipResponse          = 0x8062,
        RemoveGroupRequest                  = 0x0063,
        RemoveGroupResponse                 = 0x8063,
        RemoveAllGroups                     = 0x0064,
        GroupIfIdentify                     = 0x0065,

        /* Identify Cluster */
        IdentifySend                        = 0x0070,
        IdentifyQuery                       = 0x0071,

        /* Level Cluster */
        MoveToLevel                         = 0x0080,
        MoveToLevelOnOff                    = 0x0081,
        MoveStep                            = 0x0082,
        MoveStopMove                        = 0x0083,
        MoveStopMoveOnOff                   = 0x0084,

        /* Scenes Cluster */
        ViewScene                           = 0x00A0,
        ViewSceneResponse                   = 0x80A0,
        AddScene                            = 0x00A1,
        AddSceneResponse                    = 0x80A1,
        RemoveScene                         = 0x00A2,
        RemoveSceneResponse                 = 0x80A2,
        RemoveAllScenes                     = 0x00A3,
        RemoveAllScenesResponse             = 0x80A3,
        StoreScene                          = 0x00A4,
        StoreSceneResponse                  = 0x80A4,
        RecallScene                         = 0x00A5,
        SceneMembershipRequest              = 0x00A6,
        SceneMembershipResponse             = 0x80A6,

        /* Colour Cluster */
        MoveToHue                           = 0x00B0,
        MoveHue                             = 0x00B1,
        StepHue                             = 0x00B2,
        MoveToSaturation                    = 0x00B3,
        MoveSaturation                      = 0x00B4,
        StepStaturation                     = 0x00B5,
        MoveToHueSaturation                 = 0x00B6,
        MoveToColor                         = 0x00B7,
        MoveColor                           = 0x00B8,
        StepColor                           = 0x00B9,

        /* ZLL Commands */
        /* Touchlink */
        InitiateTouchlink                   = 0x00D0,
        TouchlinkStatus                     = 0x00D1,
        TouchlinkFactoryReset               = 0x00D2,

        /* Identify Cluster */
        IdentifyTriggerEffect               = 0x00E0,

        /* On/Off Cluster */
        CluserOnOff                         = 0x0092,
        CluserOnOffTimed                    = 0x0093,
        CluserOnOffEffects                  = 0x0094,
        CluserOnOffUpdate                   = 0x8095,

        /* Scenes Cluster */
        AddEnhancedScene                    = 0x00A7,
        ViewEnhancedScene                   = 0x00A8,
        CopyScene                           = 0x00A9,

        /* Colour Cluster */
        EnhancedMoveToHue                   = 0x00BA,
        EnhancedMoveHue                     = 0x00BB,
        EnhancedStepHue                     = 0x00BC,
        EnhancedMoveToHueSaturation         = 0x00BD,
        ColourLoopSet                       = 0x00BE,
        StopMoveStep                        = 0x00BF,
        MoveToColorTemperature              = 0x00C0,
        MoveColorTemperature                = 0x00C1,
        StepColorTemperature                = 0x00C2,

        /* ZHA Commands */
        /* Door Lock Cluster */
        LockUnlockDoor                      = 0x00F0,

        /* Attributes */
        ReadAttributeRequest                = 0x0100,
        ReadAttributeResponse               = 0x8100,
        DefaultResponse                     = 0x8101,
        AttributeReport                     = 0x8102,
        WriteAttributeRequest               = 0x0110,
        WriteAttributeResponse              = 0x8110,
        ConfigReportingRequest              = 0x0120,
        ConfigReportingResponse             = 0x8120,
        ReportAttributes                    = 0x8121,
        AttributeDiscoveryRequest           = 0x0140,
        AttributeDiscoveryResponse          = 0x8140,

        /* Persistant data manager messages */
        DataManagerAvailableRequest         = 0x0300,
        DataManagerAvailableResponse        = 0x8300,
        DataManagerSaveRecordRequest        = 0x0200,
        DataManagerSaveRecordResponse       = 0x8200,
        DataManagerLoadRecordRequest        = 0x0201,
        DataManagerLoadRecordResponse       = 0x8201,
        DataManagerDeleteAllRecordsRequest  = 0x0202,
        DataManagerDeleteAllRecordsResponse = 0x8202,

        /* Appliance Statistics Cluster 0x0B03 */
        // http://www.nxp.com/documents/user_manual/JN-UG-3076.pdf
        StatisticsClusterLogMessage         = 0x0301,   // Was 0x0500, was 0x0301
        StatisticsClusterLogMessageResponse = 0x8301,

        /* IAS Cluster */
        SendIasZoneEnroolResponse			= 0x0400,
        IasZoneStatusChangeNotify           = 0x8401,
    };
    Q_ENUM(InterfaceMessageType)

    static QString convertByteToHexString(const quint8 &byte);
    static QString convertByteArrayToHexString(const QByteArray &byteArray);
    static QString convertByte16ToHexString(const quint16 &byte);


};

#endif // ZIGBEE_H
