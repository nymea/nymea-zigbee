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

#ifndef TI_H
#define TI_H

#include <QObject>

#define MT_RPC_DATA_MAX 250

class Ti
{
    Q_GADGET

public:
    enum ZnpVersion {
        zStack12 = 0x00,
        zStack3x0 = 0x01,
        zStack30x = 0x02
    };
    Q_ENUM(ZnpVersion)

    enum ResetType {
        ResetTypeHard = 0x00,
        ResetTypeSoft = 0x01
    };
    Q_ENUM(ResetType)

    enum ResetReason {
        ResetReasonPowerUp = 0x00,
        ResetReasonExternal = 0x01,
        ResetReasonWatchDog = 0x02
    };
    Q_ENUM(ResetReason)

    enum StartupMode {
        StartupModeNormal = 0x00,
        StartupModeClean = 0x03
    };
    Q_ENUM(StartupMode)

    enum DeviceLogicalType {
        DeviceLogicalTypeCoordinator = 0x00,
        DeviceLogicalTypeRouter = 0x01,
        DeviceLogicalTypeEndDevice = 0x02,
        DeviceLogicalTypeComplexDescriptorAvailable = 0x04,
        DeviceLogicalTypeUserDescriptorAvailable = 0x08,
        DeviceLogicalTypeReserved1 = 0x10,
        DeviceLogicalTypeReserved2 = 0x20,
        DeviceLogicalTypeReserved3 = 0x40,
        DeviceLogicalTypeReserved4 = 0x80,
    };
    Q_ENUM(DeviceLogicalType)

    enum ControllerCapability {
        ControllerCapabilityNone = 0x0000,
        ControllerCapabilitySys = 0x0001,
        ControllerCapabilityMAC = 0x0002,
        ControllerCapabilityNWK = 0x0004,
        ControllerCapabilityAF  = 0x0008,
        ControllerCapabilityZDO = 0x0010,
        ControllerCapabilitySAPI = 0x0020,
        ControllerCapabilityUtil = 0x0040,
        ControllerCapabilityDebug = 0x0080,
        ControllerCapabilityApp = 0x0100,
        ControllerCapabilityZOAD = 0x1000
    };
    Q_DECLARE_FLAGS(ControllerCapabilities, ControllerCapability)
    Q_FLAG(ControllerCapabilities)

    enum StatusCode {
        StatusCodeSuccess = 0x00,
        StatusCodeFailure = 0x01,
        StatusCodeBusy = 0x02,
        StatusCodeTimeout = 0x03,
        StatusCodeUnsupported = 0x04,
        StatusCodeError = 0x05,
        StatusCodeNoNetwork = 0x06,
        StatusCodeInvalidValue = 0x07
    };
    Q_ENUM(StatusCode)

    enum CommandType {
        CommandTypePoll = 0x00,
        CommandTypeSReq = 0x20,
        CommandTypeAReq = 0x40,
        CommandTypeSRsp = 0x60,
    };
    Q_ENUM(CommandType)

    enum SubSystem {
        SubSystemReserved = 0x00,
        SubSystemSys = 0x01,
        SubSystemMAC = 0x02,
        SubSystemNwk = 0x03,
        SubSystemAF = 0x04,
        SubSystemZDO = 0x05,
        SubSystemSAPI = 0x06,
        SubSystemUtil = 0x07,
        SubSystemDebug = 0x08,
        SubSystemApp = 0x09,
        SubSystemAppCnf = 0x0F,
        SubSystemGreenPower = 0x15,
    };
    Q_ENUM(SubSystem)

    enum SYSCommand {
        SYSCommandResetReq = 0x00,
        SYSCommandPing = 0x01,
        SYSCommandVersion = 0x02,
        SYSCommandSetExtAddress = 0x03,
        SYSCommandGetExtAddress = 0x04,
        SYSCommandRamRead = 0x05,
        SYSCommandRamWrite = 0x06,
        SYSCommandOsalNvItemInit = 0x07,
        SYSCommandOsalNvRead = 0x08,
        SYSCommandOsaNvWrite = 0x09,
        SYSCommandOsalStartTimer = 0x0A,
        SYSCommandOsalStopTimer = 0x0B,
        SYSCommandOsalRandom = 0x0C,
        SYSCommandAdcRead = 0x0D,
        SYSCommandGpio = 0x0E,
        SYSCommandStackTune = 0x0F,
        SYSCommandSetTime = 0x10,
        SYSCommandGetTime = 0x11,
        SYSCommandOsalNvDelete = 0x12,
        SYSCommandOsalNvLength = 0x13,
        SYSCommandSetTxPower = 0x14,
        SYSCommandJammerParameters = 0x15,
        SYSCommandSnifferParameters = 016,
        SYSCommandZdiagsInitStats = 0x17,
        SYSCommandZdiagsClearStats = 0x18,
        SYSCommandZdiagsGetStats = 0x19,
        SYSCommandZdiagsRestoreStatsNv = 0x1A,
        SYSCommandZdiagsSaveStatsToNv = 0x1B,
        SYSCommandOsalNvReadExt = 0x1C,
        SYSCommandOsalNvWriteExt = 0x01D,

        SYSCommandNvCreate = 0x30,
        SYSCommandNvDelete = 0x31,
        SYSCommandNvLength = 0x32,
        SYSCommandNvRead = 0x33,
        SYSCommandNvWrite = 0x34,
        SYSCommandNvUpdate = 0x35,
        SYSCommandNvCompact = 0x36,

        SYSCommandResetInd = 0x80,
        SYSCommandOsalTimerExpired = 0x81,
        SYSCommandJammerInd = 0x82
    };
    Q_ENUM(SYSCommand)

    enum MACCommand {
        MACCommandResetReq = 0x01,
        MACCommandInit = 0x02,
        MACCommandStartReq = 0x03,
        MACCommandSyncReq = 0x04,
        MACCommandDataReq = 0x05,
        MACCommandAssociateReq = 0x06,
        MACCommandDisassociateReq = 0x07,
        MACCommandGetReq = 0x08,
        MACCommandSetReq = 0x09,

        MACCommandScanReq = 0x0C,
        MACCommandPollReq = 0x0D,
        MACCommandPurgeReq = 0x0E,
        MACCommandSetRxGainReq = 0x0F,

        MACCommandSecurityGetReq = 0x30,
        MACCommandSecuritySetReq = 0x31,

        MACCommandAssociateRsp = 0x50,
        MACCommandOrphanRsp = 0x51,

        MACCommandSyncLossInd = 0x80,
        MACCommandAssociateInd = 0x81,
        MACCommandAssociateCnf = 0x82,
        MACCommandBeaconNotifyInd = 0x83,
        MACCommandDataCnf = 0x84,
        MACCommandDataInd = 0x85,
        MACCommandDisassociateInd = 0x86,
        MACCommandDisassociateCnf = 0x87,

        MACCommandOrphanInd = 0x8A,
        MACCommandPollCnf = 0x8B,
        MACCommandScanCnf = 0x8C,
        MACCommandCommStatusInd = 0x8D,
        MACCommandStartCnf = 0x8E,
        MACCommandRxEnableCnf = 0x8F,
        MACCommandPurgeCnf = 0x90
    };
    Q_ENUM(MACCommand)

    enum AFCommand {
        AFCommandRegister = 0x00,
        AFCommandDataRequest = 0x01,
        AFCommandDataRequestExt = 0x02,
        AFCommandDataRequestSrcRtg = 0x03,
        AFCommandDelete = 0x04,

        AFCommandInterPanCtl = 0x10,
        AFCommandDataStore = 0x11,
        AFCommandDataRetrieve = 0x12,
        AFCommandApsfConfigSet = 0x13,
        AFCommandApsfConfigGet = 0x14,

        AFCommandDataConfirm = 0x80,
        AFCommandIncomingMsg = 0x81,
        AFCommandIncomingMsgExt = 0x82,
        AFCommandReflectError = 0x83,
    };
    Q_ENUM(AFCommand)

    enum ZDOCommand {
        ZDOCommandNwwAddrReq = 0x00,
        ZDOCommandIeeeAddrReq = 0x01,
        ZDOCommandNodeDescReq = 0x02,
        ZDOCommandPowerDescReq = 0x03,
        ZDOCommandSimpleDescReq = 0x04,
        ZDOCommandActiveEpReq = 0x05,
        ZDOCommandMatchDescReq = 0x06,
        ZDOCommandComplexDescReq = 0x07,
        ZDOCommandUserDescReq = 0x08,

        ZDOCommandEndDeviceAnnce = 0x0A,
        ZDOCommandUserDescSet = 0x0B,
        ZDOCommandServerDiscReq = 0x0C,
        ZDOCommandEndDeviceTimeoutReq = 0x0D,

        ZDOCommandEndDeviceBindReq = 0x20,
        ZDOCommandBindReq = 0x21,
        ZDOCommandUnbindReq = 0x22,
        ZDOCommandSetLinkKey = 0x23,
        ZDOCommandRemoveLinkKey = 0x24,
        ZDOCommandGetLinkKey = 0x25,
        ZDOCommandNwkDiscoveryReq = 0x26,
        ZDOCommandJoinReq = 0x27,
        ZDOCommandSendData = 0x28,
        ZDOCommandNwkAddrOfInterestReq = 0x26,

        ZDOCommandMgmtNwkDiscReq = 0x30,
        ZDOCommandMgmtLqiReq = 0x31,
        ZDOCommandMgmtRtgReq = 0x32,
        ZDOCommandMgmtBindReq = 0x33,
        ZDOCommandMgmtLeaveReq = 0x34,
        ZDOCommandMgmtDirectJoinReq = 0x35,
        ZDOCommandMgmtPermitJoinReq = 0x36,
        ZDOCommandMgmtNwkUpdateReq = 0x37,

        ZDOCommandMsgCbRegister = 0x3E,
        ZDOCommandMsgCbRemove = 0x3F,
        ZDOCommandStartupFromApp = 0x40,
        ZDOCommandAutoFindDestination = 0x41,
        ZDOCommandSecAddLinkKey = 0x42,
        ZDOCommandSecEntryLookupExt = 0x43,
        ZDOCommandSecDeviceRemove = 0x044,
        ZDOCommandExtRouteDisc = 0x45,
        ZDOCommandExtRouteCheck = 0x46,
        ZDOCommandExtRemoveGroup = 0x47,
        ZDOCommandExtRemoveAllGroup = 0x48,
        ZDOCommandExtFindAllGroupsEndpoint = 0x49,
        ZDOCommandExtFindGroup = 0x4A,
        ZDOCommandExtAddGroup = 0x4B,
        ZDOCommandExtCountAllGroups = 0xAC,
        ZDOCommandExtRxIdle = 0xAD,
        ZDOCommandExtUpdateNwkKey = 0xAE,
        ZDOCommandExtSwitchNwkKey = 0xAF,
        ZDOCommandExtNwkInfo = 0x50,
        ZDOCommandExtSecApsRemoveReq = 0x51,
        ZDOCommandForceContentratorChange = 0x52,
        ZDOCommandExtSetParams = 0x53,

        ZDOCommandNwkAddrRsp = 0x80,
        ZDOCommandNwkIeeeAddrRsp = 0x81,
        ZDOCommandNodeDescRsp = 0x82,
        ZDOCommandPowerDescRsp = 0x83,
        ZDOCommandSimpleDescRsp = 0x84,
        ZDOCommandActiveEpRsp = 0x85,
        ZDOCommandMatchDescRsp = 0x86,
        ZDOCommandComplexDescRsp = 0x87,
        ZDOCommandUserDescRsp = 0x88,
        ZDOCommandUserDescConf = 0x89,
        ZDOCommandServerDiscRsp = 0x8A,

        ZDOCommandEndDeviceBindRsp = 0xA0,
        ZDOCommandBindRsp = 0xA1,
        ZDOCommandUnbindRsp = 0xA2,

        ZDOCommandMgmtNwkDiscRsp = 0xB0,
        ZDOCommandMgmtLqiRsp = 0xB1,
        ZDOCommandMgmtRtgRsp = 0xB2,
        ZDOCommandMgmtBindRsp = 0xB3,
        ZDOCommandMgmtLeaveRsp = 0xB4,
        ZDOCommandMgmtDirectJoinRsp = 0xB5,
        ZDOCommandMgmtPermitJoinRsp = 0xB6,

        ZDOCommandMgmtNwkUpdateNotify = 0xB8,

        ZDOCommandStateChangeInd = 0xC0,
        ZDOCommandEndDeviceAnnceInd = 0xC1,
        ZDOCommandMatchNodeDscRsp = 0xC2,
        ZDOCommandStatusErrorRsp = 0xC3,
        ZDOCommandSrcRtgInd = 0xC4,
        ZDOCommandBeaconNotifyInd = 0xC5,
        ZDOCommandJoinCnf = 0xC6,
        ZDOCommandNwkDiscoveryCnf = 0xC7,
        ZDOCommandConcentratorIndCb = 0xC8,
        ZDOCommandLeaveInd = 0xC9,
        ZDOCommandTcDeviceInd = 0xCA,
        ZDOCommandPermitJoinInd = 0xCB,

        ZDOCommandSetRejoinParametersReq = 0xCC,
        ZDOCommandMsgCbIncoming = 0xFF
    };
    Q_ENUM(ZDOCommand)

    enum SAPICommand {
        SAPICommandStartRequest = 0x00,
        SAPICommandSystemReset = 0x09,
        SAPICommandBindDevice = 0x01,
        SAPICommandAllowBind = 0x02,
        SAPICommandSendDataRequest = 0x03,
        SAPICommandReadConfiguration = 0x04,
        SAPICommandWriteConfiguration = 0x05,
        SAPICommandGetDeviceInfo = 0x06,
        SAPICommandFindDeviceRequest = 0x07,
        SAPICommandPermitJoiningRequest = 0x08,

        SAPICommandStartConfirm = 0x80,
        SAPICommandBindConfirm = 0x81,
        SAPICommandAllowBindConfirm = 0x82,
        SAPICommandSendDataConfirm = 0x83,
        SAPICommandFindDeviceConfirm = 0x84,

        SAPICommandReceiveDataIndication = 0x87,
    };
    Q_ENUM(SAPICommand)

    enum UtilCommand {
        UtilCommandGetDeviceInfo = 0x00,
        UtilCommandGetNvInfo = 0x01,
        UtilCommandSetPanId = 0x02,
        UtilCommandSetChannels = 0x03,
        UtilCommandSetSecLevel = 0x04,
        UtilCommandSetPreCfgKey = 0x05,
        UtilCommandCallbackSubCmd = 0x06,
        UtilCommandKeyEvent = 0x07,
        UtilCommandTimeAlive = 0x09,
        UtilCommandLedControl = 0x0A,

        UtilCommandTestLoopback = 0x10,
        UtilCommandDataReq = 0x11,

        UtilCommandGpioSetDirection = 0x14,
        UtilCommandGpioRead = 0x15,
        UtilCommandGpioWrite = 0x16,

        UtilCommandSrcMatchEnable = 0x20,
        UtilCommandSrcMatchAddEntry = 0x21,
        UtilCommandSrcMatchDelEntry = 0x22,
        UtilCommandSrcMatchCheckSrcAddr = 0x23,
        UtilCommandSrcMatchAckAllPending = 0x24,
        UtilCommandSrcMatchCheckAllPending = 0x25,

        UtilCommandAddrMgrExtAddrLookup = 0x40,
        UtilCommandAddrMgrNwkAddrLookup = 0x41,

        UtilCommandApsmeLinkKeyDataGet = 0x44,
        UtilCommandApsmeLinkKeyNvIdGet = 0x45,

        UtilCommandAssocCount = 0x48,
        UtilCommandAssocFindDevice = 0x49,
        UtilCommandAssocGetWithAddress = 0x4A,
        UtilCommandApsmeRequestKeyCmd = 0x4B,
        UtilCommandSrngGen = 0x4C,
        UtilCommandBindAddKey = 0x4D,

        UtilCommandAssocRemove = 0x63,
        UtilCommandAssocAdd = 0x64,

        UtilCommandZclKeyEstInitEst = 0x80,
        UtilCommandZclKeyEstSign = 0x81,

        UtilCommandSyncReq = 0xE0,
        UtilCommandZclKeyEstablishInd = 0xE1,
    };
    Q_ENUM(UtilCommand)

    enum DebugCommand {
        DebugCommandSetThreshold = 0x00,
        DebugCommandMsg = 0x80,
    };
    Q_ENUM(DebugCommand)

    enum AppCommand {
        AppCommandMsg = 0x00,
        AppCommandUserTest = 0x01,

        AppCommandTllTlInd = 0x81,
    };
    Q_ENUM(AppCommand)

    enum AppCnfCommand {
        AppCnfCommandBdbStartCommissioning = 0x05,
        AppCnfCommandBdbSetChannel = 0x08,
        AppCnfCommandBdbSetTcRequireKeyExchange = 0x09,

        AppCnfCommandBdbCommissioningNotification = 0x80,

        AppCnfCommandSetNwkFrameCounter = 0xFF
    };
    Q_ENUM(AppCnfCommand)

    enum GreenPowerCommand {
        GreenPowerCommandSecReq = 0x03,
    };
    Q_ENUM(GreenPowerCommand)

    enum NvItemId {
        NvItemIdExtAddr = 0x01,
        NvItemIdBootCounter = 0x02,
        NvItemIdStartupOption = 0x03,
        NvItemIdStartDelay = 0x04,
        NvItemIdNIB = 0x21,
        NvItemIdDeviceList = 0x22,
        NvItemIdAddrMgr = 0x23,
        NvItemIdPollRate = 0x24,
        NvItemIdQueuedPollRate = 0x25,
        NvItemIdResponsePollRate = 0x26,
        NvItemIdRejoinPollRate = 0x27,
        NvItemIdDataRetries = 0x28,
        NvItemIdPollFailureRetries = 0x29,
        NvItemIdStackProfile = 0x2A,
        NvItemIdIndirectMsgTimeout = 0x2B,
        NvItemIdRouteExpiryTime = 0x2C,
        NvItemIdExtendedPanId = 0x2D,
        NvItemIdBcastRetries = 0x2E,
        NvItemIdPassiveAckTimeout = 0x2F,
        NvItemIdBcastDeliveryTime = 0x30,
        NvItemIdNwkMode = 0x31,
        NvItemIdConcentratorEnable = 0x32,
        NvItemIdConcentratorDiscovery = 0x33,
        NvItemIdConcentratorRadius = 0x34,

        NvItemIdConcentratorRC = 0x36,
        NvItemIdMwkMgrMode = 0x37,
        NvItemIdSrcRtgExpiryTime = 0x38,
        NvItemIdRouteDiscoveryTime = 0x39,
        NvItemIdNwkActiveKeyInfo = 0x3A,
        NvItemIdNwkAlternKeyInfo = 0x3B,
        NvItemIdRouterOffAssocCleanup = 0x3C,
        NvItemIdNwkLeaveReqAllowed = 0x3D,
        NvItemIdNwkChildAgeEnable = 0x3E,
        NvItemIdDeviceListKaTimeout = 0x3F,

        NvItemIdBindingTable = 0x41,
        NvItemIdGroupTable = 0x42,
        NvItemIdApsFrameRetries = 0x43,
        NvItemIdApsAckWaitDuration = 0x44,
        NvItemIdApsAckWaitMultiplier = 0x45,
        NvItemIdBindingTime = 0x46,
        NvItemIdApsUseExtPanId = 0x47,
        NvItemIdApsUseInsecureJoin = 0x48,
        NvItemIdCommisionedNwkAddr = 0x49,
        NvItemIdApsNonMemberRadious = 0x4B,
        NvItemIdApsLinkKeyTable = 0x4C,
        NvItemIdApsDuprejTimeoutInc = 0x4D,
        NvItemIdApsDuprejTimeoutCount = 0x4E,
        NvItemIdApsDuprejTableSize = 0x4F,
        NvItemIdDiagnosticStats = 0x50,
        NvItemIdBdbNodeIsOnANetwork = 0x55,
        NvItemIdSecurityLevel = 0x61,
        NvItemIdPreCfgKey = 0x62,
        NvItemIdPreCfgKeysEnable = 0x63,
        NvItemIdSecurityMode = 0x64,
        NvItemIdSecurePermitJoin = 0x65,
        NvItemIdApsLinkKeyType = 0x66,
        NvItemIdApsAllowR19Security = 0x67,
        NvItemIdImplicitCertificate = 0x69,
        NvItemIdDevicePrivateKey = 0x6A,
        NvItemIdCaPublicKey = 0x6B,
        NvItemIdKeMaxDevices = 0x6C,
        NvItemIdUseDefaultTclk = 0x6D,
        NvItemIdRngCounter = 0x6F,
        NvItemIdRandomSeed = 0x70,
        NvItemIdTrustcenterAddr = 0x71,
        NvItemIdLegacyNwkSecMaterialTableStart = 0x74, // Valid for <= Z-Stack 3.0.x
        NvItemIdExNwkSecMaterialTable = 0x07, // Valid for >= Z-Stack 3.x.0
        NvItemIdUserDesc = 0x81,
        NvItemIdNwkKey = 0x82,
        NvItemIdPanId = 0x83,
        NvItemIdChanList = 0x84,
        NvItemIdLeaveCtrl = 0x85,
        NvItemIdScanDuration = 0x86,
        NvItemIdLogicalType = 0x87,
        NvItemIdNwkMgrMinTx = 0x88,
        NvItemIdNwkMgrAddr = 0x89,

        NvItemIdZdoDirectCb = 0x8F,
        NvItemIdSceneTable = 0x91,
        NvItemIdMinFreeNwkAddr = 0x92,
        NvItemIdMaxFreeNwkAddr = 0x93,
        NvItemIdMinFreeGrpId = 0x94,
        NvItemIdMaxFreeGrpId = 0x95,
        NvItemIdMinGrpIds = 0x96,
        NvItemIdMaxGrpIds = 0x97,
        NvItemIdOtaBlockReqDelay = 0x98,
        NvItemIdSAPIEndpoint = 0xA1,
        NvItemIdSASShortAddr = 0xB1,
        NvItemIdSASExtPanId = 0xB2,
        NvItemIdSASPanId = 0xB3,
        NvItemIdSASChannelMask = 0xB4,
        NvItemIdSASProtocolVer = 0xB5,
        NvItemIdSASStackProfile = 0xB6,
        NvItemIdSASStartupCtrl = 0xB7,
        NvItemIdSASTcAddr = 0xC1,
        NvItemIdSASTcMasterKey = 0xC2,
        NvItemIdSASNwkKey = 0xC3,
        NvItemIdSASUseInsecJoin = 0xC4,
        NvItemIdSASPreCfgLinkKey = 0xC5,
        NvItemIdSASNwkKeySeqNum = 0xC6,
        NvItemIdSASNwkKeyType = 0xC7,
        NvItemIdSASNwkMgrAddr = 0xC8,
        NvItemIdSASCurrTcMasterKey = 0xD1,
        NvItemIdSASCurrNwkKey = 0xD2,
        NvItemIdSASCurrPreCfgLinkKey = 0xD3,

        NvItemIdTclkSeed = 0x101,
        NvItemIdLegacyTclkTableStart_12 = 0x101, // Keep it for Legacy 1.2 stack
        NvItemIdLegacyTclkTableStart = 0x111, // Valid for <= Z-Stack 3.0.x
        NvItemIdExTclkTable = 0x04, // Valid for >= Z-Stack 3.0.x
        NvItemIdApsLinkKeyDataStart = 0x201,
        NvItemIdApsLinkKeyDataEnd = 0x2FF,
        NvItemIdDuplicateBindingTable = 0x300,
        NvItemIdDuplicateDeviceList = 0x301,
        NvItemIdDuplicateDeviceListKaTimeout = 0x302,
        NvItemIdZnpHasConfiguredStack1 = 0xF00,
        NvItemIdZnpHasConfiguredStack3 = 0x60,
        NvItemIdZcdNvExApsKeyDataTable = 0x06,
        NvItemIdZcdNvExAddrMgr = 0x01
    };
    Q_ENUM(NvItemId)

    enum TxOption {
        TxOptionNone = 0x00,
        TxOptionsWildcardProfileId = 0x02,
        TxOptionPreprocess = 0x04,
        TxOptionLimitConcentrator = 0x08,
        TxOptionApsAck = 0x10,
        TxOptionSuppressRouteDiscovery = 0x20,
        TxOptionApsSecurity = 0x40,
        TxOptionSkipRouting = 0x80,
        TxOptionBroadCastEndpoint = 0xFF
    };
    Q_DECLARE_FLAGS(TxOptions, TxOption)
    Q_FLAG(TxOption)
};


#endif // TI_H
