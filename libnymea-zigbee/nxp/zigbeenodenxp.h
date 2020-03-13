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
        InitStateNodeDescriptor,
        InitStatePowerDescriptor,
        InitStateActiveEndpoints,
        InitStateSimpleDescriptors,
        InitStateReadClusterAttributes
    };
    Q_ENUM(InitState)

    explicit ZigbeeNodeNxp(ZigbeeBridgeControllerNxp *controller, QObject *parent = nullptr);

private:
    ZigbeeBridgeControllerNxp *m_controller = nullptr;
    InitState m_initState = InitStateNone;

    QList<quint8> m_uninitializedEndpoints;

    void setInitState(InitState initState);
    void setClusterAttributeReport(const ZigbeeClusterAttributeReport &report) override;

protected:
    void startInitialization() override;
    ZigbeeNodeEndpoint *createNodeEndpoint(quint8 endpointId, QObject *parent) override;

};

#endif // ZIGBEENODENXP_H
