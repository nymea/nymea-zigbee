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

#ifndef ZIGBEECLUSTERDOORLOCK_H
#define ZIGBEECLUSTERDOORLOCK_H

#include <QObject>
#include "zcl/zigbeecluster.h"
#include "zcl/zigbeeclusterreply.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterDoorLock : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Attribute {
        // Basic information
        AttributeLockState = 0x0000, // M
        AttributeLockType = 0x0001,  // M
        AttributeActuatorEnabled = 0x0002,  // M
        AttributeDoorState = 0x0003,
        AttributeDoorOpenEvents = 0x0004,
        AttributeDoorClosedEvents = 0x0005,
        AttributeOpenPeriod = 0x0006,

        // User, Pin, Schedule, Log information
        AttributeNumberOfLogRecordsSupported = 0x0010,
        AttributeNumberOfTotalUsersSupported = 0x0011,
        AttributeNumberOfPinUsersSupported = 0x0012,
        AttributeNumberOfRfidUsersSupported = 0x0013,
        AttributeNumberOfWeekDaySchedulesSupportedPerUser = 0x0014,
        AttributeNumberOfYearDaySchedulesSupportedPerUser = 0x0015,
        AttributeNumberOfHolidaySchedulesSupported = 0x0016,
        AttributeMaxPinCodeLength = 0x0017,
        AttributeMinPinCodeLenght = 0x0018,
        AttributeMaxRfidCodeLength = 0x0019,
        AttributeMinRfidCodeLength = 0x001a,

        // Operational settings
        AttributeEnableLogging = 0x0020,
        AttributeLanguage = 0x0021,
        AttributeLedSettings = 0x0022,
        AttributeAutoRelockTime = 0x0023,
        AttributeSoundVolume = 0x0024,
        AttributeOperatingMode = 0x0025,
        AttributeSupportedOperatingModes = 0x0026,
        AttributeDefaultConfigurationRegister = 0x0027,
        AttributeEnableLocalProgramming = 0x0028,
        AttributeEnableOneTouchLocking = 0x0029,
        AttributeEnableInsideStatusLed = 0x002a,
        AttributeEnablePrivacyModeButton = 0x002b,

        // Security settings
        AttributeWrongCodeEntryLimit = 0x0030,
        AttributeUserCodeTemporaryDisableTime = 0x0031,
        AttributeSendPinOverTheAir = 0x0032,
        AttributeRequirePinForRfOperation = 0x0033,
        AttributeZigbeeSecurityLevel = 0x0034,

        // Alarm and event masks
        AttributeAlarmMask = 0x0040,
        AttributeKeypadOperationEventMask = 0x0041,
        AttributeRfOperationEventMask = 0x0042,
        AttributeManualOperationEventMask = 0x0043,
        AttributeRfidOperationEventMask = 0x0044,
        AttributeKeypadProgrammingEventMask = 0x0045,
        AttributeRfProgrammingEventMask = 0x0046,
        AttributeRfidProgrammingEventMask = 0x0047
    };
    Q_ENUM(Attribute)

    enum Command {
        CommandLockDoor = 0x00, // M
        CommandUnlockDoor = 0x01, // M
        CommandToggle = 0x02,
        CommandUnlockWithTimeout = 0x03,
        CommandGetLogRecord = 0x04,
        CommandSetPinCode = 0x05,
        CommandGetPinCode = 0x06,
        CommandClearPinCode = 0x07,
        CommandClearAllPinCodes = 0x08,
        CommandSetUserStatus = 0x09,
        CommandGetUserStatus = 0x0a,
        CommandSetWeekdaySchedule = 0x0b,
        CommandGetWeekdaySchedule = 0x0c,
        CommandClearWeekdaySchedule = 0x0d,
        CommandSetYearDaySchedule = 0x0e,
        CommandGetYearDaySchedule = 0x0f,
        CommandClearYearDaySchedule = 0x10,
        CommandSetHolidaySchedule = 0x11,
        CommandGetHolidaySchedule = 0x12,
        CommandClearHolidaySchedule = 0x13,
        CommandSetUserType = 0x14,
        CommandGetUserType = 0x15,
        CommandSetRfidCode = 0x16,
        CommandGetRfidCode = 0x17,
        CommandClearRfidCode = 0x18,
        CommandClearAllRfidCodes = 0x19
    };
    Q_ENUM(Command)

    // AttributeLockState (0x0000)
    enum LockState {
        LockStateNotFullyLocked = 0x00,
        LockStateLocked = 0x01,
        LockStateUnlocked = 0x02,
        LockStateUndefined = 0xff
    };
    Q_ENUM(LockState)

    // AttributeLockType (0x0001)
    enum LockType {
        LockTypeDeadBolt = 0x00,
        LockTypeMagnetic = 0x01,
        LockTypeOther = 0x02,
        LockTypeMortise = 0x03,
        LockTypeRim = 0x04,
        LockTypeLatchBolt = 0x05,
        LockTypeCylindricalLock = 0x06,
        LockTypeTubularLock = 0x07,
        LockTypeInterconnectedLock = 0x08,
        LockTypeDeadLatch = 0x09,
        LockTypeDoorFurniture = 0x0a
    };
    Q_ENUM(LockType)

    // AttributeDoorState (0x0003)
    enum DoorState {
        DoorStateOpen = 0x00,
        DoorStateColsed = 0x01,
        DoorStateErrorJammed = 0x02,
        DoorStateErrorForcedOpen = 0x03,
        DoorStateErrorUnspecified = 0x04,
        DoorStateUndefined = 0xff
    };
    Q_ENUM(DoorState)

    // AttributeOperatingMode (0x0025)
    enum OperationMode {
        OperationModeNormal = 0x00,
        OperationModeVacation = 0x01,
        OperationModePrivacy = 0x02,
        OperationModeNoRfLockUnlock = 0x03,
        OperationModePassage = 0x04
    };
    Q_ENUM(OperationMode)

    // AttributeSupportedOperatingModes (0x0026)
    enum SupportedOperationMode {
        SupportedOperationModeNormal = 0x0000,
        SupportedOperationModeVacation = 0x0001,
        SupportedOperationModePrivacy = 0x0002,
        SupportedOperationModeNoRfLockUnlock = 0x0004,
        SupportedOperationModePassage = 0x0008
    };
    Q_ENUM(SupportedOperationMode)
    Q_DECLARE_FLAGS(SupportedOperationModes, SupportedOperationMode)

    // AttributeLedSettings (0x0022)
    enum LedSettings {
        LedSettingsNeverUserLedSignalisation = 0x00,
        LedSettingsUseLedSignalisationExceptAccessAllowedEvents = 0x01,
        LedSettingsUseLedSignalisationAllEvents = 0x02
    };
    Q_ENUM(LedSettings)

    // AttributeSoundVolume (0x0024)
    enum SoundVolume {
        SoundVolumeSilentMode = 0x00,
        SoundVolumeLowVolume = 0x01,
        SoundVolumeHighVolume = 0x02
    };
    Q_ENUM(SoundVolume)

    explicit ZigbeeClusterDoorLock(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    // Note: with HA 1.2, this commands have optional a RFID code or PIN code
    ZigbeeClusterReply *lockDoor(const QByteArray code = QByteArray());
    ZigbeeClusterReply *unlockDoor(const QByteArray code = QByteArray());
    ZigbeeClusterReply *toggle(const QByteArray code = QByteArray());
    ZigbeeClusterReply *unlockDoorWithTimeout(quint16 timeoutSeconds, const QByteArray code = QByteArray());

    // TODO: rest of the commands

    LockState lockState() const;
    DoorState doorState() const;

private:
    LockState m_lockState = LockStateUndefined;
    DoorState m_doorState = DoorStateUndefined;

    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

protected:
    void processDataIndication(ZigbeeClusterLibrary::Frame frame) override;

signals:
    void lockStateChanged(LockState lockState);
    void doorStateChanged(DoorState doorState);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ZigbeeClusterDoorLock::SupportedOperationModes)


#endif // ZIGBEECLUSTERDOORLOCK_H
