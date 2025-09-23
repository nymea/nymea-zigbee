/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2021, nymea GmbH
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

#include "zigbeeuartadaptermonitor.h"
#include "loggingcategory.h"

#include <QSerialPortInfo>
#include <QRegularExpression>

#ifndef DISABLE_UDEV
#include <libudev.h>
#endif

ZigbeeUartAdapterMonitor::ZigbeeUartAdapterMonitor(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<ZigbeeUartAdapter>();

    // Read initially all tty devices
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        addAdapterInternally(serialPortInfo.systemLocation());
    }

#ifdef DISABLE_UDEV
    m_timer = new QTimer(this);
    m_timer->setInterval(5000);
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, this, [=](){
        QStringList availablePorts;
        // Add a new adapter if not in the list already
        foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
            availablePorts.append(serialPortInfo.systemLocation());
            if (!m_availableAdapters.contains(serialPortInfo.systemLocation())) {
                addAdapterInternally(serialPortInfo.systemLocation());
            }
        }
        // Remove adapters no longer available
        foreach (const QString &systemLocation, m_availableAdapters.keys()) {
            if (!availablePorts.contains(systemLocation)) {
                emit adapterRemoved(m_availableAdapters.take(systemLocation));
            }
        }

    });
    m_timer->start();
#else
    // Init udev
    m_udev = udev_new();
    if (!m_udev) {
        qCWarning(dcZigbeeAdapterMonitor()) << "Could not initialize udev for the adapter monitor";
        return;
    }

    // Create udev monitor
    m_monitor = udev_monitor_new_from_netlink(m_udev, "udev");
    if (!m_monitor) {
        qCWarning(dcZigbeeAdapterMonitor()) << "Could not initialize udev monitor.";
        udev_unref(m_udev);
        m_udev = nullptr;
        return;
    }

    // Set monitor filter to tty subsystem
    if (udev_monitor_filter_add_match_subsystem_devtype(m_monitor, "tty", nullptr) < 0) {
        qCWarning(dcZigbeeAdapterMonitor()) << "Could not set subsystem device type filter to tty.";
        udev_monitor_unref(m_monitor);
        m_monitor = nullptr;
        udev_unref(m_udev);
        m_udev = nullptr;
        return;
    }

    // Enable the monitor
    if (udev_monitor_enable_receiving(m_monitor) < 0) {
        qCWarning(dcZigbeeAdapterMonitor()) << "Could not enable udev monitor.";
        udev_monitor_unref(m_monitor);
        m_monitor = nullptr;
        udev_unref(m_udev);
        m_udev = nullptr;
        return;
    }

    // Create socket notifier for read
    int socketDescriptor = udev_monitor_get_fd(m_monitor);
    m_notifier = new QSocketNotifier(socketDescriptor, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, [this, socketDescriptor](int socket){
        // Make sure the socket matches
        if (socketDescriptor != socket) {
            qCWarning(dcZigbeeAdapterMonitor()) << "socket != socketdescriptor";
            return;
        }

        // Create udev device
        udev_device *device = udev_monitor_receive_device(m_monitor);
        if (!device) {
            qCWarning(dcZigbeeAdapterMonitor()) << "Got socket sotification but could not read device information.";
            return;
        }

        QString actionString = QString::fromLatin1(udev_device_get_action(device));
        QString systemPath = QString::fromLatin1(udev_device_get_property_value(device,"DEVNAME"));
        QString manufacturerString = QString::fromLatin1(udev_device_get_property_value(device,"ID_VENDOR_ENC"));
        QString descriptionString = QString::fromLatin1(udev_device_get_property_value(device,"ID_MODEL_ENC"));
        QString serialNumberString = QString::fromLatin1(udev_device_get_property_value(device, "ID_SERIAL_SHORT"));

        // Clean udev device
        udev_device_unref(device);

        // Make sure we know the action
        if (actionString.isEmpty())
            return;

        if (actionString == "add") {
            qCDebug(dcZigbeeAdapterMonitor()) << "[+]" << systemPath << serialNumberString;
            if (!m_availableAdapters.contains(systemPath)) {
                addAdapterInternally(systemPath);
            }
        }

        if (actionString == "remove") {
            qCDebug(dcZigbeeAdapterMonitor()) << "[-]" << systemPath << serialNumberString;
            if (m_availableAdapters.contains(systemPath)) {
                ZigbeeUartAdapter adapter = m_availableAdapters.take(systemPath);
                qCDebug(dcZigbeeAdapterMonitor()) << "Removed" << adapter;
                emit adapterRemoved(adapter);
            }
        }
    });

    m_notifier->setEnabled(true);
#endif

    m_isValid = true;
}

ZigbeeUartAdapterMonitor::~ZigbeeUartAdapterMonitor()
{
#ifndef DISABLE_UDEV

    if (m_notifier)
        delete m_notifier;

    if (m_monitor)
        udev_monitor_unref(m_monitor);

    if (m_udev)
        udev_unref(m_udev);
#endif
}

QList<ZigbeeUartAdapter> ZigbeeUartAdapterMonitor::availableAdapters() const
{
    return m_availableAdapters.values();
}

bool ZigbeeUartAdapterMonitor::hasAdapter(const QString &serialPort) const
{
    return m_availableAdapters.contains(serialPort);
}

bool ZigbeeUartAdapterMonitor::isValid() const
{
    return m_isValid;
}

void ZigbeeUartAdapterMonitor::addAdapterInternally(const QString &serialPort)
{
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        if (serialPortInfo.systemLocation() != serialPort)
            continue;

        if (m_availableAdapters.keys().contains(serialPort)) {
            qCWarning(dcZigbeeAdapterMonitor()) << "The adapter" << serialPort << "has already been added to the monitor.";
            continue;
        }

        qCDebug(dcZigbeeAdapterMonitor()) << "Adapter candidate" << serialPortInfo.portName();
        qCDebug(dcZigbeeAdapterMonitor()) << "   Description:" << serialPortInfo.description();
        qCDebug(dcZigbeeAdapterMonitor()) << "   System location:" << serialPortInfo.systemLocation();
        qCDebug(dcZigbeeAdapterMonitor()) << "   Manufacturer:" << serialPortInfo.manufacturer();
        qCDebug(dcZigbeeAdapterMonitor()) << "   Serialnumber:" << serialPortInfo.serialNumber();

        if (serialPortInfo.hasProductIdentifier()) {
            qCDebug(dcZigbeeAdapterMonitor()) << "   Product identifier:" << serialPortInfo.productIdentifier();
        }

        if (serialPortInfo.hasVendorIdentifier()) {
            qCDebug(dcZigbeeAdapterMonitor()) << "   Vendor identifier:" << serialPortInfo.vendorIdentifier();
        }

        // Check if we recognize this controller
        ZigbeeUartAdapter adapter;
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
        adapter.setSerialPort(serialPortInfo.systemLocation());
        adapter.setSerialNumber(serialPortInfo.serialNumber());

        // Check if we recognize this adapter from USB information
        if (serialPortInfo.manufacturer().toLower().contains("dresden elektronik")) {
            adapter.setHardwareRecognized(true);
            adapter.setBackendType(Zigbee::ZigbeeBackendTypeDeconz);
            adapter.setBaudRate(38400);
        } else if (serialPortInfo.manufacturer().toLower().contains("nxp")) {
            adapter.setHardwareRecognized(true);
            adapter.setBackendType(Zigbee::ZigbeeBackendTypeNxp);
            adapter.setBaudRate(115200);
        }

#ifndef DISABLE_TI
        QStringList zStackModels = {"cc2530", "cc2531", "cc2538", "cc1352p", "cc2652p", "cc2652r", "cc2652rb", "sonoff zigbee 3.0 usb"};
        if (QRegularExpression(".*(" + zStackModels.join("|") + ").*").match(serialPortInfo.description().toLower()).hasMatch()) {
            adapter.setHardwareRecognized(true);
            adapter.setBackendType(Zigbee::ZigbeeBackendTypeTi);
            adapter.setBaudRate(115200);
        }
#endif

        qCDebug(dcZigbeeAdapterMonitor()) << "Added" << adapter;
        m_availableAdapters.insert(adapter.serialPort(), adapter);
        emit adapterAdded(adapter);
    }

}
