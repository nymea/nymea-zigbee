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

Example update provider configuration, if initiallyFlashed is false, a factory reset will be performed in any case

[UpdateProvider]
updateCommand=/usr/bin/maveo-zigbee-flasher -s /dev/ttymxc2 -p 131 -r 132
updateReleaseFile=/usr/share/maveo-zigbee-flasher/firmware/release.json
factoryResetCommand=/usr/bin/maveo-zigbee-flasher -s /dev/ttymxc2 -p 131 -r 132 -e
initiallyFlashed=false

*/

FirmwareUpdateHandlerNxp::FirmwareUpdateHandlerNxp(QObject *parent) :
    QObject(parent)
{

}

FirmwareUpdateHandlerNxp::FirmwareUpdateHandlerNxp(const QFileInfo &updateProviderConfgigurationFileInfo, QObject *parent) :
    QObject(parent),
    m_updateProviderConfgigurationFileInfo(updateProviderConfgigurationFileInfo)
{
    qCDebug(dcZigbeeController()) << "Initialize NXP firmware update provider from" << updateProviderConfgigurationFileInfo.absoluteFilePath();

    QSettings configuration(updateProviderConfgigurationFileInfo.absoluteFilePath(), QSettings::IniFormat, this);
    configuration.beginGroup("UpdateProvider");

    // Verify the update command
    QString updateCommand = configuration.value("updateCommand").toString();
    if (updateCommand.isEmpty()) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the update command is not specified in" << m_updateProviderConfgigurationFileInfo.absoluteFilePath();
        return;
    }

    QStringList updateCommandTokens = updateCommand.split(" ");
    qCDebug(dcZigbeeController()) << "Update command tokens" << updateCommandTokens;
    if (updateCommandTokens.count() == 0) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the update command could not be parsed correctly" << m_updateProviderConfgigurationFileInfo.absoluteFilePath();
        return;
    }

    m_updateProgram = updateCommandTokens.takeFirst();
    m_updateProgramParameters << updateCommandTokens;

    qCDebug(dcZigbeeController()) << "Update program:" << m_updateProgram << "Parameters:" << m_updateProgramParameters;

    QFileInfo updateProgramFileInfo(m_updateProgram);
    if (!updateProgramFileInfo.exists()) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the update binary does not exist" << updateProgramFileInfo.absoluteFilePath();
        return;
    }

    if (!updateProgramFileInfo.isExecutable()) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the update binary is not executable" << updateProgramFileInfo.absoluteFilePath();
        return;
    }

    // Verify the factory reset command
    QString factoryResetCommand = configuration.value("factoryResetCommand").toString();
    if (factoryResetCommand.isEmpty()) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the factory reset command is not specified in" << m_updateProviderConfgigurationFileInfo.absoluteFilePath();
        return;
    }

    QStringList factoryResetCommandTokens = factoryResetCommand.split(" ");
    qCDebug(dcZigbeeController()) << "Factory reset command tokens" << factoryResetCommandTokens;
    if (factoryResetCommandTokens.count() == 0) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the factory reset command could not be parsed correctly" << m_updateProviderConfgigurationFileInfo.absoluteFilePath();
        return;
    }

    m_factoryResetProgram = factoryResetCommandTokens.takeFirst();
    m_factoryResetParameters << factoryResetCommandTokens;

    qCDebug(dcZigbeeController()) << "Factory reset program:" << m_factoryResetProgram << "Parameters:" << m_factoryResetParameters;

    QFileInfo factoryResetProgramFileInfo(m_factoryResetProgram);
    if (!factoryResetProgramFileInfo.exists()) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the factory reset binary does not exist" << factoryResetProgramFileInfo.absoluteFilePath();
        return;
    }

    if (!factoryResetProgramFileInfo.isExecutable()) {
        qCWarning(dcZigbeeController()) << "Update provider configuration available but the factory reset binary is not executable" << updateProgramFileInfo.absoluteFilePath();
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

    // Check if the controller has been initially flashed
    m_initiallyFlashed = configuration.value("initiallyFlashed", false).toBool();

    configuration.endGroup();

    qCDebug(dcZigbeeController()) << "Firmware update provider available:" << firmwareFileInfo.fileName() << "Version:" << m_availableFirmwareVersion << "Initially flashed:" << m_initiallyFlashed;

    // Set up update process
    m_updateProcess = new QProcess(this);
    m_updateProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_updateProcess->setProgram(m_updateProgram);
    m_updateProcess->setArguments(m_updateProgramParameters);

    connect(m_updateProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus exitStatus) {
        qCDebug(dcZigbeeController()) << "Update process finihed" << exitCode << exitStatus;
        if (exitCode != 0) {
            emit updateFinished(false);
        } else {
            emit updateFinished(true);
        }
    });

    connect(m_updateProcess, &QProcess::readyRead, [=]() {
        qCDebug(dcZigbeeController()) << "Update process:" << qUtf8Printable(m_updateProcess->readAll());
    });


    // Set up factory reset process
    m_factoryResetProcess = new QProcess(this);
    m_factoryResetProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_factoryResetProcess->setProgram(m_factoryResetProgram);
    m_factoryResetProcess->setArguments(m_factoryResetParameters);

    connect(m_factoryResetProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus exitStatus) {
        qCDebug(dcZigbeeController()) << "Factory reset process finihed" << exitCode << exitStatus;
        if (exitCode != 0) {
            emit updateFinished(false);
        } else {
            // The factory reset has been finished successfully
            QSettings configuration(m_updateProviderConfgigurationFileInfo.absoluteFilePath(), QSettings::IniFormat, this);
            configuration.beginGroup("UpdateProvider");
            configuration.setValue("initiallyFlashed", true);
            configuration.endGroup();
            m_initiallyFlashed = true;
            emit initiallyFlashedChanged(m_initiallyFlashed);

            emit updateFinished(true);
        }
    });

    connect(m_factoryResetProcess, &QProcess::readyRead, [=]() {
        qCDebug(dcZigbeeController()) << "Factory reset process:" << qUtf8Printable(m_factoryResetProcess->readAll());
    });

    m_valid = true;
}

bool FirmwareUpdateHandlerNxp::initiallyFlashed() const
{
    return m_initiallyFlashed;
}

bool FirmwareUpdateHandlerNxp::updateRunning() const
{
    return m_updateProcess->state() != QProcess::NotRunning;
}

bool FirmwareUpdateHandlerNxp::isValid() const
{
    return m_valid;
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

void FirmwareUpdateHandlerNxp::startFactoryReset()
{
    if (!m_factoryResetProcess) {
        qCWarning(dcZigbeeController()) << "Cannot start factory reset process. The update provider is not vaild.";
        return;
    }

    qCDebug(dcZigbeeController()) << "Starting factory reset for NXP controller";
    qCDebug(dcZigbeeController()) << "Firmware file:" << m_availableFirmwareFileName;
    qCDebug(dcZigbeeController()) << "Firmware version:" << m_availableFirmwareVersion;
    m_factoryResetProcess->start();
}
