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

#ifndef ZIGBEECLUSTERSCENES_H
#define ZIGBEECLUSTERSCENES_H

#include <QObject>

#include "zcl/zigbeecluster.h"

class ZigbeeClusterScenes : public ZigbeeCluster
{
    Q_OBJECT
public:
    enum Attribute {
        AttributeSceneCount = 0x0000,
        AttributeCurrentScene = 0x0001,
        AttributeCurrentGroup = 0x0002,
        AttributeSceneValid = 0x0003,
        AttributeNameSupported = 0x0004,
        AttributeLastConfiguredBy = 0x0005 // Optional
    };
    Q_ENUM(Attribute)

    enum Command {
        CommandAddScene = 0x00,
        CommandViewScene = 0x01,
        CommandRemoveScene = 0x02,
        CommandRemoveAllScenes = 0x03,
        CommandStoreScene = 0x04,
        CommandRecallScene = 0x05,
        CommandGetSceneMembership = 0x06,
        CommandEnhancedAddScene= 0x40, // O
        CommandEnhancedViewScene= 0x41, // O
        CommandCopyScene= 0x42 // O
    };
    Q_ENUM(Command)

    explicit ZigbeeClusterScenes(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent = nullptr);

signals:
    void commandSent(quint8 command, const QByteArray &payload);

private:
    void setAttribute(const ZigbeeClusterAttribute &attribute) override;

protected:
    void processDataIndication(ZigbeeClusterLibrary::Frame frame) override;

};

#endif // ZIGBEECLUSTERSCENES_H
