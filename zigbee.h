#ifndef ZIGBEE_H
#define ZIGBEE_H

#include <QObject>
#include <QString>
#include <QByteArray>

class Zigbee
{
    Q_GADGET

public:
    enum ZigbeeProfile {
        ZigbeeProfileHomeAutomation = 0x0104,
        ZigbeeProfileLightLink      = 0xC05E
    };
    Q_ENUM(ZigbeeProfile)

    enum InterfaceMessageType {
        // Common Commands
        MessageTypeNone                                = 0x0000,
        MessageTypeStatus                              = 0x8000,
        MessageTypeLogging                             = 0x8001,

        MessageTypeDataIndication                      = 0x8002,

        MessageTypeNodeClusterList                     = 0x8003,
        MessageTypeNodeAttributeList                   = 0x8004,
        MessageTypeNodeCommandIdList                   = 0x8005,
        MessageTypeRestartProvisioned                  = 0x8006,
        MessageTypeFactoryNewRestart                   = 0x8007,
        MessageTypeGetVersion                          = 0x0010,
        MessageTypeVersionList                         = 0x8010,

        MessageTypeSetExtendetPanId                    = 0x0020,
        MessageTypeSetChannelMask                      = 0x0021,
        MessageTypeSetSecurity                         = 0x0022,
        MessageTypeSetDeviceType                       = 0x0023,
        MessageTypeStartNetwork                        = 0x0024,
        MessageTypeStartScan                           = 0x0025,
        MessageTypeNetworkJoinedFormed                 = 0x8024,
        MessageTypeNetworkRemoveDevice                 = 0x0026,
        MessageTypeNetworkWhitelistEnable              = 0x0027,
        MessageTypeAuthenticateDeviceRequest           = 0x0028,
        MessageTypeAuthenticateDeviceResponse          = 0x8028,
        MessageTypeOutOfBandCommisioningDataRequest    = 0x0029,
        MessageTypeOutOfBandCommisioningDataResponse   = 0x8029,

        MessageTypeReset                               = 0x0011,
        MessageTypeErasePersistentData                 = 0x0012,
        MessageTypeZllFactoryNew                       = 0x0013,
        MessageTypeGetPermitJoining                    = 0x0014,
        MessageTypeGetPermitJoiningResponse            = 0x8014,
        MessageTypeBind                                = 0x0030,
        MessageTypeBindResponse                        = 0x8030,
        MessageTypeUnbind                              = 0x0031,
        MessageTypeUnbindResponse                      = 0x8031,

        MessageTypeNetworkAdressRequest                = 0x0040,
        MessageTypeNetworkAdressResponse               = 0x8040,
        MessageTypeIeeeAddressResponse                 = 0x0041,
        MessageTypeIeeeAddressRequest                  = 0x8041,
        MessageTypeNodeDescriptorRequest               = 0x0042,
        MessageTypeNodeDescriptorRsponse               = 0x8042,
        MessageTypeSimpleDescriptorRequest             = 0x0043,
        MessageTypeSimpleDescriptorResponse            = 0x8043,
        MessageTypePowerDescriptorRequest              = 0x0044,
        MessageTypePowerDescriptorResponse             = 0x8044,
        MessageTypeActiveEndpointRequest               = 0x0045,
        MessageTypeActiveEndpointResponse              = 0x8045,
        MessageTypeMatchDescriptorRequest              = 0x0046,
        MessageTypeMatchDescriptorResponse             = 0x8046,
        MessageTypeManagementLeaveRequest              = 0x0047,
        MessageTypeManagementLeaveResponse             = 0x8047,
        MessageTypeLeaveIndication                     = 0x8048,
        MessageTypePermitJoiningRequest                = 0x0049,
        MessageTypeManagementNetworkUpdateRequest      = 0x004A,
        MessageTypeManagementNetworkUpdateResponse     = 0x804A,
        MessageTypeSystemServerDiscoveryRequest        = 0x004B,
        MessageTypeSystemServerDiscoveryResponse       = 0x804B,
        MessageTypeDeviceAnnounce                      = 0x004D,
        MessageTypeManagementLqiRequest                = 0x004E,
        MessageTypeManagementLqiResponse               = 0x804E,

        // Group Cluster
        MessageTypeAddGroupRequest                     = 0x0060,
        MessageTypeAddGroupResponse                    = 0x8060,
        MessageTypeViewGroupRequest                    = 0x0061,
        MessageTypeViewGroupResponse                   = 0x8061,
        MessageTypeGetGroupMembershipRequest           = 0x0062,
        MessageTypeGetGroupMembershipResponse          = 0x8062,
        MessageTypeRemoveGroupRequest                  = 0x0063,
        MessageTypeRemoveGroupResponse                 = 0x8063,
        MessageTypeRemoveAllGroups                     = 0x0064,
        MessageTypeGroupIfIdentify                     = 0x0065,

        // Identify Cluster
        MessageTypeIdentifySend                        = 0x0070,
        MessageTypeIdentifyQuery                       = 0x0071,

        // Level Cluster
        MessageTypeMoveToLevel                         = 0x0080,
        MessageTypeMoveToLevelOnOff                    = 0x0081,
        MessageTypeMoveStep                            = 0x0082,
        MessageTypeMoveStopMove                        = 0x0083,
        MessageTypeMoveStopMoveOnOff                   = 0x0084,

        // Scenes Cluster
        MessageTypeViewScene                           = 0x00A0,
        MessageTypeViewSceneResponse                   = 0x80A0,
        MessageTypeAddScene                            = 0x00A1,
        MessageTypeAddSceneResponse                    = 0x80A1,
        MessageTypeRemoveScene                         = 0x00A2,
        MessageTypeRemoveSceneResponse                 = 0x80A2,
        MessageTypeRemoveAllScenes                     = 0x00A3,
        MessageTypeRemoveAllScenesResponse             = 0x80A3,
        MessageTypeStoreScene                          = 0x00A4,
        MessageTypeStoreSceneResponse                  = 0x80A4,
        MessageTypeRecallScene                         = 0x00A5,
        MessageTypeSceneMembershipRequest              = 0x00A6,
        MessageTypeSceneMembershipResponse             = 0x80A6,

        //Colour Cluster
        MessageTypeMoveToHue                           = 0x00B0,
        MessageTypeMoveHue                             = 0x00B1,
        MessageTypeStepHue                             = 0x00B2,
        MessageTypeMoveToSaturation                    = 0x00B3,
        MessageTypeMoveSaturation                      = 0x00B4,
        MessageTypeStepStaturation                     = 0x00B5,
        MessageTypeMoveToHueSaturation                 = 0x00B6,
        MessageTypeMoveToColor                         = 0x00B7,
        MessageTypeMoveColor                           = 0x00B8,
        MessageTypeStepColor                           = 0x00B9,

        // ZLL Commands
        /* Touchlink */
        MessageTypeInitiateTouchlink                   = 0x00D0,
        MessageTypeTouchlinkStatus                     = 0x00D1,
        MessageTypeTouchlinkFactoryReset               = 0x00D2,

        // Identify Cluster
        MessageTypeIdentifyTriggerEffect               = 0x00E0,

        // On/Off Cluster
        MessageTypeCluserOnOff                         = 0x0092,
        MessageTypeCluserOnOffTimed                    = 0x0093,
        MessageTypeCluserOnOffEffects                  = 0x0094,
        MessageTypeCluserOnOffUpdate                   = 0x8095,

        // Scenes Cluster
        MessageTypeAddEnhancedScene                    = 0x00A7,
        MessageTypeViewEnhancedScene                   = 0x00A8,
        MessageTypeCopyScene                           = 0x00A9,

        // Colour Cluster
        MessageTypeEnhancedMoveToHue                   = 0x00BA,
        MessageTypeEnhancedMoveHue                     = 0x00BB,
        MessageTypeEnhancedStepHue                     = 0x00BC,
        MessageTypeEnhancedMoveToHueSaturation         = 0x00BD,
        MessageTypeColourLoopSet                       = 0x00BE,
        MessageTypeStopMoveStep                        = 0x00BF,
        MessageTypeMoveToColorTemperature              = 0x00C0,
        MessageTypeMoveColorTemperature                = 0x00C1,
        MessageTypeStepColorTemperature                = 0x00C2,

        // ZHA Commands
        // Door Lock Cluster
        MessageTypeLockUnlockDoor                      = 0x00F0,

        // Attributes
        MessageTypeReadAttributeRequest                = 0x0100,
        MessageTypeReadAttributeResponse               = 0x8100,
        MessageTypeDefaultResponse                     = 0x8101,
        MessageTypeAttributeReport                     = 0x8102,
        MessageTypeWriteAttributeRequest               = 0x0110,
        MessageTypeWriteAttributeResponse              = 0x8110,
        MessageTypeConfigReportingRequest              = 0x0120,
        MessageTypeConfigReportingResponse             = 0x8120,
        MessageTypeReportAttributes                    = 0x8121,
        MessageTypeAttributeDiscoveryRequest           = 0x0140,
        MessageTypeAttributeDiscoveryResponse          = 0x8140,

        /* Persistant data manager messages */
        MessageTypeDataManagerAvailableRequest         = 0x0300,
        MessageTypeDataManagerAvailableResponse        = 0x8300,
        MessageTypeDataManagerSaveRecordRequest        = 0x0200,
        MessageTypeDataManagerSaveRecordResponse       = 0x8200,
        MessageTypeDataManagerLoadRecordRequest        = 0x0201,
        MessageTypeDataManagerLoadRecordResponse       = 0x8201,
        MessageTypeDataManagerDeleteAllRecordsRequest  = 0x0202,
        MessageTypeDataManagerDeleteAllRecordsResponse = 0x8202,

        /* Appliance Statistics Cluster 0x0B03 */
        // http://www.nxp.com/documents/user_manual/JN-UG-3076.pdf
        MessageTypeStatisticsClusterLogMessage         = 0x0301,   // Was 0x0500, was 0x0301
        MessageTypeStatisticsClusterLogMessageResponse = 0x8301,

        /* IAS Cluster */
        MessageTypeSendIasZoneEnroolResponse			= 0x0400,
        MessageTypeIasZoneStatusChangeNotify            = 0x8401,
    };
    Q_ENUM(InterfaceMessageType)


    enum ClusterId {
        // Basics
        ClusterIdBasic                  = 0x0000,
        ClusterIdPower                  = 0x0001,
        ClusterIdDeviceTemperature      = 0x0002,
        ClusterIdIdentify               = 0x0003,
        ClusterIdGroups                 = 0x0004,
        ClusterIdScenes                 = 0x0005,
        ClusterIdOnOff                  = 0x0006,
        ClusterIdOnOffCOnfiguration     = 0x0007,
        ClusterIdLevelControl           = 0x0008,
        ClusterIdAlarms                 = 0x0009,
        ClusterIdTime                   = 0x000A,
        ClusterIdRssiLocation           = 0x000B,
        ClusterIdAnalogInputBasic       = 0x000C,
        ClusterIdAnalogOutputBasic      = 0x000D,
        ClusterIdValueBasic             = 0x000E,
        ClusterIdBinaryInputBasic       = 0x000F,
        ClusterIdBinaryOutputBasic      = 0x0010,
        ClusterIdBinaryValueBasic       = 0x0011,
        ClusterIdMultiStateInputBasic   = 0x0012,
        ClusterIdMultiStateOutputBasic  = 0x0013,
        ClusterIdMultiStateValueBasic   = 0x0014,
        ClusterIdCommissoning           = 0x0015,

        // Over the air uppgrade (OTA)
        ClusterIdOtaUpgrade             = 0x0019,

        // Closures
        ClusterIdShadeConfiguration     = 0x0100,

        // Door Lock
        ClusterIdDoorLock = 0x0101,

        // Heating, Ventilation and Air-Conditioning (HVAC)
        ClusterIdPumpConfigurationControl = 0x0200,
        ClusterIdThermostat               = 0x0201,
        ClusterIdFanControll              = 0x0202,
        ClusterIdDehumiditationControll   = 0x0203,
        ClusterIdThermostatUserControll   = 0x0204,

        // Lighting
        ClusterIdColorControl           = 0x0300,
        ClusterIdBallastConfiguration   = 0x0301,

        // Sensing
        ClusterIdMeasurementIlluminance         = 0x0400,
        ClusterIdIlluminanceLevelSensing        = 0x0401,
        ClusterIdTemperatureMeasurement         = 0x0402,
        ClusterIdPressureMeasurement            = 0x0403,
        ClusterIdFlowMeasurement                = 0x0404,
        ClusterIdRelativeHumidityMeasurement    = 0x0405,
        ClusterIdOccapancySensing               = 0x0406,

        // Security and Safty
        ClusterIdIasZone = 0x0500,
        ClusterIdIasAce  = 0x0501,
        ClusterIdIasWd   = 0x0502,

        // Smart energy
        ClusterIdPrice          = 0x0700,
        ClusterIdLoadControl    = 0x0701,
        ClusterIdSimpleMetering = 0x0702,

        // Electrical Measurement
        ClusterIdElectricalMeasurement = 0x0B04,

        // ZLL
        ClusterIdTouchlinkCommissioning = 0x1000
    };
    Q_ENUM(ClusterId)

    enum LightLinkDevice {

        // Lightning devices
        LightLinkDeviceOnOffLight               = 0x0000,
        LightLinkDeviceOnOffPlug                = 0x0010,
        LightLinkDeviceDimmableLight            = 0x0100,
        LightLinkDeviceDimmablePlug             = 0x0110,
        LightLinkDeviceColourLight              = 0x0200,
        LightLinkDeviceExtendedColourLight      = 0x0210,
        LightLinkDeviceColourTemperatureLight   = 0x0220,

        // Controller devices
        LightLinkDeviceColourController         = 0x8000,
        LightLinkDeviceColourSceneController    = 0x8010,
        LightLinkDeviceNonColourController      = 0x8020,
        LightLinkDeviceNonColourSceneController = 0x8030,
        LightLinkDeviceControlBridge            = 0x8040,
        LightLinkDeviceOnOffSensor              = 0x8050
    };
    Q_ENUM(LightLinkDevice)


    enum HomeAutomationDevice {
        // Generic devices
        HomeAutomationDeviceOnOffSwitch         = 0x0000,
        HomeAutomationDeviceOnOffOutput         = 0x0002,
        HomeAutomationDeviceRemoteControl       = 0x0006,
        HomeAutomationDeviceDoorLock            = 0x000A,
        HomeAutomationDeviceDoorLockController  = 0x000B,
        HomeAutomationDeviceSimpleSensor        = 0x000C,
        HomeAutomationDeviceSmartPlug           = 0x0051,

        // Lightning devices
        HomeAutomationDeviceOnOffLight           = 0x0100,
        HomeAutomationDeviceDimmableLight        = 0x0101,
        HomeAutomationDeviceDimmableColorLight   = 0x0102,
        HomeAutomationDeviceOnOffLightSwitch     = 0x0103,
        HomeAutomationDeviceDimmableSwitch       = 0x0104,
        HomeAutomationDeviceColourDimmerSwitch   = 0x0105,
        HomeAutomationDeviceLightSensor          = 0x0106,
        HomeAutomationDeviceOccupacySensor       = 0x0106,

        // Heating, Ventilation and Air-Conditioning (HVAC) devices
        HomeAutomationDeviceThermostat           = 0x0301,

        // Intruder Alarm System (IAS) devices
        HomeAutomationDeviceIsaControlEquipment             = 0x0400, // CIE
        HomeAutomationDeviceIsaAncillaryControlEquipment    = 0x0401, // ACE
        HomeAutomationDeviceIsaZone                         = 0x0401,
        HomeAutomationDeviceIsaWarningDevice                = 0x0401  // WD
    };
    Q_ENUM(HomeAutomationDevice)

//    enum DeviceType {
//        DeviceTypeUnknown       = 0xFFFF, // Unknown type
//        DeviceTypeBasic         = 0x0002, // Gateway
//        DeviceTypeGateway       = 0x0002, // Gateway
//        DeviceTypeSimpleSensor  = 0x000C, // ZHA Simple Sensor
//        DeviceTypeSmartPlug     = 0x0051, // ZHA Smart Plug
//        DeviceTypeControlBridge = 0x0840, // Control Bridge
//        DeviceTypeLampOnOff     = 0x0000, // ZLL on/off lamp
//        DeviceTypeLampDimm      = 0x0100, // ZLL mono lamp

//#define SIMPLE_DESCR_LAMP_DIMM_ZLL     0x0100   // ZLL mono lamp
//#define SIMPLE_DESCR_LAMP_ONOFF        0x0100   // ZHA on/off lamp
//#define SIMPLE_DESCR_LAMP_DIMM         0x0101   // ZHA dimmable lamp
//#define SIMPLE_DESCR_LAMP_COLOUR       0x0102   // ZHA dimmable colour lamp
//#define SIMPLE_DESCR_LAMP_CCTW         0x01FF   // ZHA / ZLL CCTW lamp
//#define SIMPLE_DESCR_LAMP_COLOUR_DIMM  0x0200   // ZLL dimmable colour lamp
//#define SIMPLE_DESCR_LAMP_COLOUR_EXT   0x0210   // ZLL extended colour lamp
//#define SIMPLE_DESCR_LAMP_COLOUR_TEMP  0x0220   // ZLL colour temperature lamp
//#define SIMPLE_DESCR_HVAC_HC_UNIT      0x0300   // ZHA HVAC HC Unit (HeatingManager)
//#define SIMPLE_DESCR_THERMOSTAT        0x0301   // ZHA Thermostat
//#define SIMPLE_DESCR_HVAC_PUMP         0x0303   // ZHA NVAC Pump
//#define SIMPLE_DESCR_SWITCH_ONOFF      0x0103   // ZHA On/Off Switch
//#define SIMPLE_DESCR_SWITCH_DIMM       0x0104   // ZHA Dimm Switch
//#define SIMPLE_DESCR_SWITCH_COLL_DIMM  0x0105   // ZHA Color Dimm Switch
//#define SIMPLE_DESCR_LIGHT_SENSOR      0x0106   // ZHA Light sensor
//#define SIMPLE_DESCR_SMOKE_SENSOR      0x0012   // CES - TriTech CO/smoke sensor
//#define SIMPLE_DESCR_WINDOW_SENSOR     0x0014   // CES - window sensor
//#define SIMPLE_DESCR_OCCUPANCY_SENSOR  0x0107   // ZH/ZLO - Occupancy Sensor
//    };

};

#endif // ZIGBEE_H
