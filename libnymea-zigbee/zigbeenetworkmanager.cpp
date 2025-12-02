// SPDX-License-Identifier: LGPL-3.0-or-later

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* nymea-zigbee
* Zigbee integration module for nymea
*
* Copyright (C) 2013 - 2024, nymea GmbH
* Copyright (C) 2024 - 2025, chargebyte austria GmbH
*
* This file is part of nymea-zigbee.
*
* nymea-zigbee is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3
* of the License, or (at your option) any later version.
*
* nymea-zigbee is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with nymea-zigbee. If not, see <https://www.gnu.org/licenses/>.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "zigbeenetworkmanager.h"
#include "loggingcategory.h"

#include "backends/nxp/zigbeenetworknxp.h"
#include "backends/deconz/zigbeenetworkdeconz.h"
#ifndef ZIGBEE_DISABLE_TI
#include "backends/ti/zigbeenetworkti.h"
#endif

#include <QDateTime>

QStringList ZigbeeNetworkManager::availableZigbeeBackendTypes()
{
    return {"deCONZ", "NXP"};
}

ZigbeeNetwork *ZigbeeNetworkManager::createZigbeeNetwork(const QUuid &networkUuid, Zigbee::ZigbeeBackendType backend, QObject *parent)
{
    // Note: required for generating random PAN ID
    srand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch() / 1000));

    switch (backend) {
    case Zigbee::ZigbeeBackendTypeNxp:
        return qobject_cast<ZigbeeNetwork *>(new ZigbeeNetworkNxp(networkUuid, parent));
    case Zigbee::ZigbeeBackendTypeDeconz:
        return qobject_cast<ZigbeeNetwork *>(new ZigbeeNetworkDeconz(networkUuid, parent));
#ifndef ZIGBEE_DISABLE_TI
    case Zigbee::ZigbeeBackendTypeTi:
        return qobject_cast<ZigbeeNetwork *>(new ZigbeeNetworkTi(networkUuid, parent));
#endif
    }

    return nullptr;
}
