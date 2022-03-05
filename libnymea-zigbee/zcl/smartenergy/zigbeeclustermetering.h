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

#ifndef ZIGBEECLUSTERMETERING_H
#define ZIGBEECLUSTERMETERING_H

#include <QObject>
#include "zcl/zigbeecluster.h"
#include "zcl/zigbeeclusterreply.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterMetering : public ZigbeeCluster
{
    Q_OBJECT
    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:
    enum Attribute {
        // Reading information set
        AttributeCurrentSummationDelivered = 0x0000,
        AttributeCurrentSummationReceived = 0x0001,
        AttributeCurrentMaxDemandDelivered = 0x0002,
        AttributeCurrentMaxDemandReceived = 0x0003,
        AttributeDFTSummation = 0x0004,
        AttributeDailyFreezeTime = 0x0005,
        AttributePowerFactor = 0x0006,
        AttributeReadingSnapShotTime = 0x0007,
        AttributeCurrentMaxDemandDeliveredTime = 0x0008,
        AttributeCurrentMaxDemandReceivedTime = 0x0009,
        AttributeDefaultUpdatePeriod = 0x000A,
        AttributeFastPollUpdatePeriod = 0x000B,
        AttributeCurrentBlockPeriodConsumptionDelivered = 0x000C,
        AttributeDailyConsumptionTarget = 0x000D,
        AttributeCurrentBlock = 0x000E,
        AttributeProfileIntervalPeriod = 0x000F,
        AttributeIntervalReadReportingPeriod = 0x0010,
        AttributePresetReadingTime = 0x0011,
        AttributeVolumePerReport = 0x0012,
        AttributeFlowRestriction = 0x0013,
        AttributeSupplyStatus = 0x0014,
        AttributeCurrentInletEnergyCarrierSummation = 0x0015,
        AttributeCurrentOutletEnergyCarrierSummation = 0x0016,
        AttributeInletTemperature = 0x0017,
        AttributeOutletTemperature = 0x0018,
        AttributeControlTemperature = 0x0019,
        AttributeCurrentInletEnergyCarrierDemand = 0x001A,
        AttributeCurrentOutletEnergyCarrierDemand = 0x001B,
        AttributePreviousBlockPeriodConsumptionDelivered = 0x001C,

        // Summation TOU information set
        AttributeCurrentTier1SummationDelivered = 0x0100,
        AttributeCurrentTier1SummationReceived = 0x0101,
        AttributeCurrentTier2SummationDelivered = 0x0102,
        AttributeCurrentTier2SummationReceived = 0x0103,
        AttributeCurrentTier3SummationDelivered = 0x0104,
        AttributeCurrentTier3SummationReceived = 0x0105,
        AttributeCurrentTier4SummationDelivered = 0x0106,
        AttributeCurrentTier4SummationReceived = 0x0107,
        AttributeCurrentTier5SummationDelivered = 0x0108,
        AttributeCurrentTier5SummationReceived = 0x0109,
        AttributeCurrentTier6SummationDelivered = 0x010A,
        AttributeCurrentTier6SummationReceived = 0x010B,
        AttributeCurrentTier7SummationDelivered = 0x010C,
        AttributeCurrentTier7SummationReceived = 0x010D,
        AttributeCurrentTier8SummationDelivered = 0x010E,
        AttributeCurrentTier8SummationReceived = 0x010F,
        AttributeCurrentTier9SummationDelivered = 0x0110,
        AttributeCurrentTier9SummationReceived = 0x0111,
        AttributeCurrentTier10SummationDelivered = 0x0112,
        AttributeCurrentTier10SummationReceived = 0x0113,
        AttributeCurrentTier11SummationDelivered = 0x0114,
        AttributeCurrentTier11SummationReceived = 0x0115,
        AttributeCurrentTier12SummationDelivered = 0x0116,
        AttributeCurrentTier12SummationReceived = 0x0117,
        AttributeCurrentTier13SummationDelivered = 0x0118,
        AttributeCurrentTier13SummationReceived = 0x0119,
        AttributeCurrentTier14SummationDelivered = 0x011A,
        AttributeCurrentTier14SummationReceived = 0x011B,
        AttributeCurrentTier15SummationDelivered = 0x011C,
        AttributeCurrentTier15SummationReceived = 0x011D,

        // Meter status attribute set
        AttributeStatus = 0x0200,
        AttributeRemainingBatteryLife = 0x0201,
        AttributeHoursInOperation = 0x0202,
        AttributeHoursInFault = 0x0203,

        // Formatting
        AttributeUnitofMeasure = 0x0300,
        AttributeMultiplier = 0x0301,
        AttributeDivisor = 0x0302,
        AttributeSummationFormatting = 0x0303,
        AttributeDemandFormatting = 0x0304,
        AttributeHistoricalConsumptionFormatting = 0x0305,
        AttributeMeteringDeviceType = 0x0306,
        AttributeSiteID = 0x0307,
        AttributeMeterSerialNumber = 0x0308,
        AttributeEnergyCarrierUnitOfMeasure = 0x0309,
        AttributeEnergyCarrierSummationFormatting = 0x030A,
        AttributeEnergyCarrierDemandFormatting = 0x030B,
        AttributeTemperatureUnitOfMeasure = 0x030B,
        AttributeTemperatureFormatting = 0x030C,

        // Historical consumption
        AttributeInstantaneousDemand = 0x0400,
        AttributeCurrentDayConsumptionDelivered = 0x0401,
        AttributeCurrentDayConsumptionReceived = 0x0402,
        AttributePreviousDayConsumptionDelivered = 0x0403,
        AttributePreviousDayConsumptionReceived = 0x0404,
        AttributeCurrentPartialProfileIntervalStartTimeDelivered = 0x0405,
        AttributeCurrentPartialProfileIntervalStartTimeReceived = 0x0406,
        AttributeCurrentPartialProfileIntervalValueDelivered = 0x0407,
        AttributeCurrentPartialProfileIntervalValueReceived = 0x0408,
        AttributeCurrentDayMaxPressure = 0x0409,
        AttributeCurrentDayMinPressure = 0x040A,
        AttributePreviousDayMaxPressure = 0x040B,
        AttributePreviousDayMinPressure = 0x040C,
        AttributeCurrentDayMaxDemand = 0x040D,
        AttributePreviousDayMaxDemand = 0x040E,
        AttributeCurrentMonthMaxDemand = 0x040F,
        AttributeCurrentYearMaxDemand = 0x0410,
        AttributeCurrentDayMaxEnergyCarrierDemand = 0x0411,
        AttributePreviousDayMaxEnergyCarrierDemand = 0x0412,
        AttributeCurrentMonthMaxEnergyCarrierDemand = 0x0413,
        AttributeCurrentMonthMinEnergyCarrierDemand = 0x0414,
        AttributeCurrentYearMaxEnergyCarrierDemand = 0x0415,
        AttributeCurrentYearMinEnergyCarrierDemand = 0x0416,

        // Load profile configuration
        AttributeMaxNumberOfPeriodsDelivered = 0x0500,

        // Supply limit,
        AttributeCurrentDemandDelivered = 0x0600,
        AttributeDemandLimit = 0x0601,
        AttributeDemandIntegrationPeriod = 0x0602,
        AttributeNumberOfDemandSubintervals = 0x0603,

        // Block information
        AttributeCurrentNoTierBlock1SummationDelivered = 0x0700,
        AttributeCurrentNoTierBlock2SummationDelivered = 0x0701,
        AttributeCurrentNoTierBlock3SummationDelivered = 0x0702,
        AttributeCurrentNoTierBlock4SummationDelivered = 0x0703,
        AttributeCurrentNoTierBlock5SummationDelivered = 0x0704,
        AttributeCurrentNoTierBlock6SummationDelivered = 0x0705,
        AttributeCurrentNoTierBlock7SummationDelivered = 0x0706,
        AttributeCurrentNoTierBlock8SummationDelivered = 0x0707,
        AttributeCurrentNoTierBlock9SummationDelivered = 0x0708,
        AttributeCurrentNoTierBlock10SummationDelivered = 0x0709,
        AttributeCurrentNoTierBlock11SummationDelivered = 0x070A,
        AttributeCurrentNoTierBlock12SummationDelivered = 0x070B,
        AttributeCurrentNoTierBlock13SummationDelivered = 0x070C,
        AttributeCurrentNoTierBlock14SummationDelivered = 0x070D,
        AttributeCurrentNoTierBlock15SummationDelivered = 0x070E,
        AttributeCurrentNoTierBlock16SummationDelivered = 0x070F,
        AttributeCurrentTier1Block1SummationDelivered = 0x0710,
        AttributeCurrentTier1Block2SummationDelivered = 0x0711,
        AttributeCurrentTier1Block3SummationDelivered = 0x0712,
        AttributeCurrentTier1Block4SummationDelivered = 0x0713,
        AttributeCurrentTier1Block5SummationDelivered = 0x0714,
        AttributeCurrentTier1Block6SummationDelivered = 0x0715,
        AttributeCurrentTier1Block7SummationDelivered = 0x0716,
        AttributeCurrentTier1Block8SummationDelivered = 0x0717,
        AttributeCurrentTier1Block9SummationDelivered = 0x0718,
        AttributeCurrentTier1Block10SummationDelivered = 0x0719,
        AttributeCurrentTier1Block11SummationDelivered = 0x071A,
        AttributeCurrentTier1Block12SummationDelivered = 0x071B,
        AttributeCurrentTier1Block13SummationDelivered = 0x071C,
        AttributeCurrentTier1Block14SummationDelivered = 0x071D,
        AttributeCurrentTier1Block15SummationDelivered = 0x071E,
        AttributeCurrentTier1Block16SummationDelivered = 0x071F,
        AttributeCurrentTier2Block1SummationDelivered = 0x0720,
        AttributeCurrentTier2Block2SummationDelivered = 0x0721,
        AttributeCurrentTier2Block3SummationDelivered = 0x0722,
        AttributeCurrentTier2Block4SummationDelivered = 0x0723,
        AttributeCurrentTier2Block5SummationDelivered = 0x0724,
        AttributeCurrentTier2Block6SummationDelivered = 0x0725,
        AttributeCurrentTier2Block7SummationDelivered = 0x0726,
        AttributeCurrentTier2Block8SummationDelivered = 0x0727,
        AttributeCurrentTier2Block9SummationDelivered = 0x0728,
        AttributeCurrentTier2Block10SummationDelivered = 0x0729,
        AttributeCurrentTier2Block11SummationDelivered = 0x072A,
        AttributeCurrentTier2Block12SummationDelivered = 0x072B,
        AttributeCurrentTier2Block13SummationDelivered = 0x072C,
        AttributeCurrentTier2Block14SummationDelivered = 0x072D,
        AttributeCurrentTier2Block15SummationDelivered = 0x072E,
        AttributeCurrentTier2Block16SummationDelivered = 0x072F,
        AttributeCurrentTier3Block1SummationDelivered = 0x0730,
        AttributeCurrentTier3Block2SummationDelivered = 0x0731,
        AttributeCurrentTier3Block3SummationDelivered = 0x0732,
        AttributeCurrentTier3Block4SummationDelivered = 0x0733,
        AttributeCurrentTier3Block5SummationDelivered = 0x0734,
        AttributeCurrentTier3Block6SummationDelivered = 0x0735,
        AttributeCurrentTier3Block7SummationDelivered = 0x0736,
        AttributeCurrentTier3Block8SummationDelivered = 0x0737,
        AttributeCurrentTier3Block9SummationDelivered = 0x0738,
        AttributeCurrentTier3Block10SummationDelivered = 0x0739,
        AttributeCurrentTier3Block11SummationDelivered = 0x073A,
        AttributeCurrentTier3Block12SummationDelivered = 0x073B,
        AttributeCurrentTier3Block13SummationDelivered = 0x073C,
        AttributeCurrentTier3Block14SummationDelivered = 0x073D,
        AttributeCurrentTier3Block15SummationDelivered = 0x073E,
        AttributeCurrentTier3Block16SummationDelivered = 0x073F,
        AttributeCurrentTier4Block1SummationDelivered = 0x0740,
        AttributeCurrentTier4Block2SummationDelivered = 0x0741,
        AttributeCurrentTier4Block3SummationDelivered = 0x0742,
        AttributeCurrentTier4Block4SummationDelivered = 0x0743,
        AttributeCurrentTier4Block5SummationDelivered = 0x0744,
        AttributeCurrentTier4Block6SummationDelivered = 0x0745,
        AttributeCurrentTier4Block7SummationDelivered = 0x0746,
        AttributeCurrentTier4Block8SummationDelivered = 0x0747,
        AttributeCurrentTier4Block9SummationDelivered = 0x0748,
        AttributeCurrentTier4Block10SummationDelivered = 0x0749,
        AttributeCurrentTier4Block11SummationDelivered = 0x074A,
        AttributeCurrentTier4Block12SummationDelivered = 0x074B,
        AttributeCurrentTier4Block13SummationDelivered = 0x074C,
        AttributeCurrentTier4Block14SummationDelivered = 0x074D,
        AttributeCurrentTier4Block15SummationDelivered = 0x074E,
        AttributeCurrentTier4Block16SummationDelivered = 0x074F,

        AttributeCurrentTier5Block1SummationDelivered = 0x0750,
        AttributeCurrentTier5Block2SummationDelivered = 0x0751,
        AttributeCurrentTier5Block3SummationDelivered = 0x0752,
        AttributeCurrentTier5Block4SummationDelivered = 0x0753,
        AttributeCurrentTier5Block5SummationDelivered = 0x0754,
        AttributeCurrentTier5Block6SummationDelivered = 0x0755,
        AttributeCurrentTier5Block7SummationDelivered = 0x0756,
        AttributeCurrentTier5Block8SummationDelivered = 0x0757,
        AttributeCurrentTier5Block9SummationDelivered = 0x0758,
        AttributeCurrentTier5Block10SummationDelivered = 0x0759,
        AttributeCurrentTier5Block11SummationDelivered = 0x075A,
        AttributeCurrentTier5Block12SummationDelivered = 0x075B,
        AttributeCurrentTier5Block13SummationDelivered = 0x075C,
        AttributeCurrentTier5Block14SummationDelivered = 0x075D,
        AttributeCurrentTier5Block15SummationDelivered = 0x075E,
        AttributeCurrentTier5Block16SummationDelivered = 0x075F,

        AttributeCurrentTier6Block1SummationDelivered = 0x0760,
        AttributeCurrentTier6Block2SummationDelivered = 0x0761,
        AttributeCurrentTier6Block3SummationDelivered = 0x0762,
        AttributeCurrentTier6Block4SummationDelivered = 0x0763,
        AttributeCurrentTier6Block5SummationDelivered = 0x0764,
        AttributeCurrentTier6Block6SummationDelivered = 0x0765,
        AttributeCurrentTier6Block7SummationDelivered = 0x0766,
        AttributeCurrentTier6Block8SummationDelivered = 0x0767,
        AttributeCurrentTier6Block9SummationDelivered = 0x0768,
        AttributeCurrentTier6Block10SummationDelivered = 0x0769,
        AttributeCurrentTier6Block11SummationDelivered = 0x076A,
        AttributeCurrentTier6Block12SummationDelivered = 0x076B,
        AttributeCurrentTier6Block13SummationDelivered = 0x076C,
        AttributeCurrentTier6Block14SummationDelivered = 0x076D,
        AttributeCurrentTier6Block15SummationDelivered = 0x076E,
        AttributeCurrentTier6Block16SummationDelivered = 0x076F,

        AttributeCurrentTier7Block1SummationDelivered = 0x0770,
        AttributeCurrentTier7Block2SummationDelivered = 0x0771,
        AttributeCurrentTier7Block3SummationDelivered = 0x0772,
        AttributeCurrentTier7Block4SummationDelivered = 0x0773,
        AttributeCurrentTier7Block5SummationDelivered = 0x0774,
        AttributeCurrentTier7Block6SummationDelivered = 0x0775,
        AttributeCurrentTier7Block7SummationDelivered = 0x0776,
        AttributeCurrentTier7Block8SummationDelivered = 0x0777,
        AttributeCurrentTier7Block9SummationDelivered = 0x0778,
        AttributeCurrentTier7Block10SummationDelivered = 0x0779,
        AttributeCurrentTier7Block11SummationDelivered = 0x077A,
        AttributeCurrentTier7Block12SummationDelivered = 0x077B,
        AttributeCurrentTier7Block13SummationDelivered = 0x077C,
        AttributeCurrentTier7Block14SummationDelivered = 0x077D,
        AttributeCurrentTier7Block15SummationDelivered = 0x077E,
        AttributeCurrentTier7Block16SummationDelivered = 0x077F,

        AttributeCurrentTier8Block1SummationDelivered = 0x0780,
        AttributeCurrentTier8Block2SummationDelivered = 0x0781,
        AttributeCurrentTier8Block3SummationDelivered = 0x0782,
        AttributeCurrentTier8Block4SummationDelivered = 0x0783,
        AttributeCurrentTier8Block5SummationDelivered = 0x0784,
        AttributeCurrentTier8Block6SummationDelivered = 0x0785,
        AttributeCurrentTier8Block7SummationDelivered = 0x0786,
        AttributeCurrentTier8Block8SummationDelivered = 0x0787,
        AttributeCurrentTier8Block9SummationDelivered = 0x0788,
        AttributeCurrentTier8Block10SummationDelivered = 0x0789,
        AttributeCurrentTier8Block11SummationDelivered = 0x078A,
        AttributeCurrentTier8Block12SummationDelivered = 0x078B,
        AttributeCurrentTier8Block13SummationDelivered = 0x078C,
        AttributeCurrentTier8Block14SummationDelivered = 0x078D,
        AttributeCurrentTier8Block15SummationDelivered = 0x078E,
        AttributeCurrentTier8Block16SummationDelivered = 0x078F,

        AttributeCurrentTier9Block1SummationDelivered = 0x0790,
        AttributeCurrentTier9Block2SummationDelivered = 0x0791,
        AttributeCurrentTier9Block3SummationDelivered = 0x0792,
        AttributeCurrentTier9Block4SummationDelivered = 0x0793,
        AttributeCurrentTier9Block5SummationDelivered = 0x0794,
        AttributeCurrentTier9Block6SummationDelivered = 0x0795,
        AttributeCurrentTier9Block7SummationDelivered = 0x0796,
        AttributeCurrentTier9Block8SummationDelivered = 0x0797,
        AttributeCurrentTier9Block9SummationDelivered = 0x0798,
        AttributeCurrentTier9Block10SummationDelivered = 0x0799,
        AttributeCurrentTier9Block11SummationDelivered = 0x079A,
        AttributeCurrentTier9Block12SummationDelivered = 0x079B,
        AttributeCurrentTier9Block13SummationDelivered = 0x079C,
        AttributeCurrentTier9Block14SummationDelivered = 0x079D,
        AttributeCurrentTier9Block15SummationDelivered = 0x079E,
        AttributeCurrentTier9Block16SummationDelivered = 0x079F,

        AttributeCurrentTier10Block1SummationDelivered = 0x07A0,
        AttributeCurrentTier10Block2SummationDelivered = 0x07A1,
        AttributeCurrentTier10Block3SummationDelivered = 0x07A2,
        AttributeCurrentTier10Block4SummationDelivered = 0x07A3,
        AttributeCurrentTier10Block5SummationDelivered = 0x07A4,
        AttributeCurrentTier10Block6SummationDelivered = 0x07A5,
        AttributeCurrentTier10Block7SummationDelivered = 0x07A6,
        AttributeCurrentTier10Block8SummationDelivered = 0x07A7,
        AttributeCurrentTier10Block9SummationDelivered = 0x07A8,
        AttributeCurrentTier10Block10SummationDelivered = 0x07A9,
        AttributeCurrentTier10Block11SummationDelivered = 0x07AA,
        AttributeCurrentTier10Block12SummationDelivered = 0x07AB,
        AttributeCurrentTier10Block13SummationDelivered = 0x07AC,
        AttributeCurrentTier10Block14SummationDelivered = 0x07AD,
        AttributeCurrentTier10Block15SummationDelivered = 0x07AE,
        AttributeCurrentTier10Block16SummationDelivered = 0x07AF,

        AttributeCurrentTier11Block1SummationDelivered = 0x07B0,
        AttributeCurrentTier11Block2SummationDelivered = 0x07B1,
        AttributeCurrentTier11Block3SummationDelivered = 0x07B2,
        AttributeCurrentTier11Block4SummationDelivered = 0x07B3,
        AttributeCurrentTier11Block5SummationDelivered = 0x07B4,
        AttributeCurrentTier11Block6SummationDelivered = 0x07B5,
        AttributeCurrentTier11Block7SummationDelivered = 0x07B6,
        AttributeCurrentTier11Block8SummationDelivered = 0x07B7,
        AttributeCurrentTier11Block9SummationDelivered = 0x07B8,
        AttributeCurrentTier11Block10SummationDelivered = 0x07B9,
        AttributeCurrentTier11Block11SummationDelivered = 0x07BA,
        AttributeCurrentTier11Block12SummationDelivered = 0x07BB,
        AttributeCurrentTier11Block13SummationDelivered = 0x07BC,
        AttributeCurrentTier11Block14SummationDelivered = 0x07BD,
        AttributeCurrentTier11Block15SummationDelivered = 0x07BE,
        AttributeCurrentTier11Block16SummationDelivered = 0x07BF,

        AttributeCurrentTier12Block1SummationDelivered = 0x07C0,
        AttributeCurrentTier12Block2SummationDelivered = 0x07C1,
        AttributeCurrentTier12Block3SummationDelivered = 0x07C2,
        AttributeCurrentTier12Block4SummationDelivered = 0x07C3,
        AttributeCurrentTier12Block5SummationDelivered = 0x07C4,
        AttributeCurrentTier12Block6SummationDelivered = 0x07C5,
        AttributeCurrentTier12Block7SummationDelivered = 0x07C6,
        AttributeCurrentTier12Block8SummationDelivered = 0x07C7,
        AttributeCurrentTier12Block9SummationDelivered = 0x07C8,
        AttributeCurrentTier12Block10SummationDelivered = 0x07C9,
        AttributeCurrentTier12Block11SummationDelivered = 0x07CA,
        AttributeCurrentTier12Block12SummationDelivered = 0x07CB,
        AttributeCurrentTier12Block13SummationDelivered = 0x07CC,
        AttributeCurrentTier12Block14SummationDelivered = 0x07CD,
        AttributeCurrentTier12Block15SummationDelivered = 0x07CE,
        AttributeCurrentTier12Block16SummationDelivered = 0x07CF,

        AttributeCurrentTier13Block1SummationDelivered = 0x07D0,
        AttributeCurrentTier13Block2SummationDelivered = 0x07D1,
        AttributeCurrentTier13Block3SummationDelivered = 0x07D2,
        AttributeCurrentTier13Block4SummationDelivered = 0x07D3,
        AttributeCurrentTier13Block5SummationDelivered = 0x07D4,
        AttributeCurrentTier13Block6SummationDelivered = 0x07D5,
        AttributeCurrentTier13Block7SummationDelivered = 0x07D6,
        AttributeCurrentTier13Block8SummationDelivered = 0x07D7,
        AttributeCurrentTier13Block9SummationDelivered = 0x07D8,
        AttributeCurrentTier13Block10SummationDelivered = 0x07D9,
        AttributeCurrentTier13Block11SummationDelivered = 0x07DA,
        AttributeCurrentTier13Block12SummationDelivered = 0x07DB,
        AttributeCurrentTier13Block13SummationDelivered = 0x07DC,
        AttributeCurrentTier13Block14SummationDelivered = 0x07DD,
        AttributeCurrentTier13Block15SummationDelivered = 0x07DE,
        AttributeCurrentTier13Block16SummationDelivered = 0x07DF,

        AttributeCurrentTier14Block1SummationDelivered = 0x07E0,
        AttributeCurrentTier14Block2SummationDelivered = 0x07E1,
        AttributeCurrentTier14Block3SummationDelivered = 0x07E2,
        AttributeCurrentTier14Block4SummationDelivered = 0x07E3,
        AttributeCurrentTier14Block5SummationDelivered = 0x07E4,
        AttributeCurrentTier14Block6SummationDelivered = 0x07E5,
        AttributeCurrentTier14Block7SummationDelivered = 0x07E6,
        AttributeCurrentTier14Block8SummationDelivered = 0x07E7,
        AttributeCurrentTier14Block9SummationDelivered = 0x07E8,
        AttributeCurrentTier14Block10SummationDelivered = 0x07E9,
        AttributeCurrentTier14Block11SummationDelivered = 0x07EA,
        AttributeCurrentTier14Block12SummationDelivered = 0x07EB,
        AttributeCurrentTier14Block13SummationDelivered = 0x07EC,
        AttributeCurrentTier14Block14SummationDelivered = 0x07ED,
        AttributeCurrentTier14Block15SummationDelivered = 0x07EE,
        AttributeCurrentTier14Block16SummationDelivered = 0x07EF,

        AttributeCurrentTier15Block1SummationDelivered = 0x07F0,
        AttributeCurrentTier15Block2SummationDelivered = 0x07F1,
        AttributeCurrentTier15Block3SummationDelivered = 0x07F2,
        AttributeCurrentTier15Block4SummationDelivered = 0x07F3,
        AttributeCurrentTier15Block5SummationDelivered = 0x07F4,
        AttributeCurrentTier15Block6SummationDelivered = 0x07F5,
        AttributeCurrentTier15Block7SummationDelivered = 0x07F6,
        AttributeCurrentTier15Block8SummationDelivered = 0x07F7,
        AttributeCurrentTier15Block9SummationDelivered = 0x07F8,
        AttributeCurrentTier15Block10SummationDelivered = 0x07F9,
        AttributeCurrentTier15Block11SummationDelivered = 0x07FA,
        AttributeCurrentTier15Block12SummationDelivered = 0x07FB,
        AttributeCurrentTier15Block13SummationDelivered = 0x07FC,
        AttributeCurrentTier15Block14SummationDelivered = 0x07FD,
        AttributeCurrentTier15Block15SummationDelivered = 0x07FE,
        AttributeCurrentTier15Block16SummationDelivered = 0x07FF,

        // Alarms
        AttributeGenericAlarmMask = 0x0800,
        AttributeElectricityAlarmMask = 0x0801,
        AttributeGenericFlowPressureAlarmMask = 0x0802,
        AttributeWaterSpecificAlarmMask = 0x0803,
        AttributeHeatandCoolingSpecificAlarmMask = 0x0804,
        AttributeGasSpecificAlarmMask = 0x0805,
    };
    Q_ENUM(Attribute)

    enum SupplyStatus {
        SupplyStatusOff = 0x00,
        SupplyStatusArmed = 0x01,
        SupplyStatusOn = 0x02
    };
    Q_ENUM(SupplyStatus)

    enum UnitOfMeasure {
        UnitOfMeasureBinaryKWh = 0x00,
        UnitOfMeasureBinaryCubicMeter = 0x01,
        UnitOfMeasureBinaryCubicFeet = 0x02,
        UnitOfMeasureBinaryCentum = 0x03,
        UnitOfMeasureBinaryUSGallons = 0x04,
        UnitOfMeasureBinaryImperialGallons = 0x05,
        UnitOfMeasureBinaryBTU = 0x06,
        UnitOfMeasureBinaryLiters = 0x07,
        UnitOfMeasureBinaryKPAGauge = 0x08,
        UnitOfMeasureBinaryKPAAbsolute = 0x09,
        UnitOfMeasureBinaryMegaCubicFeet = 0x0A,
        UnitOfMeasureBinaryUnitless = 0x0B,
        UnitOfMeasureBinaryMegaJoule = 0x0C,

        UnitOfMeasureBCDKWh = 0x80,
        UnitOfMeasureBCDCubicMeter = 0x81,
        UnitOfMeasureBCDCubicFeet = 0x82,
        UnitOfMeasureBCDCentum = 0x83,
        UnitOfMeasureBCDUSGallons = 0x84,
        UnitOfMeasureBCDImperialGallons = 0x85,
        UnitOfMeasureBCDBTU = 0x86,
        UnitOfMeasureBCDLiters = 0x87,
        UnitOfMeasureBCDKPAGauge = 0x88,
        UnitOfMeasureBCDKPAAbsolute = 0x89,
        UnitOfMeasureBCDMegaCubicFeet = 0x8A,
        UnitOfMeasureBCDUnitless = 0x8B,
        UnitOfMeasureBCDMegaJoule = 0x8C,
    };
    Q_ENUM(UnitOfMeasure)

    enum MeteringDeviceType {
        MeteringDeviceTypeElectric = 0x00,
        MeteringDeviceTypeGas = 0x01,
        MeteringDeviceTypeWater = 0x02,
        MeteringDeviceTypeThermal = 0x03,
        MeteringDeviceTypePressure = 0x04,
        MeteringDeviceTypeHeat = 0x05,
        MeteringDeviceTypeCooling = 0x06,
        MeteringDeviceTypeMirroredGas = 0x80,
        MeteringDeviceTypeMirroredWater = 0x81,
        MeteringDeviceTypeMirroredThermal = 0x82,
        MeteringDeviceTypeMirroredPressure = 0x83,
        MeteringDeviceTypeMirroredHeat = 0x84,
        MeteringDeviceTypeMirroredCooling = 0x85,
    };
    Q_ENUM(MeteringDeviceType)

    enum TemperatureUnitOfMeasure {
        TemperatureUnitOfMeasureBinaryKelvin = 0x00,
        TemperatureUnitOfMeasureBinaryDegreesCelsius = 0x01,
        TemperatureUnitOfMeasureBinaryDegreesFahrenheit = 0x02,

        TemperatureUnitOfMeasureBCDKelvin = 0x80,
        TemperatureUnitOfMeasureBCDDegreesCelsius = 0x81,
        TemperatureUnitOfMeasureBCDDegreesFahrenheit = 0x82,
    };
    Q_ENUM(TemperatureUnitOfMeasure)

    enum AlarmCode {
        // Generic
        AlarmCodeCheckMeter = 0x00,
        AlarmCodeLowBattery = 0x01,
        AlarmCodeTamperDetected = 0x02,
        AlarmCodeInputFailure = 0x03, // Electricity Power failure, water pipe empty, sensor failure
        AlarmCodeInputQualityFailure = 0x04, // power quality, low presure, burst
        AlarmCodeLeakDetect = 0x05,
        AlarmCodeServiceDisconnect = 0x06,
        AlarmCodeInputReversed = 0x07, // electricity reversed, reverse flow, sensor reversed

        // Electricity
        AlarmCodeLowVoltageL1 = 0x10,
        AlarmCodeHighVoltageL1 = 0x11,
        AlarmCodeLowVoltageL2 = 0x12,
        AlarmCodeHighVoltageL2 = 0x13,
        AlarmCodeLowVoltageL3 = 0x14,
        AlarmCodeHighVoltageL3 = 0x15,
        AlarmCodeOverCurrentL1 = 0x16,
        AlarmCodeOverCurrentL2 = 0x17,
        AlarmCodeOverCurrentL3 = 0x18,
        AlarmCodeFrequencyTooLowL1 = 0x19,
        AlarmCodeFrequencyTooHighL1 = 0x1A,
        AlarmCodeFrequencyTooLowL2 = 0x1B,
        AlarmCodeFrequencyTooHighL2 = 0x1C,
        AlarmCodeFrequencyTooLowL3 = 0x1D,
        AlarmCodeFrequencyTooHighL3 = 0x1E,
        AlarmCodeGroundFault = 0x1F,
        AlarmCodeElectricTamperDetect = 0x20,

        // Flow/Pressure
        AlarmCodeBurstDetect = 0x30,
        AlarmCodePressureTooLow = 0x31,
        AlarmCodePressureTooHigh = 0x32,
        AlarmCodeFlowSensorCommunicationError = 0x33,
        AlarmCodeFlowSensorMeasurementFault = 0x34,
        AlarmCodeFlowSensorReverseFlow = 0x35,
        AlarmCodeFlowSensorAirDetect = 0x36,
        AlarmCodePipeEmpty = 0x37,

        // Heat and cooling specific
        AlarmCodeInletTemperatureSensorFault = 0x50,
        AlarmCodeOutletTemperatureSensorFault = 0x51,
    };
    Q_ENUM(AlarmCode)

    explicit ZigbeeClusterMetering(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    // Used to refresh formatting attributes (multiplier/divisor)
    ZigbeeClusterReply* readFormatting();

    quint32 multiplier() const;
    quint32 divisor() const;

    quint64 currentSummationDelivered() const;
    qint32 instantaneousDemand() const;

signals:
    void currentSummationDeliveredChanged(quint64 currentSummationDelivered);
    void instantaneousDemandChanged(qint32 instantaneousDemand);

private:
    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

    quint32 m_multiplier = 1;
    quint32 m_divisor = 1;

    quint64 m_currentSummationDelivered = 0;
    qint32 m_instantaneousDemand = 0;
};

#endif // ZIGBEECLUSTERMETERING_H
