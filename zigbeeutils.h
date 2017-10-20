#ifndef ZIGBEEUTILS_H
#define ZIGBEEUTILS_H

#include <QObject>

#include "zigbee.h"

class ZigbeeUtils
{
    Q_GADGET

public:
    // Debug utils
    static QString convertByteToHexString(const quint8 &byte);
    static QString convertByteArrayToHexString(const QByteArray &byteArray);
    static QString convertUint16ToHexString(const quint16 &byte);
    static QString convertUint64ToHexString(const quint64 &byte);

    // Enum prittify print methods
    static QString messageTypeToString(const Zigbee::InterfaceMessageType &type);
    static QString clusterIdToString(const Zigbee::ClusterId &clusterId);
    static QString profileIdToString(const Zigbee::ZigbeeProfile &profileId);

};

#endif // ZIGBEEUTILS_H
