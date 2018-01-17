#include "zigbeeutils.h"

#include <QMetaEnum>
#include <QDataStream>

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
        byteArray[b / 8] = (byteArray.at( b / 8) | ((bitArray[b] ? 1 : 0) << (7 - ( b % 8))));
    }
    return byteArray;
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
        hexString.append(convertByteToHexString((quint8)byteArray.at(i)));
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
