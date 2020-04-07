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

#include "zigbeeutils.h"

#include <QDebug>
#include <QDateTime>
#include <QMetaEnum>
#include <QDataStream>

#include <math.h>

// Disabling 1000 to 2500 K, as we never want to go into full red for color temp
static QList<QColor> colorTemperatureScale = {
//    QColor(255, 56, 0),     // 1000 K
//    QColor(255, 71, 0),
//    QColor(255, 83, 0),
//    QColor(255, 93, 0),
//    QColor(255, 101, 0),
//    QColor(255, 109, 0),
//    QColor(255, 115, 0),
//    QColor(255, 121, 0),
//    QColor(255, 126, 0),
//    QColor(255, 131, 0),
//    QColor(255, 138, 18),
//    QColor(255, 142, 33),
//    QColor(255, 147, 44),
//    QColor(255, 152, 54),
//    QColor(255, 157, 63),
//    QColor(255, 161, 72),    // 2500K
    QColor(255, 165, 79),
    QColor(255, 169, 87),
    QColor(255, 173, 94),
    QColor(255, 177, 101),
    QColor(255, 180, 107),
    QColor(255, 184, 114),
    QColor(255, 187, 120),
    QColor(255, 190, 126),
    QColor(255, 193, 132),
    QColor(255, 196, 137),
    QColor(255, 199, 143),
    QColor(255, 201, 148),
    QColor(255, 204, 153),
    QColor(255, 206, 159),
    QColor(255, 209, 163),
    QColor(255, 211, 168),
    QColor(255, 213, 173),
    QColor(255, 215, 177),
    QColor(255, 217, 182),
    QColor(255, 219, 186),
    QColor(255, 221, 190),
    QColor(255, 223, 194),
    QColor(255, 225, 198),
    QColor(255, 227, 202),
    QColor(255, 228, 206),
    QColor(255, 230, 210),
    QColor(255, 232, 213),
    QColor(255, 233, 217),
    QColor(255, 235, 220),
    QColor(255, 236, 224),
    QColor(255, 238, 227),
    QColor(255, 239, 230),
    QColor(255, 240, 233),
    QColor(255, 242, 236),
    QColor(255, 243, 239),
    QColor(255, 244, 242),
    QColor(255, 245, 245),
    QColor(255, 246, 247),
    QColor(255, 248, 251),
    QColor(255, 249, 253),
    QColor(254, 249, 255),
    QColor(252, 247, 255),
    QColor(249, 246, 255),
    QColor(247, 245, 255),
    QColor(245, 243, 255),
    QColor(243, 242, 255),
    QColor(240, 241, 255),
    QColor(239, 240, 255),
    QColor(237, 239, 255),
    QColor(235, 238, 255),
    QColor(233, 237, 255),
    QColor(231, 236, 255),
    QColor(230, 235, 255),
    QColor(228, 234, 255),
    QColor(227, 233, 255),
    QColor(225, 232, 255),
    QColor(224, 231, 255),
    QColor(222, 230, 255),
    QColor(221, 230, 255),
    QColor(220, 229, 255),
    QColor(218, 229, 255),
    QColor(217, 227, 255),
    QColor(216, 227, 255),
    QColor(215, 226, 255),
    QColor(214, 225, 255),
    QColor(212, 225, 255),
    QColor(211, 224, 255),
    QColor(210, 223, 255),
    QColor(209, 223, 255),
    QColor(208, 222, 255),
    QColor(207, 221, 255),
    QColor(207, 221, 255),
    QColor(206, 220, 255),
    QColor(205, 220, 255),
    QColor(207, 218, 255),
    QColor(207, 218, 255),
    QColor(206, 217, 255),
    QColor(205, 217, 255),
    QColor(204, 216, 255),
    QColor(204, 216, 255),
    QColor(203, 215, 255),
    QColor(202, 215, 255),
    QColor(202, 214, 255),
    QColor(201, 214, 255),
    QColor(200, 213, 255),
    QColor(200, 213, 255),
    QColor(199, 212, 255),
    QColor(198, 212, 255),
    QColor(198, 212, 255),
    QColor(197, 211, 255),
    QColor(197, 211, 255),
    QColor(197, 210, 255),
    QColor(196, 210, 255),
    QColor(195, 210, 255),
    QColor(195, 209, 255) // 12000 K
};

QBitArray ZigbeeUtils::convertByteArrayToBitArray(const QByteArray &byteArray)
{
    QBitArray bitArray(byteArray.count() * 8);

    // Convert from QByteArray to QBitArray
    for(int i = 0; i < byteArray.count(); ++i) {
        for(int b = 0; b < 8; b++) {
            bitArray.setBit(i * 8 + b, byteArray.at(i) & (1 << ( 7 - b)));
        }
    }

    return bitArray;
}

QByteArray ZigbeeUtils::convertBitArrayToByteArray(const QBitArray &bitArray)
{
    QByteArray byteArray;

    for(int b = 0; b < bitArray.count(); ++b) {
        byteArray[b / 8] = static_cast<char>((byteArray.at( b / 8) | ((bitArray[b] ? 1 : 0) << (7 - ( b % 8)))));
    }
    return byteArray;
}

bool ZigbeeUtils::checkBitUint8(const quint8 &value, const int &bitNumber)
{
    return value & (1 << bitNumber);
}

bool ZigbeeUtils::checkBitUint16(const quint16 &value, const int &bitNumber)
{
    return value & (1 << bitNumber);
}

quint16 ZigbeeUtils::convertByteArrayToUint16(const QByteArray &data)
{
    Q_ASSERT_X(data.count() == 2, "converting data", "Invalid byte array size for converting to quint16");

    quint16 value = static_cast<quint8>(data.at(0));
    value <<= 8;
    value |= static_cast<quint8>(data.at(1));
    return value;
}

quint64 ZigbeeUtils::convertByteArrayToUint64(const QByteArray &data)
{
    Q_ASSERT_X(data.count() == 8, "converting data", "Invalid byte array size for converting to quint64");

    quint64 value = static_cast<quint8>(data.at(0));
    value <<= 8;
    value |= static_cast<quint8>(data.at(1));
    value <<= 8;
    value |= static_cast<quint8>(data.at(2));
    value <<= 8;
    value |= static_cast<quint8>(data.at(3));
    value <<= 8;
    value |= static_cast<quint8>(data.at(4));
    value <<= 8;
    value |= static_cast<quint8>(data.at(5));
    value <<= 8;
    value |= static_cast<quint8>(data.at(6));
    value <<= 8;
    value |= static_cast<quint8>(data.at(7));
    return value;
}

QString ZigbeeUtils::convertByteToHexString(const quint8 &byte)
{
    QString hexString(QStringLiteral("0x%1"));
    hexString = hexString.arg(byte, 2, 16, QLatin1Char('0'));
    return hexString.toStdString().data();
}

QString ZigbeeUtils::convertByteArrayToHexString(const QByteArray &byteArray)
{
    QString hexString;
    for (int i = 0; i < byteArray.count(); i++) {
        hexString.append(convertByteToHexString(static_cast<quint8>(byteArray.at(i))));
        if (i != byteArray.count() - 1) {
            hexString.append(" ");
        }
    }
    return hexString.toStdString().data();
}

QString ZigbeeUtils::convertUint16ToHexString(const quint16 &value)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << value;

    return QString("0x%1").arg(convertByteArrayToHexString(data).remove(" ").remove("0x"));
}

QString ZigbeeUtils::convertUint32ToHexString(const quint32 &value)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << value;

    return QString("0x%1").arg(convertByteArrayToHexString(data).remove(" ").remove("0x"));
}

QString ZigbeeUtils::convertUint64ToHexString(const quint64 &value)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << value;
    return QString("0x%1").arg(convertByteArrayToHexString(data).remove(" ").remove("0x"));
}

QString ZigbeeUtils::messageTypeToString(const Zigbee::InterfaceMessageType &type)
{
    QMetaObject metaObject = Zigbee::staticMetaObject;
    QMetaEnum metaEnum = metaObject.enumerator(metaObject.indexOfEnumerator("InterfaceMessageType"));

    QString enumString = metaEnum.valueToKey(type);

    return enumString.remove("Zigbee::InterfaceMessageType(MessageType").remove(")");
}

QString ZigbeeUtils::clusterIdToString(const Zigbee::ClusterId &clusterId)
{
    QMetaObject metaObject = Zigbee::staticMetaObject;
    QMetaEnum metaEnum = metaObject.enumerator(metaObject.indexOfEnumerator("ClusterId"));

    QString enumString = metaEnum.valueToKey(clusterId);

    return enumString.remove("Zigbee::ClusterId(ClusterId").remove(")").append(QString("(%1)").arg(ZigbeeUtils::convertUint16ToHexString(clusterId)));
}

QString ZigbeeUtils::profileIdToString(const Zigbee::ZigbeeProfile &profileId)
{
    QMetaObject metaObject = Zigbee::staticMetaObject;
    QMetaEnum metaEnum = metaObject.enumerator(metaObject.indexOfEnumerator("ZigbeeProfile"));

    QString enumString = metaEnum.valueToKey(profileId);

    return enumString.remove("Zigbee::ZigbeeProfile(ZigbeeProfile").remove(")");
}

quint64 ZigbeeUtils::generateRandomPanId()
{
    // Note: the PAN ID has to be between 0x0000 and 0x3fff
    return static_cast<quint64>(rand() % (0x3fff - 1) + 1);
}

QPointF ZigbeeUtils::convertColorToXY(const QColor &color)
{
    // https://developers.meethue.com/develop/application-design-guidance/color-conversion-formulas-rgb-to-xy-and-back/

    // Color values between [0, 1]

    // Gamma correction
    double redGamma =   (color.redF()   > 0.04045) ? pow((color.redF()   + 0.055) / (1.0 + 0.055), 2.4) : (color.redF()   / 12.92);
    double greenGamme = (color.greenF() > 0.04045) ? pow((color.greenF() + 0.055) / (1.0 + 0.055), 2.4) : (color.greenF() / 12.92);
    double blueGamme =  (color.blueF()  > 0.04045) ? pow((color.blueF()  + 0.055) / (1.0 + 0.055), 2.4) : (color.blueF()  / 12.92);

    // Convert the RGB values to XYZ using the Wide RGB D65 conversion formula
    double xx = redGamma * 0.664511 + greenGamme * 0.154324 + blueGamme * 0.162028;
    double yy = redGamma * 0.283881 + greenGamme * 0.668433 + blueGamme * 0.047685;
    double zz = redGamma * 0.000088 + greenGamme * 0.072310 + blueGamme * 0.986039;

    //qWarning() << "xyz" << xx << yy << zz;

    double x =  xx / (xx + yy + zz);
    double y =  yy / (xx + yy + zz);

    // Correct brightness if required
    if (y >= 1) y = 1.0;

    //qWarning() << "xy" << x << y;

    // TODO: ceck if this point is within the color gamut triangle of the light, otherwise get closest point

    return QPointF(x, y);
}

QColor ZigbeeUtils::convertXYToColor(const QPointF &xyColor)
{
    // https://developers.meethue.com/develop/application-design-guidance/color-conversion-formulas-rgb-to-xy-and-back/

    // TODO: ceck if this point is within the color gamut triangle of the light, otherwise get closest point

    // Extract x y and z
    double xx = xyColor.x();
    double yy = xyColor.y();
    double zz = 1.0 - xx - yy;

    // Get x, y and z
    double y = 1.0; // assume full brightness for the calculation
    double x = (y / yy) * xx;
    double z = (y / yy) * zz;

    //qWarning() << "xyz" << x << y << z;

    // Convert to r, g and b according D65
    double r =  x * 1.656492 - y * 0.354851 - z * 0.255038;
    double g = -x * 0.707196 + y * 1.655397 + z * 0.036152;
    double b =  x * 0.051713 - y * 0.121364 + z * 1.011530;

    if (r > b && r > g && r > 1.0) {
        // red is too big
        g = g / r;
        b = b / r;
        r = 1.0;
    } else if (g > b && g > r && g > 1.0) {
        // green is too big
        r = r / g;
        b = b / g;
        g = 1.0;
    } else if (b > r && b > g && b > 1.0) {
        // blue is too big
        r = r / b;
        g = g / b;
        b = 1.0;
    }

    // Apply gamma correction
    r = (r <= 0.0031308) ? 12.92 * r : (1.0 + 0.055) * pow(r, (1.0 / 2.4)) - 0.055;
    g = (g <= 0.0031308) ? 12.92 * g : (1.0 + 0.055) * pow(g, (1.0 / 2.4)) - 0.055;
    b = (b <= 0.0031308) ? 12.92 * b : (1.0 + 0.055) * pow(b, (1.0 / 2.4)) - 0.055;

    if (r > b && r > g) {
        // red is biggest
        if (r > 1.0) {
            g = g / r;
            b = b / r;
            r = 1.0;
        }
    } else if (g > b && g > r) {
        // green is biggest
        if (g > 1.0) {
            r = r / g;
            b = b / g;
            g = 1.0;
        }
    } else if (b > r && b > g) {
        // blue is biggest
        if (b > 1.0) {
            r = r / b;
            g = g / b;
            b = 1.0;
        }
    }

    // Make sure we don't have any negative round error values
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;

    //qWarning() << r << g << b;
    QColor color;
    color.setRedF(r);
    color.setGreenF(g);
    color.setBlueF(b);
    color.setAlphaF(1.0);

    return color;
}

QColor ZigbeeUtils::convertXYToColor(quint16 x, quint16 y)
{
    QPointF xy(x / 65536.0, y / 65536.0);
    return convertXYToColor(xy);
}

QColor ZigbeeUtils::interpolateColorFromColorTemperature(int colorTemperature, int minValue, int maxValue)
{
    Q_ASSERT_X(colorTemperature >= minValue && colorTemperature <= maxValue, "interpolate colors", "Interpolation value not between min and max value");
    int intervalSize = maxValue - minValue;
    int intervalPosition = colorTemperature - minValue;
    double percentage = intervalPosition * 1.0 / intervalSize;
    int closestColorIndex = qRound((colorTemperatureScale.count() - 1) * (1.0 - percentage));
    // FIXME: interpolate between the selected index and the next color for more accuracy if required
    return colorTemperatureScale.at(closestColorIndex);
}

ZigbeeClusterAttributeReport ZigbeeUtils::parseAttributeReport(const QByteArray &data)
{
    QByteArray dataCopy = data;
    quint8 sequenceNumber = 0;
    quint16 sourceAddress = 0;
    quint8 endpointId = 0;
    quint16 clusterId = 0;
    quint16 attributeId = 0;
    quint8 attributeStatus = 0;
    quint8 attributDataType = 0;
    quint16 dataSize = 0;

    QDataStream stream(&dataCopy, QIODevice::ReadOnly);
    stream >> sequenceNumber >> sourceAddress >> endpointId >> clusterId >> attributeId >> attributeStatus >> attributDataType >> dataSize;

    Zigbee::DataType dataType = static_cast<Zigbee::DataType>(attributDataType);
    QByteArray attributeData = data.right(dataSize);

    if (attributeData.length() != dataSize) {
        //qCWarning(dcZigbeeNetwork()) << "HACK" << attributeData.length() << "!=" << dataSize;
        // Note: the NXP firmware for JN5169 has a bug here and does not send the attributeStatus.
        // Repars data without attribute status
        sequenceNumber = 0;
        sourceAddress = 0;
        endpointId = 0;
        clusterId = 0;
        attributeId = 0;
        attributeStatus = 0;
        attributDataType = 0;
        dataSize = 0;

        QDataStream alternativeStream(&dataCopy, QIODevice::ReadOnly);
        alternativeStream >> sequenceNumber >> sourceAddress >> endpointId >> clusterId >> attributeId >> attributDataType >> dataSize;

        dataType = static_cast<Zigbee::DataType>(attributDataType);
        attributeData = data.right(dataSize);
    }

//    qCDebug(dcZigbeeNetwork()) << "Attribute read response:";
//    qCDebug(dcZigbeeNetwork()) << "    SQN:" << ZigbeeUtils::convertByteToHexString(sequenceNumber);
//    qCDebug(dcZigbeeNetwork()) << "    Source address:" << ZigbeeUtils::convertUint16ToHexString(sourceAddress);
//    qCDebug(dcZigbeeNetwork()) << "    End point:" << ZigbeeUtils::convertByteToHexString(endpointId);
//    qCDebug(dcZigbeeNetwork()) << "    Cluster:" << ZigbeeUtils::clusterIdToString(static_cast<Zigbee::ClusterId>(clusterId));
//    qCDebug(dcZigbeeNetwork()) << "    Attribut id:" << ZigbeeUtils::convertUint16ToHexString(attributeId);
//    qCDebug(dcZigbeeNetwork()) << "    Attribut status:" <<  static_cast<Zigbee::ZigbeeStatus>(attributeStatus);
//    qCDebug(dcZigbeeNetwork()) << "    Attribut data type:" << dataType;
//    qCDebug(dcZigbeeNetwork()) << "    Attribut size:" << dataSize;
//    qCDebug(dcZigbeeNetwork()) << "    Data:" << ZigbeeUtils::convertByteArrayToHexString(attributeData);

//    switch (dataType) {
//    case Zigbee::CharString:
//        qCDebug(dcZigbeeNetwork()) << "    Data(converted)" << QString::fromUtf8(attributeData);
//        break;
//    case Zigbee::Bool:
//        qCDebug(dcZigbeeNetwork()) << "    Data(converted)" << static_cast<bool>(attributeData.at(0));
//        break;
//    default:
//        break;
//    }


//    ZigbeeNodeNxp *node = qobject_cast<ZigbeeNodeNxp *>(getZigbeeNode(sourceAddress));
//    if (!node) {
//        qCWarning(dcZigbeeNode()) << "Received an attribute report from an unknown node. Ignoring data.";
//        return;
//    }

    ZigbeeClusterAttributeReport attributeReport;
    attributeReport.sourceAddress = sourceAddress;
    attributeReport.endpointId = endpointId;
    attributeReport.clusterId = static_cast<Zigbee::ClusterId>(clusterId);
    attributeReport.attributeId = attributeId;
    attributeReport.attributeStatus = static_cast<Zigbee::ZigbeeStatus>(attributeStatus);
    attributeReport.dataType = dataType;
    attributeReport.data = attributeData;
    return attributeReport;
}
