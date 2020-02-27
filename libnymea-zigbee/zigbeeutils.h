/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
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
    static QString convertUint32ToHexString(const quint32 &value);
    static QString convertUint64ToHexString(const quint64 &value);

    // Enum prittify print methods
    static QString messageTypeToString(const Zigbee::InterfaceMessageType &type);
    static QString clusterIdToString(const Zigbee::ClusterId &clusterId);
    static QString profileIdToString(const Zigbee::ZigbeeProfile &profileId);

    static quint64 generateRandomPanId();

};

#endif // ZIGBEEUTILS_H
