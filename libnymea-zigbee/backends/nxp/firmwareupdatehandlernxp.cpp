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

#include "firmwareupdatehandlernxp.h"
#include "loggingcategory.h"

#include <QDir>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonParseError>

/*
 * [UpdateProvider]
 * updateBinary=/usr/bin/maveo-zigbee-flasher
 * updateReleaseFile=/usr/share/maveo-zigbee-flasher/firmware/release.json
 */

FirmwareUpdateHandlerNxp::FirmwareUpdateHandlerNxp(QObject *parent) :
    QObject(parent)
{

}

FirmwareUpdateHandlerNxp::FirmwareUpdateHandlerNxp(const QFileInfo &updateProviderConfgigurationFileInfo, QObject *parent) :
    QObject(parent),
    m_updateProviderConfgigurationFileInfo(updateProviderConfgigurationFileInfo)
{
    qCDebug(dcZigbeeController()) << "Initialize NXP firmware update provider";

    QSettings configuration(updateProviderConfgigurationFileInfo.absoluteFilePath(), QSettings::IniFormat, this);
    configuration.beginGroup("UpdateProvider");

    // Verify the update provider binary
    m_updateBinary = configuration.value("updateBinary").toString();
    if (m_updateBinary.isEmpty()) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the update binary is not specified in" << m_updateProviderConfgigurationFileInfo.absoluteFilePath();
        return;
    }

    QFileInfo updateBinaryFileInfo(m_updateBinary);
    if (!updateBinaryFileInfo.exists()) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the update binary does not exist" << updateBinaryFileInfo.absoluteFilePath();
        return;
    }

    if (!updateBinaryFileInfo.isExecutable()) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the update binary is not executable" << updateBinaryFileInfo.absoluteFilePath();
        return;
    }

    // Verify update release file and corresponding update file and version
    m_updateReleaseFilePath = configuration.value("updateReleaseFile").toString();
    QFileInfo releaseFileInfo(m_updateReleaseFilePath);
    if (! releaseFileInfo.exists()) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the release file does not exist" << releaseFileInfo.absoluteFilePath();
        return;
    }

    // Read the release file
    QFile releaseFile(releaseFileInfo.absoluteFilePath());
    if (!releaseFile.open(QFile::ReadOnly)) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but could not open update release file" << releaseFileInfo.absoluteFilePath() << releaseFile.errorString();
        return;
    }

    QByteArray releaseFileData = releaseFile.readAll();
    releaseFile.close();

    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(releaseFileData, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but could not parse the release file" << releaseFileInfo.absoluteFilePath() << jsonError.errorString();
        return;
    }

    QVariantMap releaseMap = jsonDoc.toVariant().toMap();
    if (!releaseMap.contains("version")) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the release file does not contain available version information" << releaseFileInfo.absoluteFilePath();
        return;
    }

    m_availableFirmwareVersion = releaseMap.value("version").toString();
    if (m_availableFirmwareVersion.isEmpty()) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the version in the release file is empty" << releaseFileInfo.absoluteFilePath();
        return;
    }

    if (!releaseMap.contains("firmware")) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the release file does not contain available firmware file name" << releaseFileInfo.absoluteFilePath();
        return;
    }

    m_availableFirmwareFileName = releaseMap.value("firmware").toString();
    if (m_availableFirmwareFileName.isEmpty()) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the firmware file path in the release file is empty" << releaseFileInfo.absoluteFilePath();
        return;
    }

    QFileInfo firmwareFileInfo(releaseFileInfo.canonicalPath() + QDir::separator() + m_availableFirmwareFileName);
    if (!firmwareFileInfo.exists()) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the update firmware file does not exist" << firmwareFileInfo.absoluteFilePath();
        return;
    }

    qCDebug(dcZigbeeController()) << "Firmware update prvider available:" << firmwareFileInfo.absoluteFilePath() << "Version:" << m_availableFirmwareVersion;
    configuration.endGroup();

    m_updateProcess = new QProcess(this);
    m_updateProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_updateProcess->setProgram(m_updateBinary);

    connect(m_updateProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus exitStatus) {
        qCDebug(dcZigbeeController()) << "Update process finihed" << exitCode << exitStatus;
        if (exitCode != 0) {
            emit updateFinished(false);
        } else {
            emit updateFinished(true);
        }
    });

    connect(m_updateProcess, &QProcess::readyRead, [=]() {
        qCDebug(dcZigbeeController()) << "Update process:" << qUtf8Printable(m_updateProcess->readAll().replace(0x1B, '\r'));
    });

    m_valid = true;
}

bool FirmwareUpdateHandlerNxp::updateRunning() const
{
    return m_updateProcess->state() != QProcess::NotRunning;
}

bool FirmwareUpdateHandlerNxp::isValid() const
{
    return m_valid;
}

QString FirmwareUpdateHandlerNxp::updateBinary() const
{
    return m_updateBinary;
}

QString FirmwareUpdateHandlerNxp::availableFirmwareFileName() const
{
    return m_availableFirmwareFileName;
}

QString FirmwareUpdateHandlerNxp::availableFirmwareVersion() const
{
    return m_availableFirmwareVersion;
}

void FirmwareUpdateHandlerNxp::startUpdate()
{
    if (!m_updateProcess) {
        qCWarning(dcZigbeeController()) << "Cannot start update process. The updater is not vaild.";
        return;
    }

    qCDebug(dcZigbeeController()) << "Starting firmware update for NXP controller";
    qCDebug(dcZigbeeController()) << "Firmware file:" << m_availableFirmwareFileName;
    qCDebug(dcZigbeeController()) << "Firmware version:" << m_availableFirmwareVersion;
    m_updateProcess->start();
}
