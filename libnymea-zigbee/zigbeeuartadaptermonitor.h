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
