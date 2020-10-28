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

#include "zigbeebridgecontroller.h"
#include "loggingcategory.h"

ZigbeeBridgeController::ZigbeeBridgeController(QObject *parent) : QObject(parent)
{

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
