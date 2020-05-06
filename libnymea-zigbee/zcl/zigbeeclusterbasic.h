#ifndef ZIGBEECLUSTERBASIC_H
#define ZIGBEECLUSTERBASIC_H

#include <QObject>

#include "zigbeecluster.h"

class ZigbeeClusterBasic : public ZigbeeCluster
{
    Q_OBJECT
public:

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

    // From attribute 0x0007 power source
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


    explicit ZigbeeClusterBasic(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);


signals:

};

#endif // ZIGBEECLUSTERBASIC_H
