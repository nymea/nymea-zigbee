#include "zigbeeutils.h"

#include <QMetaEnum>
#include <QDataStream>

QString ZigbeeUtils::convertByteToHexString(const quint8 &byte)
{
    QString hexString;
    QString byteString = QString::number(byte, 16);
    if (byteString.count() == 1) {
        hexString = QString("0x0%1").arg(byteString);
    } else {
        hexString = QString("0x%1").arg(byteString);
    }
    return hexString.toStdString().data();
}

QString ZigbeeUtils::convertByteArrayToHexString(const QByteArray &byteArray)
{
    QString hexString;
    for (int i = 0; i < byteArray.count(); i++) {
        hexString.append(convertByteToHexString((quint8)byteArray.at(i)));
        if (i != byteArray.count() -1) {
            hexString.append(" ");
        }
    }
    return hexString.toStdString().data();
}

QString ZigbeeUtils::convertUint16ToHexString(const quint16 &byte)
{
    quint8 msbByte = (byte >> 8) & 0xff;
    quint8 lsbByte = (byte >> 0) & 0xff;

    return convertByteToHexString(msbByte) + convertByteToHexString(lsbByte).remove("0x");
}

QString ZigbeeUtils::convertUint64ToHexString(const quint64 &byte)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << byte;
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

    return enumString.remove("Zigbee::ClusterId(ClusterId").remove(")");
}

QString ZigbeeUtils::profileIdToString(const Zigbee::ZigbeeProfile &profileId)
{
    QMetaObject metaObject = Zigbee::staticMetaObject;
    QMetaEnum metaEnum = metaObject.enumerator(metaObject.indexOfEnumerator("ZigbeeProfile"));

    QString enumString = metaEnum.valueToKey(profileId);

    return enumString.remove("Zigbee::ZigbeeProfile(ZigbeeProfile").remove(")");
}
