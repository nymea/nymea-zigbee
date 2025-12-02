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

#include "zigbeebridgecontroller.h"
#include "loggingcategory.h"

ZigbeeBridgeController::ZigbeeBridgeController(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QSerialPort::SerialPortError>();
}

QString ZigbeeBridgeController::firmwareVersion() const
{
    return m_firmwareVersion;
}

bool ZigbeeBridgeController::available() const
{
    return m_available;
}

bool ZigbeeBridgeController::canUpdate() const
{
    return m_canUpdate;
}

bool ZigbeeBridgeController::initiallyFlashed() const
{
    return m_initiallyFlashed;
}

bool ZigbeeBridgeController::updateRunning() const
{
    return m_updateRunning;
}

bool ZigbeeBridgeController::updateAvailable(const QString &currentVersion)
{
    Q_UNUSED(currentVersion)
    return false;
}

QString ZigbeeBridgeController::updateFirmwareVersion() const
{
    return QString();
}

void ZigbeeBridgeController::startFirmwareUpdate()
{
    qCWarning(dcZigbeeController()) << "Cannot start firmware update. The feature is not implemented for this controller.";
}

void ZigbeeBridgeController::startFactoryResetUpdate()
{
    qCWarning(dcZigbeeController()) << "Cannot start firmware factory reset update. The feature is not implemented for this controller.";
}

void ZigbeeBridgeController::setSettingsDirectory(const QDir &settingsDirectory)
{
    qCDebug(dcZigbeeController()) << "Using settings directory" << settingsDirectory.absolutePath();
    m_settingsDirectory = settingsDirectory;

    // Check if we have an update provider for the controller, if so,
    // we can perform an automatic updates

    initializeUpdateProvider();
}

void ZigbeeBridgeController::setAvailable(bool available)
{
    if (m_available == available)
        return;

    qCDebug(dcZigbeeController()) << "Available changed" << available;
    m_available = available;
    emit availableChanged(m_available);
}

void ZigbeeBridgeController::setFirmwareVersion(const QString &firmwareVersion)
{
    if (m_firmwareVersion == firmwareVersion)
        return;

    qCDebug(dcZigbeeController()) << "Firmware version changed" << firmwareVersion;
    m_firmwareVersion = firmwareVersion;
    emit firmwareVersionChanged(m_firmwareVersion);
}

void ZigbeeBridgeController::initializeUpdateProvider()
{
    qCDebug(dcZigbeeController()) << "No firmware update provider configured for this controller.";
}
