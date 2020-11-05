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

#include "zigbeeuartadaptermonitor.h"
#include "loggingcategory.h"

#include <QSerialPortInfo>

ZigbeeUartAdapterMonitor::ZigbeeUartAdapterMonitor(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<ZigbeeUartAdapter>();

    m_udev = udev_new();
    if (!m_udev) {
        qCWarning(dcZigbeeAdapterMonitor()) << "Could not initialize udev for the adapter monitor";
        return;
    }

    // Read initially all tty devices
    struct udev_enumerate *enumerate = udev_enumerate_new(m_udev);
    if (!enumerate) {
        qCWarning(dcZigbeeAdapterMonitor()) << "Could not create udev enumerate for initial device reading for the adapter monitor.";
        udev_unref(m_udev);
        m_udev = nullptr;
        return;
    }

    udev_enumerate_add_match_subsystem(enumerate, "tty");

    if (udev_enumerate_scan_devices(enumerate) < 0) {
        qCWarning(dcZigbeeAdapterMonitor()) << "Failed to scan devices from udev enumerate.";
        udev_enumerate_unref(enumerate);
        enumerate = nullptr;
        udev_unref(m_udev);
        m_udev = nullptr;
        return;
    }

    qCDebug(dcZigbeeAdapterMonitor()) << "Load initial list of available serial ports...";
    struct udev_list_entry *devices = nullptr;
    devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *dev_list_entry = nullptr;
    udev_list_entry_foreach(dev_list_entry, devices) {
        struct udev_device *device = nullptr;
        const char *path;
        path = udev_list_entry_get_name(dev_list_entry);
        device = udev_device_new_from_syspath(m_udev, path);

        // Print properties
        struct udev_list_entry *properties = udev_device_get_properties_list_entry(device);
        struct udev_list_entry *property_list_entry = nullptr;
        udev_list_entry_foreach(property_list_entry, properties) {
            qCDebug(dcZigbeeAdapterMonitor()) << " - Property" << udev_list_entry_get_name(property_list_entry) << udev_list_entry_get_value(property_list_entry);
        }

        QString devicePath = QString::fromLatin1(udev_device_get_property_value(device,"DEVNAME"));
        QString manufacturerString = QString::fromLatin1(udev_device_get_property_value(device,"ID_VENDOR_ENC"));
        QString descriptionString = QString::fromLatin1(udev_device_get_property_value(device,"ID_MODEL_ENC"));
        QString serialNumberString = QString::fromLatin1(udev_device_get_property_value(device, "ID_SERIAL_SHORT"));

        // Clean up this device since we have all information
        udev_device_unref(device);

        qCDebug(dcZigbeeAdapterMonitor()) << "[+]" << devicePath  << manufacturerString << descriptionString << serialNumberString;
        addAdapterInternally(devicePath);
    }

    udev_enumerate_unref(enumerate);
    enumerate = nullptr;

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
        QString devicePath = QString::fromLatin1(udev_device_get_property_value(device,"DEVNAME"));
        QString manufacturerString = QString::fromLatin1(udev_device_get_property_value(device,"ID_VENDOR_ENC"));
        QString descriptionString = QString::fromLatin1(udev_device_get_property_value(device,"ID_MODEL_ENC"));
        QString serialNumberString = QString::fromLatin1(udev_device_get_property_value(device, "ID_SERIAL_SHORT"));

        // Clean udev device
        udev_device_unref(device);

        // Make sure we know the action
        if (actionString.isEmpty())
            return;

        if (actionString == "add") {
            qCDebug(dcZigbeeAdapterMonitor()) << "[+]" << devicePath << serialNumberString;
            if (!m_availableAdapters.contains(devicePath)) {
                addAdapterInternally(devicePath);
            }
        }

        if (actionString == "remove") {
            qCDebug(dcZigbeeAdapterMonitor()) << "[-]" << devicePath << serialNumberString;
            if (m_availableAdapters.contains(devicePath)) {
                ZigbeeUartAdapter adapter = m_availableAdapters.take(devicePath);
                qCDebug(dcZigbeeAdapterMonitor()) << "Removed" << adapter;
                emit adapterRemoved(adapter);
            }
        }
    });

    m_notifier->setEnabled(true);
    m_isValid = true;
}

QList<ZigbeeUartAdapter> ZigbeeUartAdapterMonitor::availableAdapters() const
{
    return m_availableAdapters.values();
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

        qCDebug(dcZigbeeAdapterMonitor()) << "Added" << adapter;
        m_availableAdapters.insert(adapter.serialPort(), adapter);
        emit adapterAdded(adapter);
    }

}
