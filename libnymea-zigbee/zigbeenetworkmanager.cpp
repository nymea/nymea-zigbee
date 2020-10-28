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

#include "zigbeenetworkmanager.h"
#include "loggingcategory.h"

#include "backends/nxp/zigbeenetworknxp.h"
#include "backends/deconz/zigbeenetworkdeconz.h"

#include <QDateTime>
#include <QSerialPortInfo>

QList<ZigbeeAdapter> ZigbeeNetworkManager::availableAdapters()
{
    QList<ZigbeeAdapter> adapters;
    qCDebug(dcZigbeeNetwork()) << "Loading available adapters" ;
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        qCDebug(dcZigbeeNetwork()) << "Adapter candidate" << serialPortInfo.portName();
        qCDebug(dcZigbeeNetwork()) << "   Description:" << serialPortInfo.description();
        qCDebug(dcZigbeeNetwork()) << "   System location:" << serialPortInfo.systemLocation();
        qCDebug(dcZigbeeNetwork()) << "   Manufacturer:" << serialPortInfo.manufacturer();
        qCDebug(dcZigbeeNetwork()) << "   Serialnumber:" << serialPortInfo.serialNumber();

        if (serialPortInfo.hasProductIdentifier()) {
            qCDebug(dcZigbeeNetwork()) << "   Product identifier:" << serialPortInfo.productIdentifier();
        }

        if (serialPortInfo.hasVendorIdentifier()) {
            qCDebug(dcZigbeeNetwork()) << "   Vendor identifier:" << serialPortInfo.vendorIdentifier();
        }

        // Check if we recognize this controller
        ZigbeeAdapter adapter;
        if (serialPortInfo.portName().isEmpty()) {
            adapter.setName("Zigbee adapter");
        } else {
            adapter.setName(serialPortInfo.portName());
        }

        if (serialPortInfo.description().isEmpty()) {
            adapter.setDescription("Unknown");
        } else {
            adapter.setDescription(serialPortInfo.description());
        }
        adapter.setSystemLocation(serialPortInfo.systemLocation());

        // Check if we recognize this adapter from USB information
        if (serialPortInfo.manufacturer().toLower().contains("dresden elektronik")) {
            adapter.setBackendSuggestionAvailable(true);
            adapter.setSuggestedBackendType(Zigbee::BackendTypeDeconz);
            adapter.setSuggestedBaudRate(38400);
        } else if (serialPortInfo.manufacturer().toLower().contains("nxp")) {
            adapter.setBackendSuggestionAvailable(true);
            adapter.setSuggestedBackendType(Zigbee::BackendTypeNxp);
            adapter.setSuggestedBaudRate(115200);
        }

        adapters.append(adapter);
    }

    return adapters;
}

QStringList ZigbeeNetworkManager::availableBackendTypes()
{
    return {"deCONZ", "NXP"};
}

ZigbeeNetwork *ZigbeeNetworkManager::createZigbeeNetwork(Zigbee::BackendType backend, QObject *parent)
{
    // Note: required for generating random PAN ID
    srand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch() / 1000));

    switch (backend) {
    case Zigbee::BackendTypeNxp:
        return qobject_cast<ZigbeeNetwork *>(new ZigbeeNetworkNxp(parent));
    case Zigbee::BackendTypeDeconz:
        return qobject_cast<ZigbeeNetwork *>(new ZigbeeNetworkDeconz(parent));
    }

    return nullptr;
}
