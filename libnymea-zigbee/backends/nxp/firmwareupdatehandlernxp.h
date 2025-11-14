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

#ifndef FIRMWAREUPDATEHANDLERNXP_H
#define FIRMWAREUPDATEHANDLERNXP_H

#include <QObject>
#include <QFileInfo>
#include <QProcess>

class FirmwareUpdateHandlerNxp : public QObject
{
    Q_OBJECT
public:
    explicit FirmwareUpdateHandlerNxp(QObject *parent = nullptr);
    FirmwareUpdateHandlerNxp(const QFileInfo &updateProviderConfgigurationFileInfo, QObject *parent = nullptr);

    bool initiallyFlashed() const;
    bool updateRunning() const;
    bool isValid() const;

    QString availableFirmwareVersion() const;

    void startUpdate();
    void startFactoryReset();

signals:
    void updateRunningChanged(bool updateRunning);
    void updateFinished(bool success);
    void initiallyFlashedChanged(bool initiallyFlashed);

private:
    QFileInfo m_updateProviderConfgigurationFileInfo;

    bool m_initiallyFlashed = false;
    bool m_valid = false;

    QProcess *m_updateProcess = nullptr;
    QProcess *m_factoryResetProcess = nullptr;

    QString m_updateProgram;
    QStringList m_updateProgramParameters;

    QString m_factoryResetProgram;
    QStringList m_factoryResetParameters;

    QString m_updateReleaseFilePath;
    QString m_availableFirmwareVersion;

};

#endif // FIRMWAREUPDATEHANDLERNXP_H
