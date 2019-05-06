#ifndef ZIGBEEUTILS_H
#define ZIGBEEUTILS_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QBitArray>

#include "zigbee.h"

class ZigbeeUtils
{
    Q_GADGET

public:
    // Data utils
    QBitArray convertByteArrayToBitArray(const QByteArray &byteArray);
    QByteArray convertBitArrayToByteArray(const QBitArray &bitArray);
    static bool checkBitUint16(const quint16 &value, const int &bitNumber);

    static quint16 convertByteArrayToUint16(const QByteArray &data);
    static quint64 convertByteArrayToUint64(const QByteArray &data);

    // Debug utils
    static QString convertByteToHexString(const quint8 &byte);
    static QString convertByteArrayToHexString(const QByteArray &byteArray);
    static QString convertUint16ToHexString(const quint16 &value);
    static QString convertUint64ToHexString(const quint64 &value);

    // Enum prittify print methods
    static QString messageTypeToString(const Zigbee::InterfaceMessageType &type);
    static QString clusterIdToString(const Zigbee::ClusterId &clusterId);
    static QString profileIdToString(const Zigbee::ZigbeeProfile &profileId);

};

#endif // ZIGBEEUTILS_H
