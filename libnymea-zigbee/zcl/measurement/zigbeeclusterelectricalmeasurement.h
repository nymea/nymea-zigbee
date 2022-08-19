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

#ifndef ZIGBEECLUSTERELECTRICALMEASUREMENT_H
#define ZIGBEECLUSTERELECTRICALMEASUREMENT_H

#include <QObject>
#include "zcl/zigbeecluster.h"
#include "zcl/zigbeeclusterreply.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterElectricalMeasurement : public ZigbeeCluster
{
    Q_OBJECT
    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:

    enum Attribute {
        AttributeBasicInformationMeasurementType = 0x0000,

        AttributeDCMeasurementAttributeDCVoltage = 0x0100,
        AttributeDCMeasurementAttributeDCVoltageMin = 0x0101,
        AttributeDCMeasurementAttributeDCVoltageMax = 0x0102,
        AttributeDCMeasurementAttributeDCCurrent = 0x0103,
        AttributeDCMeasurementAttributeDCCurrentMin = 0x0104,
        AttributeDCMeasurementAttributeDCCurrentMax = 0x0105,
        AttributeDCMeasurementAttributeDCPower = 0x0106,
        AttributeDCMeasurementAttributeDCPowerMin = 0x0107,
        AttributeDCMeasurementAttributeDCPowerMax = 0x0108,

        AttributeDCFormattingDCVoltageMultiplier = 0x0200,
        AttributeDCFormattingDCVoltageDivisor = 0x0201,
        AttributeDCFormattingDCCurrentMultiplier = 0x0202,
        AttributeDCFormattingDCCurrentDivisor = 0x0203,
        AttributeDCFormattingDCPowerMultiplier = 0x0204,
        AttributeDCFormattingDCPowerDivisor = 0x0205,

        AttributeACTotalMeasurementACFrequency = 0x0300,
        AttributeACTotalMeasurementACFrequencyMin = 0x0301,
        AttributeACTotalMeasurementACFrequencyMax = 0x0302,
        AttributeACTotalMeasurementNeutralCurrent = 0x0303,
        AttributeACTotalMeasurementTotalActivePower = 0x0304,
        AttributeACTotalMeasurementTotalReactivePower = 0x0305,
        AttributeACTotalMeasurementTotalApparentPower = 0x0306,
        AttributeACTotalMeasurementMeasured1stHarmonicCurrent = 0x0307,
        AttributeACTotalMeasurementMeasured3stHarmonicCurrent = 0x0308,
        AttributeACTotalMeasurementMeasured5stHarmonicCurrent = 0x0309,
        AttributeACTotalMeasurementMeasured7stHarmonicCurrent = 0x030A,
        AttributeACTotalMeasurementMeasured9stHarmonicCurrent = 0x030B,
        AttributeACTotalMeasurementMeasured11stHarmonicCurrent = 0x030C,
        AttributeACTotalMeasurementMeasuredPhase1stHarmonicCurrent = 0x030D,
        AttributeACTotalMeasurementMeasuredPhase3stHarmonicCurrent = 0x030E,
        AttributeACTotalMeasurementMeasuredPhase5stHarmonicCurrent = 0x030F,
        AttributeACTotalMeasurementMeasuredPhase7stHarmonicCurrent = 0x0310,
        AttributeACTotalMeasurementMeasuredPhase9stHarmonicCurrent = 0x0311,
        AttributeACTotalMeasurementMeasuredPhase11stHarmonicCurrent = 0x0312,

        AttributeACTotalFormattingACFrequencyMultiplier = 0x0400,
        AttributeACTotalFormattingACFrequencyDivisor = 0x0401,
        AttributeACTotalFormattingPowerMultiplier = 0x0402,
        AttributeACTotalFormattingPowerDivisor = 0x0403,
        AttributeACTotalFormattingHarmonicCurrentMultiplier = 0x0404,
        AttributeACTotalFormattingPhaseHarmonicCurrentMultiplier = 0x0405,

        AttributeACPhaseAMeasurementReserved = 0x0500,
        AttributeACPhaseAMeasurementLineCurrent = 0x0501,
        AttributeACPhaseAMeasurementActiveCurrent = 0x0502,
        AttributeACPhaseAMeasurementReactiveCurrent = 0x0503,
        AttributeACPhaseAMeasurementRMSVoltage = 0x0505,
        AttributeACPhaseAMeasurementRMSVoltageMin = 0x0506,
        AttributeACPhaseAMeasurementRMSVoltageMax = 0x0507,
        AttributeACPhaseAMeasurementRMSCurrent = 0x0508,
        AttributeACPhaseAMeasurementRMSCurrentMin = 0x0509,
        AttributeACPhaseAMeasurementRMSCurrentMax = 0x050A,
        AttributeACPhaseAMeasurementActivePower = 0x050B,
        AttributeACPhaseAMeasurementActivePowerMin = 0x050C,
        AttributeACPhaseAMeasurementActivePowerMax = 0x050D,
        AttributeACPhaseAMeasurementReactivePower = 0x050E,
        AttributeACPhaseAMeasurementApparentPower = 0x050F,
        AttributeACPhaseAMeasurementPowerFactor = 0x0510,
        AttributeACPhaseAMeasurementAverageRMSVoltageMeasurementPeriod = 0x0511,
        AttributeACPhaseAMeasurementAverageRMSOverVoltageCounter = 0x0512,
        AttributeACPhaseAMeasurementAverageRMSUnderVoltageCounter = 0x0513,
        AttributeACPhaseAMeasurementRMSExtremeOverVoltagePeriod = 0x0514,
        AttributeACPhaseAMeasurementRMSExtremeUnderVoltagePeriod = 0x0515,
        AttributeACPhaseAMeasurementRMSVoltageSagPeriod = 0x0516,
        AttributeACPhaseAMeasurementRMSVoltageSwellPeriod = 0x0517,

        AttributeACFormattingACVoltageMultiplier = 0x0600,
        AttributeACFormattingACVoltageDivisor = 0x0601,
        AttributeACFormattingCurrentMultiplier = 0x0602,
        AttributeACFormattingCurrentDivisor = 0x0603,
        AttributeACFormattingPowerMultiplier = 0x0604,
        AttributeACFormattingPowerDivisor = 0x0605,

        AttributeDCManuufacturerThresholdAlarmDCOverloadAlarmsMask = 0x0700,
        AttributeDCManuufacturerThresholdAlarmDCVoltageOverload = 0x0701,
        AttributeDCManuufacturerThresholdAlarmDCCurrentOverload = 0x0702,

        AttributeACManufacturerThresholdAlarmACAlarmsMask = 0x0800,
        AttributeACManufacturerThresholdAlarmACVoltageOverload = 0x0801,
        AttributeACManufacturerThresholdAlarmACCurrentOverload = 0x0802,
        AttributeACManufacturerThresholdAlarmACActivePowerOverload = 0x0803,
        AttributeACManufacturerThresholdAlarmACReactivePowerOverload = 0x0804,
        AttributeACManufacturerThresholdAlarmAverageRMSOverVoltage = 0x0805,
        AttributeACManufacturerThresholdAlarmAverageRMSUnderVoltage = 0x0806,
        AttributeACManufacturerThresholdAlarmRMSExtremeOverVoltage = 0x0807,
        AttributeACManufacturerThresholdAlarmRMSExtremeUnderVoltage = 0x0808,
        AttributeACManufacturerThresholdAlarmRMSVoltageSag = 0x0809,
        AttributeACManufacturerThresholdAlarmRMSVoltageSwell = 0x080A,

        AttributeACPhaseBMeasurementLineCurrent = 0x0901,
        AttributeACPhaseBMeasurementActiveCurrent = 0x0902,
        AttributeACPhaseBMeasurementReactiveCurrent = 0x0903,
        AttributeACPhaseBMeasurementRMSVoltage = 0x0905,
        AttributeACPhaseBMeasurementRMSVoltageMin = 0x0906,
        AttributeACPhaseBMeasurementRMSVoltageMax = 0x0907,
        AttributeACPhaseBMeasurementRMSCurrent = 0x0908,
        AttributeACPhaseBMeasurementRMSCurrentMin = 0x0909,
        AttributeACPhaseBMeasurementRMSCurrentMax = 0x090A,
        AttributeACPhaseBMeasurementActivePower = 0x090B,
        AttributeACPhaseBMeasurementActivePowerMin = 0x090C,
        AttributeACPhaseBMeasurementActivePowerMax = 0x090D,
        AttributeACPhaseBMeasurementReactivePower = 0x090E,
        AttributeACPhaseBMeasurementApparentPower = 0x090F,
        AttributeACPhaseBMeasurementPowerFactor = 0x0910,
        AttributeACPhaseBMeasurementAverageRMSVoltageMeasurementPeriod = 0x0911,
        AttributeACPhaseBMeasurementAverageRMSOverVoltageCounter = 0x0912,
        AttributeACPhaseBMeasurementAverageRMSUnderVoltageCounter = 0x0913,
        AttributeACPhaseBMeasurementRMSExtremeOverVoltagePeriod = 0x0914,
        AttributeACPhaseBMeasurementRMSExtremeUnderVoltagePeriod = 0x0915,
        AttributeACPhaseBMeasurementRMSVoltageSagPeriod = 0x0916,
        AttributeACPhaseBMeasurementRMSVoltageSwellPeriod = 0x0917,

        AttributeACPhaseCMeasurementLineCurrent = 0x0A01,
        AttributeACPhaseCMeasurementActiveCurrent = 0x0A02,
        AttributeACPhaseCMeasurementReactiveCurrent = 0x0A03,
        AttributeACPhaseCMeasurementRMSVoltage = 0x0A05,
        AttributeACPhaseCMeasurementRMSVoltageMin = 0x0A06,
        AttributeACPhaseCMeasurementRMSVoltageMax = 0x0A07,
        AttributeACPhaseCMeasurementRMSCurrent = 0x0A08,
        AttributeACPhaseCMeasurementRMSCurrentMin = 0x0A09,
        AttributeACPhaseCMeasurementRMSCurrentMax = 0x0A0A,
        AttributeACPhaseCMeasurementActivePower = 0x0A0B,
        AttributeACPhaseCMeasurementActivePowerMin = 0x0A0C,
        AttributeACPhaseCMeasurementActivePowerMax = 0x0A0D,
        AttributeACPhaseCMeasurementReactivePower = 0x0A0E,
        AttributeACPhaseCMeasurementApparentPower = 0x0A0F,
        AttributeACPhaseCMeasurementPowerFactor = 0x0A10,
        AttributeACPhaseCMeasurementAverageRMSVoltageMeasurementPeriod = 0x0A11,
        AttributeACPhaseCMeasurementAverageRMSOverVoltageCounter = 0x0A12,
        AttributeACPhaseCMeasurementAverageRMSUnderVoltageCounter = 0x0A13,
        AttributeACPhaseCMeasurementRMSExtremeOverVoltagePeriod = 0x0A14,
        AttributeACPhaseCMeasurementRMSExtremeUnderVoltagePeriod = 0x0A15,
        AttributeACPhaseCMeasurementRMSVoltageSagPeriod = 0x0A16,
        AttributeACPhaseCMeasurementRMSVoltageSwellPeriod = 0x0A17

    };
    Q_ENUM(Attribute)


    enum MeasurementType {
        MeasurementTypeACActive = 0x0001,
        MeasurementTypeACReactive = 0x0002,
        MeasurementTypeACApparent = 0x0004,
        MeasurementTypePhaseA = 0x0008,
        MeasurementTypePhaseB = 0x0010,
        MeasurementTypePhaseC = 0x0020,
        MeasurementTypeDC = 0x0040,
        MeasurementTypeHarmonics = 0x0080,
        MeasurementTypePowerQuality = 0x0100
    };
    Q_DECLARE_FLAGS(MeasurementTypes, MeasurementType)
    Q_FLAG(MeasurementTypes)

    enum DCOverloadAlarm {
        DCOverloadAlarmVoltageOverload = 0x01,
        DCOverloadAlarmCurrentOverload = 0x02
    };
    Q_DECLARE_FLAGS(DCOverloadAlarmsMask, DCOverloadAlarm)
    Q_FLAG(DCOverloadAlarmsMask)

    enum ACAlarm {
        ACAlarmVoltageOverload = 0x0001,
        ACAlarmCurrentOverload = 0x0002,
        ACAlarmActivePowerOverload = 0x0004,
        ACAlarmReactivePowerOverload = 0x0008,
        ACAlarmAverageRMSOverVoltage = 0x0010,
        ACAlarmAverageRMSUnderVoltage = 0x0020,
        ACAlarmRMSExtremeOverVoltage = 0x0040,
        ACAlarmRMSExtremeUnderVoltage = 0x0080,
        ACAlarmRMSVoltageSag = 0x0100,
        ACAlarmRMSVoltageSwell = 0x0200
    };
    Q_DECLARE_FLAGS(ACAlarmsMask, ACAlarm)
    Q_FLAG(ACAlarmsMask)

    enum ProfileIntervalPeriod {
        ProfileIntervalPeriodDaily = 0x00,
        ProfileIntervalPeriod60Minutes = 0x01,
        ProfileIntervalPeriod30Minutes = 0x02,
        ProfileIntervalPeriod15Minutes = 0x03,
        ProfileIntervalPeriod10Minutes = 0x04,
        ProfileIntervalPeriod7p5Minutes = 0x05,
        ProfileIntervalPeriod5Minutes = 0x06,
        ProfileIntervalPeriod2p5Minutes = 0x07
    };
    Q_ENUM(ProfileIntervalPeriod)

    enum MeasurementStatus {
        MeasurementStatusSuccess = 0x00,
        MeasurementTypeAttributeProfileNotSupported = 0x01,
        MeasurementStatusInvalidStartTime = 0x02,
        MeasurementStatusMoreIntervalsRequestedThanCanBeReturned = 0x03,
        MeasurementTypeNoIntervalsAvailableForRequestedTime = 0x04
    };
    Q_ENUM(MeasurementStatus)

    enum ClientCommand {
        CommandGetProfileInfo = 0x00,
        CommandGetMeasurementProfileInfo = 0x01
    };
    Q_ENUM(ClientCommand)

    enum ServerCommand {
        CommandGetProfileInfoResponse = 0x00,
        CommandGetMeasurementProfileResponse = 0x01
    };
    Q_ENUM(ServerCommand)

    explicit ZigbeeClusterElectricalMeasurement(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    quint16 activePowerPhaseA() const;

signals:
    void activePowerPhaseAChanged(qint16 activePowerPhaseA);

    void getProfileInfoResponse(quint8 profileCount, ProfileIntervalPeriod profileIntervalPeriod, quint8 maxNumberOfIntervals, const QList<quint16> &attributes);
    void getMeasurementProfileInfoResponse(const QDateTime &startTime, MeasurementStatus status, ProfileIntervalPeriod profileIntervalPeriod, quint8 numberOfIntervals, quint8 attributeId, const QList<quint16> &values);

protected:
    void processDataIndication(ZigbeeClusterLibrary::Frame frame) override;

private:
    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

    qint16 m_activePowerPhaseA = 0;

};

#endif // ZIGBEECLUSTERELECTRICALMEASUREMENT_H
