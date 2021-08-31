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

#ifndef ZIGBEECLUSTERLIBRARY_H
#define ZIGBEECLUSTERLIBRARY_H

#include <QObject>
#include <QDebug>

#include "zigbee.h"
#include "zigbeedatatype.h"

class ZigbeeClusterLibrary
{
    Q_GADGET
public:
    /* General ZCL commans */
    enum Command {
        CommandReadAttributes = 0x00,
        CommandReadAttributesResponse = 0x01,
        CommandWriteAttributes = 0x02,
        CommandWriteAttributesUndivided = 0x03,
        CommandWriteAttributesResponse = 0x04,
        CommandWriteAttributesNoResponse = 0x05,
        CommandConfigureReporting = 0x06,
        CommandConfigureReportingResponse = 0x07,
        CommandReadReportingConfiguration = 0x08,
        CommandReadReportingConfigurationResponse = 0x09,
        CommandReportAttributes = 0x0a,
        CommandDefaultResponse = 0x0b,
        CommandDiscoverAttributes = 0x0c,
        CommandDiscoverAttributesResponse = 0x0d,
        CommandReadAttributesStructured = 0x0e,
        CommandWriteAttributesStructured = 0x0f,
        CommandWriteAttributesStructuredResponse = 0x10,
        CommandDiscoverCommandsReceived = 0x11,
        CommandDiscoverCommandsReceivedResponse = 0x12,
        CommandDiscoverCommandsGenerated = 0x13,
        CommandDiscoverCommandsGeneratedResponse = 0x14,
        CommandDiscoverAttributesExtended = 0x15,
        CommandDiscoverAttributesExtendedResponse = 0x16
    };
    Q_ENUM(Command)

    enum Status {
        StatusSuccess = 0x00,
        StatusFailure = 0x01,
        StatusNotAuthorized = 0x7e,
        StatusReservedFieldNotZero = 0x7f,
        StatusMalformedCommand = 0x80,
        StatusUnsupportedClusterCommand = 0x81,
        StatusUnsupportedGeneralCommand = 0x82,
        StatusUnsupportedManufacturerClusterCommand = 0x83,
        StatusUnsupportedManufacturerGeneralCommand = 0x84,
        StatusInvalidField = 0x85,
        StatusUnsupportedAttribute = 0x86,
        StatusInvalidValue = 0x87,
        StatusReadOnly = 0x88,
        StatusInsufficientSpace = 0x89,
        StatusDuplicateExists = 0x8a,
        StatusNotFound = 0x8b,
        StatusUnreportableAttribute = 0x8c,
        StatusInvalidDataType = 0x8d,
        StatusInvalidSector = 0x8e,
        StatusWriteOnly = 0x8f,
        StatusInconsistentStartupState = 0x90,
        StatusDefinedOutOfBand = 0x91,
        StatusInconsistent = 0x92,
        StatusActionDenied = 0x93,
        StatusTimeout = 0x94,
        StatusAbort = 0x95,
        StatusInvalidImage = 0x96,
        StatusWaitForData = 0x97,
        StatusNoImageAvailable = 0x98,
        StatusRequireMoreImage = 0x99,
        StatusNotificationPending = 0x9a,
        StatusHardwareFailure = 0xc0,
        StatusSoftwareFailure = 0xc1,
        StatusCalibrationError = 0xc2,
        StatusUnsupportedCluster = 0xc3
    };
    Q_ENUM(Status)

    enum ClusterId {
        // Basics
        ClusterIdUnknown                = 0xffff,
        ClusterIdBasic                  = 0x0000,
        ClusterIdPowerConfiguration     = 0x0001,
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
        ClusterIdAnalogInput            = 0x000C,
        ClusterIdAnalogOutput           = 0x000D,
        ClusterIdAnalogValue            = 0x000E,
        ClusterIdBinaryInput            = 0x000F,
        ClusterIdBinaryOutput           = 0x0010,
        ClusterIdBinaryValue            = 0x0011,
        ClusterIdMultistateInput        = 0x0012,
        ClusterIdMultistateOutput       = 0x0013,
        ClusterIdMultistateValue        = 0x0014,
        ClusterIdCommissoning           = 0x0015,

        // Over the air uppgrade (OTA)
        ClusterIdOtaUpgrade             = 0x0019,

        // Poll controll
        ClusterIdPollControl            = 0x0020,


        // Closures
        ClusterIdShadeConfiguration     = 0x0100,

        // Door Lock
        ClusterIdDoorLock               = 0x0101,

        // Heating, Ventilation and Air-Conditioning (HVAC)
        ClusterIdPumpConfigurationControl = 0x0200,
        ClusterIdThermostat               = 0x0201,
        ClusterIdFanControll              = 0x0202,
        ClusterIdDehumiditationControl    = 0x0203,
        ClusterIdThermostatUserControl    = 0x0204,

        // Lighting
        ClusterIdColorControl           = 0x0300,
        ClusterIdBallastConfiguration   = 0x0301,

        // Sensing
        ClusterIdIlluminanceMeasurement         = 0x0400,
        ClusterIdIlluminanceLevelSensing        = 0x0401,
        ClusterIdTemperatureMeasurement         = 0x0402,
        ClusterIdPressureMeasurement            = 0x0403,
        ClusterIdFlowMeasurement                = 0x0404,
        ClusterIdRelativeHumidityMeasurement    = 0x0405,
        ClusterIdOccupancySensing               = 0x0406,

        // Security and Safty
        ClusterIdIasZone = 0x0500,
        ClusterIdIasAce  = 0x0501,
        ClusterIdIasWd   = 0x0502,

        // Smart energy
        ClusterIdPrice          = 0x0700,
        ClusterIdLoadControl    = 0x0701,
        ClusterIdSimpleMetering = 0x0702,

        // ZLL
        ClusterIdTouchlinkCommissioning = 0x1000,

        // NXP Appliances
        ClusterIdApplianceControl           = 0x001B,
        ClusterIdApplianceIdentification    = 0x0B00,
        ClusterIdApplianceEventsAlerts      = 0x0B02,
        ClusterIdApplianceStatistics        = 0x0B03,

        // Electrical Measurement
        ClusterIdElectricalMeasurement      = 0x0B04,
        ClusterIdDiagnostics                = 0x0B05,

        // Zigbee green power
        ClusterIdGreenPower                 = 0x0021,

        // Manufacturer specific
        ClusterIdManufacturerSpecificPhilips = 0xfc00,

    };
    Q_ENUM(ClusterId)

    enum GlobalAttribute {
        GlobalAttributeClusterRevision = 0xfffd,
        GlobalAttributeAttributeReportingStatus = 0xfffe
    };
    Q_ENUM(GlobalAttribute)

    enum AttributeReportingStatus {
        AttributeReportingStatusPending = 0x00,
        AttributeReportingStatusComplete = 0x01
    };
    Q_ENUM(AttributeReportingStatus)

    // Frame control field
    enum FrameType {
        FrameTypeGlobal = 0x00,
        FrameTypeClusterSpecific = 0x01
    };
    Q_ENUM(FrameType)

    enum Direction {
        DirectionClientToServer = 0x00,
        DirectionServerToClient = 0x01
    };
    Q_ENUM(Direction)

    enum ReportingDirection {
        ReportingDirectionReporting = 0x00,
        ReportingDirectionReceiving = 0x01
    };
    Q_ENUM(ReportingDirection)

    typedef struct FrameControl {
        FrameType frameType = FrameTypeGlobal;
        bool manufacturerSpecific = false;
        Direction direction = DirectionClientToServer;
        bool disableDefaultResponse = false;
    } FrameControl;

    typedef struct Header {
        FrameControl frameControl;
        quint16 manufacturerCode = 0;
        quint8 transactionSequenceNumber = 0;
        quint8 command;
    } ZclHeader;

    typedef struct Frame {
        Header header;
        QByteArray payload;
    } Frame;


    // Read attribute
    typedef struct ReadAttributeStatusRecord {
        quint16 attributeId;
        ZigbeeClusterLibrary::Status attributeStatus;
        ZigbeeDataType dataType;
    } ReadAttributeStatusRecord;

    // Write attribute
    typedef struct WriteAttributeRecord {
        quint16 attributeId;
        Zigbee::DataType dataType;
        QByteArray data;
    } WriteAttributeRecord;

    // Reporting attributes
    typedef struct AttributeReportingConfiguration {
        ReportingDirection direction = ReportingDirectionReporting;
        quint16 attributeId = 0x0000;
        Zigbee::DataType dataType = Zigbee::NoData;
        quint16 minReportingInterval = 0x0000; // seconds
        quint16 maxReportingInterval = 0x0000; // seconds
        QByteArray reportableChange; // Data depending on the dataType
        quint16 timeoutPeriod = 0x0000;  // seconds, only used for direction receiving
    } AttributeReportingConfiguration;

    // Response of reporting configuration
    typedef struct AttributeReportingStatusRecord {
        ZigbeeClusterLibrary::Status status;
        ReportingDirection direction = ReportingDirectionReporting;
        quint16 attributeId = 0x0000;
    } AttributeReportingStatusRecord;


    // General parse/build methods
    static quint8 buildFrameControlByte(const FrameControl &frameControl);
    static FrameControl parseFrameControlByte(quint8 frameControlByte);

    static QByteArray buildHeader(const Header &header);

    static QList<ReadAttributeStatusRecord> parseAttributeStatusRecords(const QByteArray &payload);

    //static QByteArray readAttributeData(const QDataStream &stream, Zigbee::DataType dataType);
    static ZigbeeDataType readDataType(QDataStream *stream, Zigbee::DataType dataType);

    static Frame parseFrameData(const QByteArray &frameData);
    static QByteArray buildFrame(const Frame &frame);

    // AttributeReportingConfiguration
    static QByteArray buildAttributeReportingConfiguration(const AttributeReportingConfiguration &reportingConfiguration);
    static QByteArray buildWriteAttributeRecord(const WriteAttributeRecord &writeAttributeRecord);
    // TODO: parseAttributeReportingConfiguration

    static QList<AttributeReportingStatusRecord> parseAttributeReportingStatusRecords(const QByteArray &payload);

};

QDebug operator<<(QDebug debug, const ZigbeeClusterLibrary::FrameControl &frameControl);
QDebug operator<<(QDebug debug, const ZigbeeClusterLibrary::Header &header);
QDebug operator<<(QDebug debug, const ZigbeeClusterLibrary::Frame &frame);
QDebug operator<<(QDebug debug, const ZigbeeClusterLibrary::ReadAttributeStatusRecord &attributeStatusRecord);
QDebug operator<<(QDebug debug, const ZigbeeClusterLibrary::AttributeReportingConfiguration &attributeReportingConfiguration);
QDebug operator<<(QDebug debug, const ZigbeeClusterLibrary::AttributeReportingStatusRecord &attributeReportingStatusRecord);


#endif // ZIGBEECLUSTERLIBRARY_H
