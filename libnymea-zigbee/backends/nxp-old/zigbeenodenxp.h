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

#ifndef ZIGBEENODENXP_H
#define ZIGBEENODENXP_H

#include <QObject>

#include "zigbeenode.h"
#include "zigbeenodeendpointnxp.h"
#include "zigbeebridgecontrollernxp.h"

class ZigbeeNodeNxp : public ZigbeeNode
{
    Q_OBJECT

    friend class ZigbeeNetworkNxp;

public:
    enum InitState {
        InitStateNone,
        InitStateError,
        InitStateNodeDescriptor,
        InitStatePowerDescriptor,
        InitStateActiveEndpoints,
        InitStateSimpleDescriptors,
        InitStateReadBasicClusterAttributes
    };
    Q_ENUM(InitState)

    explicit ZigbeeNodeNxp(ZigbeeBridgeControllerNxp *controller, QObject *parent = nullptr);

    void leaveNetworkRequest(bool rejoin = false, bool removeChildren = false) override;

private:
    ZigbeeBridgeControllerNxp *m_controller = nullptr;
    InitState m_initState = InitStateNone;
    int m_initStateRetry = 0;

    QList<quint8> m_uninitializedEndpoints;
    QList<quint16> m_uninitalizedBasicClusterAttributes;

    void setInitState(InitState initState);
    void setClusterAttributeReport(const ZigbeeClusterAttributeReport &report) override;

protected:
    void startInitialization() override;
    ZigbeeNodeEndpoint *createNodeEndpoint(quint8 endpointId, QObject *parent) override;

};

#endif // ZIGBEENODENXP_H
