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

    QString availableFirmwareFileName() const;
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
    QString m_availableFirmwareFileName;
    QString m_availableFirmwareVersion;

};

#endif // FIRMWAREUPDATEHANDLERNXP_H
