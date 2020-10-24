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

#ifndef ZIGBEEBRIDGECONTROLLER_H
#define ZIGBEEBRIDGECONTROLLER_H

#include <QDir>
#include <QObject>
#include <QSerialPort>

#include "zigbee.h"

Q_DECLARE_METATYPE(QSerialPort::SerialPortError);

class ZigbeeBridgeController : public QObject
{
    Q_OBJECT
    friend class ZigbeeNetwork;

public:
    explicit ZigbeeBridgeController(QObject *parent = nullptr);
    ~ZigbeeBridgeController() = default;

    QString firmwareVersion() const;
    bool available() const;

    bool canUpdate() const;
    bool initiallyFlashed() const;
    bool updateRunning() const;

    // Optional update/initialize procedure for the zigbee controller
    virtual bool updateAvailable(const QString &currentVersion);
    virtual QString updateFirmwareVersion() const;
    virtual void startFirmwareUpdate();
    virtual void startFactoryResetUpdate();

protected:
    QString m_firmwareVersion;
    bool m_available = false;
    bool m_canUpdate = false;
    bool m_initiallyFlashed = false;
    bool m_updateRunning = false;
    QDir m_settingsDirectory = QDir("/etc/nymea/");

    void setAvailable(bool available);
    void setFirmwareVersion(const QString &firmwareVersion);

    virtual void initializeUpdateProvider();

private:
    void setSettingsDirectory(const QDir &settingsDirectory);

signals:
    void availableChanged(bool available);
    void firmwareVersionChanged(const QString &firmwareVersion);
    void canUpdateChanged(bool canUpdate);
    void updateRunningChanged(bool updateRunning);

    // APS notifications
    void apsDataConfirmReceived(const Zigbee::ApsdeDataConfirm &confirm);
    void apsDataIndicationReceived(const Zigbee::ApsdeDataIndication &indication);

};

#endif // ZIGBEEBRIDGECONTROLLER_H
