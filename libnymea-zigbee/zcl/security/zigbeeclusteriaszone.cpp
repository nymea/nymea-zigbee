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

#include "zigbeeclusteriaszone.h"
#include "zigbeenetworkreply.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"
#include "zigbeeutils.h"

#include <QDataStream>

ZigbeeClusterIasZone::ZigbeeClusterIasZone(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, ZigbeeCluster::Direction direction, QObject *parent) :
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdIasZone, direction, parent)
{

}

ZigbeeClusterIasZone::ZoneState ZigbeeClusterIasZone::zoneState() const
{
    return m_zoneState;
}

ZigbeeClusterIasZone::ZoneType ZigbeeClusterIasZone::zoneType() const
{
    return m_zoneType;
}

ZigbeeClusterIasZone::ZoneStatusFlags ZigbeeClusterIasZone::zoneStatus() const
{
    return m_zoneStatus;
}

ZigbeeClusterReply *ZigbeeClusterIasZone::sendZoneEnrollRequest(ZigbeeClusterIasZone::ZoneType zoneType, quint16 manufacturerCode)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint16>(zoneType);
    stream << manufacturerCode;
    ZigbeeClusterReply *reply = executeClusterCommand(ServerCommandZoneEnrollRequest, payload);
    return reply;
}

ZigbeeClusterReply* ZigbeeClusterIasZone::sendZoneEnrollResponse(quint8 zoneId, EnrollResponseCode code)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint8>(code);
    stream << zoneId;
    ZigbeeClusterReply *reply = executeClusterCommand(ClientCommandEnrollResponse, payload);
    return reply;
}

ZigbeeClusterReply *ZigbeeClusterIasZone::sendZoneStatusChangeNotification(ZigbeeClusterIasZone::ZoneStatus status, quint8 zoneId, quint16 delay)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint16>(status);
    stream << static_cast<quint8>(0); // extended status, reserved for future use
    stream << zoneId;
    stream << delay;
    ZigbeeClusterReply *reply = executeClusterCommand(ClientCommandEnrollResponse, payload);
    return reply;
}

void ZigbeeClusterIasZone::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    if (attribute.id() == AttributeZoneState) {
        quint8 zoneStateInt = attribute.dataType().toUInt8();
        m_zoneState = static_cast<ZoneState>(zoneStateInt);
        qCDebug(dcZigbeeCluster()) << m_node << m_endpoint << m_zoneState;
    } else if (attribute.id() == AttributeZoneType) {
        quint16 zoneTypeInt = attribute.dataType().toUInt16();
        m_zoneType = static_cast<ZoneType>(zoneTypeInt);
        qCDebug(dcZigbeeCluster()) << m_node << m_endpoint << m_zoneType;
    } else if (attribute.id() == AttributeZoneStatus) {
        quint16 zoneStatusInt = attribute.dataType().toUInt16();
        m_zoneStatus = ZoneStatusFlags(zoneStatusInt);
        qCDebug(dcZigbeeCluster()) << m_node << m_endpoint << m_zoneStatus;
    }
}

void ZigbeeClusterIasZone::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    qCDebug(dcZigbeeCluster()) << "Processing cluster frame" << m_node << m_endpoint << this << frame;

    switch (m_direction) {
    case Client:
        // TODO: handle client frames
        break;
    case Server:
        // If the client cluster sends data to a server cluster (independent which), the command was executed on the device like button pressed
        if (frame.header.frameControl.direction == ZigbeeClusterLibrary::DirectionServerToClient) {
            // Read the payload which is
            ServerCommand command = static_cast<ServerCommand>(frame.header.command);
            qCDebug(dcZigbeeCluster()) << "Command received from" << m_node << m_endpoint << this << command;
            switch (command) {
            case ServerCommandStatusChangedNotification: {
                QDataStream stream(frame.payload);
                stream.setByteOrder(QDataStream::LittleEndian);
                quint16 zoneStatus = 0; quint8 extendedStatus = 0; quint8 zoneId = 0xff; quint16 delay = 0;
                stream >> zoneStatus >> extendedStatus >> zoneId >> delay;
                qCDebug(dcZigbeeCluster()) << "IAS zone status notification from" << m_node << m_endpoint << this
                                           << ZoneStatusFlags(zoneStatus) << "Extended status:" << ZigbeeUtils::convertByteToHexString(extendedStatus)
                                           << "Zone ID:" << ZigbeeUtils::convertByteToHexString(zoneId) << "Delay:" << delay << "[s/4]";

                // Update the ZoneState attribute
                setAttribute(ZigbeeClusterAttribute(AttributeZoneState, ZigbeeDataType(Zigbee::BitMap16, frame.payload.left(2))));
                emit zoneStatusChanged(ZoneStatusFlags(zoneStatus), extendedStatus, zoneId, delay);

                // Respond with default response if enabled
                if (!frame.header.frameControl.disableDefaultResponse) {
                    // Send the default response with success back to the cluster
                    ZigbeeClusterReply *reply = sendDefaultResponse(frame.header.transactionSequenceNumber, command, ZigbeeClusterLibrary::StatusSuccess);
                    connect(reply, &ZigbeeClusterReply::finished, this, [](){
                        qCDebug(dcZigbeeCluster()) << "Default response sent successfully to the IAS zone status notification.";
                    });
                }

                break;
            }
            case ServerCommandZoneEnrollRequest: {
                QDataStream stream(frame.payload);
                stream.setByteOrder(QDataStream::LittleEndian);
                quint16 zoneTypeInt = 0; quint16 manufacturerCode = 0;
                stream >> zoneTypeInt >> manufacturerCode;
                ZoneType zoneType = static_cast<ZoneType>(zoneTypeInt);
                qCDebug(dcZigbeeCluster()) << "IAS zone enroll request from" << m_node << m_endpoint << this
                                           << zoneType << "Manufacturer code:" << ZigbeeUtils::convertUint16ToHexString(manufacturerCode);
                // Update the ZoneState attribute
                setAttribute(ZigbeeClusterAttribute(AttributeZoneType, ZigbeeDataType(Zigbee::Enum16, frame.payload.left(2))));
                emit zoneEnrollRequest(zoneType, manufacturerCode, frame.header.transactionSequenceNumber);

                // Respond with default response if enabled
                if (!frame.header.frameControl.disableDefaultResponse) {
                    // Send the default response with success back to the cluster
                    ZigbeeClusterReply *reply = sendDefaultResponse(frame.header.transactionSequenceNumber, command, ZigbeeClusterLibrary::StatusSuccess);
                    connect(reply, &ZigbeeClusterReply::finished, this, [](){
                        qCDebug(dcZigbeeCluster()) << "Default response sent successfully to the IAS zone status notification.";
                    });
                }
                break;
            }
            }
        }
        break;
    }

}
