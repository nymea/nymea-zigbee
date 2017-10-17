#include "zigbee.h"

QString Zigbee::convertByteToHexString(const quint8 &byte)
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

QString Zigbee::convertByteArrayToHexString(const QByteArray &byteArray)
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

QString Zigbee::convertByte16ToHexString(const quint16 &byte)
{
    quint8 msbByte = (byte >> 8) & 0xff;
    quint8 lsbByte = (byte >> 0) & 0xff;

    return convertByteToHexString(msbByte) + convertByteToHexString(lsbByte).remove("0x");
}
