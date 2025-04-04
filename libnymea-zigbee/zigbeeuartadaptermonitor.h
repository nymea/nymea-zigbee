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

#ifndef ZIGBEEUARTADAPTERMONITOR_H
#define ZIGBEEUARTADAPTERMONITOR_H

#include <QObject>
#include <QSocketNotifier>

#ifdef DISABLE_UDEV
#include <QTimer>
#endif

#include "zigbeeuartadapter.h"

class ZigbeeUartAdapterMonitor : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeUartAdapterMonitor(QObject *parent = nullptr);
    ~ZigbeeUartAdapterMonitor();

    QList<ZigbeeUartAdapter> availableAdapters() const;
    bool hasAdapter(const QString &serialPort) const;

    bool isValid() const;

private:
    bool m_isValid = false;

#ifdef DISABLE_UDEV
    QTimer *m_timer = nullptr;
#else
    struct udev *m_udev = nullptr;
    struct udev_monitor *m_monitor = nullptr;
    QSocketNotifier *m_notifier = nullptr;
#endif

    QHash<QString, ZigbeeUartAdapter> m_availableAdapters;

    void addAdapterInternally(const QString &serialPort);

signals:
    void adapterAdded(const ZigbeeUartAdapter &adapter);
    void adapterRemoved(const ZigbeeUartAdapter &adapter);

};

#endif // ZIGBEEUARTADAPTERMONITOR_H
