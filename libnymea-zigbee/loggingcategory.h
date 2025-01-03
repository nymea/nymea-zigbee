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

#ifndef LOGGINGCATEGORY_H
#define LOGGINGCATEGORY_H

#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(dcZigbeeAps)
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeNode)
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeNetwork)
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeCluster)
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeEndpoint)
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeInterface)
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeController)
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeDeviceObject)
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeAdapterMonitor)
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeClusterLibrary)
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeControllerDebug)
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeNetworkDatabase)
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeInterfaceTraffic)

#endif // LOGGINGCATEGORY_H
