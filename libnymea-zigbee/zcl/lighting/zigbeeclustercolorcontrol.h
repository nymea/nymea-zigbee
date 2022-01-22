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

#ifndef ZIGBEECLUSTERCOLORCONTROL_H
#define ZIGBEECLUSTERCOLORCONTROL_H

#include <QObject>

#include "zcl/zigbeecluster.h"
#include "zcl/zigbeeclusterreply.h"

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeClusterColorControl : public ZigbeeCluster
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;

public:

    enum Attribute {
        // Color information
        AttributeCurrentHue = 0x0000,
        AttributeCurrentSaturation = 0x0001,
        AttributeRemainingTime = 0x0002,
        AttributeCurrentX = 0x0003,
        AttributeCurrentY = 0x0004,
        AttributeDriftCompensation = 0x0005,
        AttributeCompensationText = 0x0006,
        AttributeColorTemperatureMireds = 0x0007,
        AttributeColorMode = 0x0008,
        AttributeEnhancedCurrentHue = 0x4000,
        AttributeEnhancedColorMode = 0x4001,
        AttributeColorLoopActive = 0x4002,
        AttributeColorLoopDirection = 0x4003,
        AttributeColorLoopTime = 0x4004,
        AttributeColorLoopStartEnhancedHue = 0x4005,
        AttributeColorLoopStoredEnhancedHue = 0x4006,
        AttributeColorCapabilities = 0x400a,
        AttributeColorTempPhysicalMinMireds = 0x400b,
        AttributeColorTempPhysicalMaxMireds = 0x400c,
        // Primaries information
        AttributeNumberOfPrimaries = 0x0010,
        AttributePrimary1X = 0x0011,
        AttributePrimary1Y = 0x0012,
        AttributePrimary1Intensity = 0x0013,
        AttributePrimary2X = 0x0015,
        AttributePrimary2Y = 0x0016,
        AttributePrimary2Intensity = 0x0017,
        AttributePrimary3X = 0x0019,
        AttributePrimary3Y = 0x001a,
        AttributePrimary3Intensity = 0x001b,
        // Additional primaries information
        AttributePrimary4X = 0x0020,
        AttributePrimary4Y = 0x0021,
        AttributePrimary4Intensity = 0x0022,
        AttributePrimary5X = 0x0024,
        AttributePrimary5Y = 0x0025,
        AttributePrimary5Intensity = 0x0026,
        AttributePrimary6X = 0x0028,
        AttributePrimary6Y = 0x0029,
        AttributePrimary6Intensity = 0x002a,
        // Defined color points settings
        AttributeWhitePointX = 0x0030,
        AttributeWhitePointY = 0x0031,
        AttributeColorPointRX = 0x0032,
        AttributeColorPointRY = 0x0033,
        AttributeColorPointRIntensity = 0x0034,
        AttributeColorPointGX = 0x0036,
        AttributeColorPointGY = 0x0037,
        AttributeColorPointGIntensity = 0x0038,
        AttributeColorPointBX = 0x003a,
        AttributeColorPointBY = 0x003b,
        AttributeColorPointBIntensity = 0x003c
    };
    Q_ENUM(Attribute)

    enum DriftCompensation {
        DriftCompensationNone = 0x00,
        DriftCompensationOther = 0x01,
        DriftCompensationTemperatureMonitoring = 0x02,
        DriftCompensationOpticalLuminanceMonitoring = 0x03,
        DriftCompensationOpticalColorMonitoring = 0x04
    };
    Q_ENUM(DriftCompensation)

    enum ColorMode {
        ColorModeHueSaturation = 0x00,
        ColorModeXY = 0x01,
        ColorModeColorTemperatureMired = 0x02
    };
    Q_ENUM(ColorMode)

    enum EnhancedColorMode {
        EnhancedColorModeCurrentHueSaturation = 0x00,
        EnhancedColorModeCurrentXY = 0x01,
        EnhancedColorModeColorTemperatureMireds = 0x02,
        EnhancedColorModeEnhancedCurrentHueSaturation = 0x03
    };
    Q_ENUM(EnhancedColorMode)

    enum ColorCapability {
        ColorCapabilityHueSaturation = 0x01,
        ColorCapabilityEnhancedHue = 0x02,
        ColorCapabilityColorLoop = 0x04,
        ColorCapabilityXY = 0x08,
        ColorCapabilityColorTemperature = 0x10
    };
    Q_FLAG(ColorCapability)
    Q_DECLARE_FLAGS(ColorCapabilities, ColorCapability)

    enum Command {
        CommandMoveToHue = 0x00,
        CommandMoveHue = 0x01,
        CommandStepHue = 0x02,
        CommandMoveToSaturation = 0x03,
        CommandMoveSaturation = 0x04,
        CommandStepSaturation = 0x05,
        CommandMoveToHueAndSaturation = 0x06,
        CommandMoveToColor = 0x07,
        CommandMoveColor = 0x08,
        CommandStepColor = 0x09,
        CommandMoveToColorTemperature = 0x0a,
        CommandEnhancedMoveToHue = 0x40,
        CommandEnhancedMoveHue = 0x41,
        CommandEnhancedStepHue = 0x42,
        CommandEnhancedMoveToHueAndSaturation = 0x43,
        CommandColorLoopSet = 0x44,
        CommandStopMoveStep = 0x47,
        CommandMoveColorTemperature = 0x4b,
        CommandStepColorTemperature = 0x4c
    };
    Q_ENUM(Command)

    enum MoveDirection {
        MoveDirectionShortestDistance = 0x00,
        MoveDirectionLongestDistance = 0x01,
        MoveDirectionUp = 0x02,
        MoveDirectionDown = 0x03
    };
    Q_ENUM(MoveDirection)

    enum MoveMode {
        MoveModeStop = 0x00,
        MoveModeUp = 0x01,
        MoveModeDown = 0x02
    };
    Q_ENUM(MoveMode)

    enum StepMode {
        StepModeUp = 0x01,
        StepModeDown = 0x03
    };
    Q_ENUM(StepMode)

    // For the color loop command
    enum ColorLoopUpdate {
        ColorLoopUpdateAction = 0x01,
        ColorLoopUpdateDirection = 0x02,
        ColorLoopUpdateTime = 0x04,
        ColorLoopUpdateStartHue = 0x08
    };
    Q_FLAG(ColorLoopUpdate)
    Q_DECLARE_FLAGS(ColorLoopUpdateFlags, ColorLoopUpdate)

    enum ColorLoopAction {
        ColorLoopActionDeactivate = 0x00,
        ColorLoopActionActivateColorLoopStartEnhancedHue = 0x01,
        ColorLoopActionActivateEnhancedCurrentHue = 0x02
    };
    Q_ENUM(ColorLoopAction)

    enum ColorLoopDirection {
        ColorLoopDirectionDecrementHue = 0x00,
        ColorLoopDirectionIncrementHue = 0x01
    };
    Q_ENUM(ColorLoopDirection)

    explicit ZigbeeClusterColorControl(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

    ZigbeeClusterReply *commandMoveToHue(quint8 hue, MoveDirection direction, quint16 transitionTime);
    ZigbeeClusterReply *commandMoveHue(MoveMode moveMode, quint8 rate);
    ZigbeeClusterReply *commandStepHue(StepMode stepMode, quint8 stepSize, quint8 transitionTime);
    ZigbeeClusterReply *commandMoveToSaturation(quint8 saturation, MoveDirection direction, quint16 transitionTime);
    ZigbeeClusterReply *commandMoveSaturation(MoveMode moveMode, quint8 rate);
    ZigbeeClusterReply *commandStepSaturation(StepMode stepMode, quint8 stepSize, quint8 transitionTime);
    ZigbeeClusterReply *commandMoveToHueAndSaturation(quint8 hue, quint8 saturation, quint16 transitionTime);

    ZigbeeClusterReply *commandMoveToColor(quint16 colorX, quint16 colorY, quint16 transitionTime);
    ZigbeeClusterReply *commandMoveColor(quint16 colorXRate, quint16 colorYRate);
    ZigbeeClusterReply *commandStepColor(quint16 stepX, quint16 stepY, quint16 transitionTime);

    ZigbeeClusterReply *commandEnhancedMoveToHue(quint16 enhancedHue, MoveDirection direction, quint16 transitionTime);
    ZigbeeClusterReply *commandEnhancedMoveHue(MoveMode moveMode, quint16 rate);
    ZigbeeClusterReply *commandEnhancedStepHue(StepMode stepMode, quint16 stepSize, quint16 transitionTime);
    ZigbeeClusterReply *commandEnhancedMoveToHueAndSaturation(quint16 enhancedHue, quint8 saturation, quint16 transitionTime);

    ZigbeeClusterReply *commandColorLoopSet(ColorLoopUpdateFlags updateFlag, ColorLoopAction action, ColorLoopDirection direction, quint16 time, quint16 startHue);
    ZigbeeClusterReply *commandStopMoveStep();

    ZigbeeClusterReply *commandMoveToColorTemperature(quint16 colorTemperatureMireds, quint16 transitionTime);
    ZigbeeClusterReply *commandMoveColorTemperature(MoveMode moveMode, quint16 rate, quint16 minColorTemperature, quint16 maxColorTemperature);
    ZigbeeClusterReply *commandStepColorTemperature(StepMode stepMode, quint16 stepSize, quint16 transitionTime, quint16 minColorTemperature, quint16 maxColorTemperature);

    quint16 colorTemperatureMireds() const;
    ColorCapabilities colorCapabilities() const;

signals:
    void colorTemperatureMiredsChanged(quint16 colorTemperatureMireds);
    void colorCapabilitiesChanged(ColorCapabilities colorCapabilities);

private:
    quint16 m_colorTemperatureMireds = 0;
    ColorCapabilities m_colorCapabilities = ColorCapabilities();

    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

};

#endif // ZIGBEECLUSTERCOLORCONTROL_H
