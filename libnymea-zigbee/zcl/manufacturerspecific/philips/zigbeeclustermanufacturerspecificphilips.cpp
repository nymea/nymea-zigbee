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

#include "zigbeeclustermanufacturerspecificphilips.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeClusterManufacturerSpecificPhilips::ZigbeeClusterManufacturerSpecificPhilips(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdManufacturerSpecificPhilips, direction, parent)
{

}

void ZigbeeClusterManufacturerSpecificPhilips::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    qCDebug(dcZigbeeCluster()) << "Processing manufacturer specific (Philips) cluster frame" << m_node << m_endpoint << this << frame;

    // Increase the tsn for continuous id increasing on both sides
    m_transactionSequenceNumber = frame.header.transactionSequenceNumber;

    switch (m_direction) {
    case Client:
        qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
        break;
    case Server:
        if (frame.header.frameControl.direction == ZigbeeClusterLibrary::DirectionServerToClient) {
            Command command = static_cast<Command>(frame.header.command);
            switch (command) {
            case CommandButtonPress: {
                QDataStream payloadStream(frame.payload);
                payloadStream.setByteOrder(QDataStream::LittleEndian);
                quint8 button; quint16 unknown1; quint8 unknown2; quint8 operation;
                payloadStream >> button >> unknown1 >> unknown2 >> operation;
                qCDebug(dcZigbeeCluster()) << "Received manufacturer specific (Philips) button press. Button:" << button << "Operation:" << operation;
                emit buttonPressed(button, Operation(operation));
                break;
            }
            default:
                qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
            }
        } else {
            qCWarning(dcZigbeeCluster()) << "Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
        }
        break;
    }
}
