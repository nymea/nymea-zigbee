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

#ifndef ZIGBEECLUSTERBASIC_H
#define ZIGBEECLUSTERBASIC_H

#include <QObject>

#include "zcl/zigbeecluster.h"

class ZigbeeClusterBasic : public ZigbeeCluster
{
    Q_OBJECT
public:

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

    enum Attribute {
        AttributeZclVersion = 0x0000, // Mandatory
        AttributeAppVersion = 0x0001,
        AttributeStackVersion = 0x0002,
        AttributeHardwareVersion = 0x0003,
        AttributeManufacturerName = 0x0004,
        AttributeModelIdentifier = 0x0005,
        AttributeDateCode = 0x0006, // ISO 8601 YYYYMMDD
        AttributePowerSource = 0x0007, // Mandatory
        AttributeLocationDescription = 0x0010,
        AttributePhysicalEnvironment = 0x0011,
        AttributeDeviceEnabled = 0x0012, // 0: disabled, 1: enabled
        AttributeAlarmMask = 0x0013,
        AttributeDisableLocalConfig = 0x0014,
        AttributeSwBuildId = 0x4000
    };
    Q_ENUM(Attribute)

    // Enum for AttributePowerSource(0x0007)
    enum AttributePowerSourceValue {
        AttributePowerSourceValueUnknown = 0x00,
        AttributePowerSourceValueMainsSinglePhase = 0x01,
        AttributePowerSourceValueMainsThreePhase = 0x02,
        AttributePowerSourceValueBattery = 0x03,
        AttributePowerSourceValueDcSource = 0x04,
        AttributePowerSourceValueEmergencyMainsConstantlyPowered = 0x05,
        AttributePowerSourceValueEmergencyMainsTransferSwitch = 0x06
    };
    Q_ENUM(AttributePowerSourceValue)

    // Enum for AttributePhysicalEnvironment(0x0011)
    enum PhysicalEnvironment {
        PhysicalEnvironmentUnspecifiedEnvironment = 0x00,
        PhysicalEnvironmentAtrium = 0x01,
        PhysicalEnvironmentBar = 0x02,
        PhysicalEnvironmentCourtyard = 0x03,
        PhysicalEnvironmentBathroom = 0x04,
        PhysicalEnvironmentBedroom = 0x05,
        PhysicalEnvironmentBilliardRoom = 0x06,
        PhysicalEnvironmentUtilityRoom = 0x07,
        PhysicalEnvironmentCellar = 0x08,
        PhysicalEnvironmentStorageCloset = 0x09,
        PhysicalEnvironmentTheater = 0x0a,
        PhysicalEnvironmentOffice = 0x0b,
        PhysicalEnvironmentDeck = 0x0c,
        PhysicalEnvironmentDen = 0x0d,
        PhysicalEnvironmentDiningRoom = 0x0e,
        PhysicalEnvironmentElectricalRoom = 0x0f,
        PhysicalEnvironmentElevator = 0x10,
        PhysicalEnvironmentEntry = 0x11,
        PhysicalEnvironmentFamilyRoom = 0x12,
        PhysicalEnvironmentMainFloor = 0x13,
        PhysicalEnvironmentUpstairs = 0x14,
        PhysicalEnvironmentDownstairs = 0x15,
        PhysicalEnvironmentBasement = 0x16,
        PhysicalEnvironmentGallery = 0x17,
        PhysicalEnvironmentGameRoom = 0x18,
        PhysicalEnvironmentGarage = 0x19,
        PhysicalEnvironmentGym = 0x1a,
        PhysicalEnvironmentHallway = 0x1b,
        PhysicalEnvironmentHouse = 0x1c,
        PhysicalEnvironmentKitchen = 0x1d,
        PhysicalEnvironmentLaundryRoom = 0x1e,
        PhysicalEnvironmentLibrary = 0x1f,
        PhysicalEnvironmentMasterBedRoom = 0x20,
        PhysicalEnvironmentMudRoom = 0x21,
        PhysicalEnvironmentNursery = 0x22,
        PhysicalEnvironmentPantry = 0x23,
        PhysicalEnvironmentOffice2 = 0x24,
        PhysicalEnvironmentOutside = 0x25,
        PhysicalEnvironmentPool = 0x26,
        PhysicalEnvironmentPorch = 0x27,
        PhysicalEnvironmentSewingRoom = 0x28,
        PhysicalEnvironmentSittingRoom = 0x29,
        PhysicalEnvironmentStairway = 0x2a,
        PhysicalEnvironmentYard = 0x2b,
        PhysicalEnvironmentAttic = 0x2c,
        PhysicalEnvironmentHotTub = 0x2d,
        PhysicalEnvironmentLivingRoom = 0x2e,
        PhysicalEnvironmentSauna = 0x2f,
        PhysicalEnvironmentShopWorkshop = 0x30,
        PhysicalEnvironmentGuestBedroom = 0x31,
        PhysicalEnvironmentGuestBath = 0x32,
        PhysicalEnvironmentPowderRoom = 0x33,
        PhysicalEnvironmentBackYard = 0x34,
        PhysicalEnvironmentFrontYard = 0x35,
        PhysicalEnvironmentPatio = 0x36,
        PhysicalEnvironmentDriveway = 0x37,
        PhysicalEnvironmentSunRoom = 0x38,
        PhysicalEnvironmentGrandRoom = 0x39,
        PhysicalEnvironmentSpa = 0x3a,
        PhysicalEnvironmentWhirlpool = 0x3b,
        PhysicalEnvironmentShed = 0x3c,
        PhysicalEnvironmentEquipmentStorage = 0x3d,
        PhysicalEnvironmentHobbyCraftRoom = 0x3e,
        PhysicalEnvironmentFountain = 0x3f,
        PhysicalEnvironmentPond = 0x40,
        PhysicalEnvironmentReceptionRoom = 0x41,
        PhysicalEnvironmentBreakfastRoom = 0x42,
        PhysicalEnvironmentNook = 0x43,
        PhysicalEnvironmentGarden = 0x44,
        PhysicalEnvironmentBalcony = 0x45,
        PhysicalEnvironmentPanicRoom = 0x46,
        PhysicalEnvironmentTerrace = 0x47,
        PhysicalEnvironmentRoof = 0x48,
        PhysicalEnvironmentToilet = 0x49,
        PhysicalEnvironmentToiletMain = 0x4a,
        PhysicalEnvironmentOutsideToilet = 0x4b,
        PhysicalEnvironmentShowerRoom = 0x4c,
        PhysicalEnvironmentStudy = 0x4d,
        PhysicalEnvironmentFrontGarden = 0x4e,
        PhysicalEnvironmentBackGarden = 0x4f,
        PhysicalEnvironmentKettle = 0x50,
        PhysicalEnvironmentTelevision = 0x51,
        PhysicalEnvironmentStove = 0x52,
        PhysicalEnvironmentMicrowave = 0x53,
        PhysicalEnvironmentToaster = 0x54,
        PhysicalEnvironmentVacuum = 0x55,
        PhysicalEnvironmentAppliance = 0x56,
        PhysicalEnvironmentFrontDoor = 0x57,
        PhysicalEnvironmentBackDoor = 0x58,
        PhysicalEnvironmentFridgeDoor = 0x59,
        PhysicalEnvironmentMedicationCabinetDoor = 0x60,
        PhysicalEnvironmentWardrobeDoor = 0x61,
        PhysicalEnvironmentFrontCupboardDoor = 0x62,
        PhysicalEnvironmentOtherDoor = 0x63,
        PhysicalEnvironmentWaitingRoom = 0x64,
        PhysicalEnvironmentTriageRoom = 0x65,
        PhysicalEnvironmentDoctorsOffice = 0x66,
        PhysicalEnvironmentPatientsPrivateRoom = 0x67,
        PhysicalEnvironmentConsultationRoom = 0x68,
        PhysicalEnvironmentNurseStation = 0x69,
        PhysicalEnvironmentWard = 0x6a,
        PhysicalEnvironmentCorridor = 0x6b,
        PhysicalEnvironmentOperatingTheatre = 0x6c,
        PhysicalEnvironmentDentalSurgeryRoom = 0x6d,
        PhysicalEnvironmentMedicalImagingRoom = 0x6e,
        PhysicalEnvironmentDecontaminationRoom = 0x6f,
        PhysicalEnvironmentUnknownEnvironment = 0xff
    };
    Q_ENUM(PhysicalEnvironment)

    // Enum for AttributeAlarmMask(0x0013)
    enum AlarmMask {
        AlarmMaskGeneralHardwareFault = 0x01,
        AlarmMaskGeneralSoftwareFault = 0x02
    };
    Q_ENUM(AlarmMask)
    Q_DECLARE_FLAGS(AlarmMasks, AlarmMask)

    enum DiableLocalConfig {
        DiableLocalConfigReset = 0x01,
        DiableLocalConfigDeviceConfiguration = 0x02
    };
    Q_ENUM(DiableLocalConfig)
    Q_DECLARE_FLAGS(DiableLocalConfigs, DiableLocalConfig)

    explicit ZigbeeClusterBasic(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

private:
    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(ZigbeeClusterBasic::AlarmMasks)
Q_DECLARE_OPERATORS_FOR_FLAGS(ZigbeeClusterBasic::DiableLocalConfigs)

#endif // ZIGBEECLUSTERBASIC_H
