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

#ifndef ZIGBEECLUSTER_H
#define ZIGBEECLUSTER_H

#include <QObject>

#include "zigbee.h"
#include "zigbeeclusterreply.h"
#include "zigbeeclusterlibrary.h"
#include "zigbeeclusterattribute.h"

struct ZigbeeClusterReportConfigurationRecord {
    quint8 direction;
    Zigbee::DataType dataType;
    quint16 attributeId;
    quint16 minInterval;
    quint16 maxInterval;
    quint16 timeout;
    quint8 change;
};

typedef struct ZigbeeClusterAttributeReport {
    quint16 sourceAddress;
    quint8 endpointId;
    ZigbeeClusterLibrary::ClusterId clusterId;
    quint16 attributeId;
    ZigbeeClusterLibrary::Status attributeStatus;
    Zigbee::DataType dataType;
    QByteArray data;
} ZigbeeClusterAttributeReport;

class ZigbeeNode;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeNetworkReply;

class ZigbeeCluster : public QObject
{
    Q_OBJECT

    friend class ZigbeeNode;
    friend class ZigbeeNetwork;
    friend class ZigbeeNetworkDatabase;

public:
    enum Direction {
        Server,
        Client
    };
    Q_ENUM(Direction)

    explicit ZigbeeCluster(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, ZigbeeClusterLibrary::ClusterId clusterId, Direction direction, QObject *parent = nullptr);

    ZigbeeNode *node() const;
    ZigbeeNodeEndpoint *endpoint() const;

    Direction direction() const;

    ZigbeeClusterLibrary::ClusterId clusterId() const;
    QString clusterName() const;

    QList<ZigbeeClusterAttribute> attributes() const;
    bool hasAttribute(quint16 attributeId) const;
    ZigbeeClusterAttribute attribute(quint16 attributeId);

    // ZCL global commands
    ZigbeeClusterReply *readAttributes(QList<quint16> attributes, quint16 manufacturerCode = 0x0000);
    ZigbeeClusterReply *writeAttributes(QList<ZigbeeClusterLibrary::WriteAttributeRecord> writeAttributeRecords, quint16 manufacturerCode = 0x0000);
    ZigbeeClusterReply *configureReporting(QList<ZigbeeClusterLibrary::AttributeReportingConfiguration> reportingConfigurations, quint16 manufacturerCode = 0x0000);

    // Helper methods for sending cluster specific commands
    ZigbeeNetworkRequest createGeneralRequest();

    // Global commands
    ZigbeeClusterReply *executeGlobalCommand(quint8 command, const QByteArray &payload = QByteArray(), quint16 manufacturerCode = 0x0000, quint8 transactionSequenceNumber = newTransactionSequenceNumber());

    // Cluster specific
    ZigbeeClusterReply *createClusterReply(const ZigbeeNetworkRequest &request, ZigbeeClusterLibrary::Frame frame);
    ZigbeeClusterReply *executeClusterCommand(quint8 command, const QByteArray &payload = QByteArray(), ZigbeeClusterLibrary::Direction direction = ZigbeeClusterLibrary::DirectionClientToServer, bool disableDefaultResponse = false);

    ZigbeeClusterReply *sendClusterServerResponse(quint8 command, quint8 transactionSequenceNumber, const QByteArray &payload = QByteArray());
    ZigbeeClusterReply *sendDefaultResponse(quint8 transactionSequenceNumber, quint8 command, quint8 status);

    bool verifyNetworkError(ZigbeeClusterReply *zclReply, ZigbeeNetworkReply *networkReply);
    void finishZclReply(ZigbeeClusterReply *zclReply);

protected:
    ZigbeeNetwork *m_network = nullptr;
    ZigbeeNode *m_node = nullptr;
    ZigbeeNodeEndpoint *m_endpoint= nullptr;

    ZigbeeClusterLibrary::ClusterId m_clusterId = ZigbeeClusterLibrary::ClusterIdUnknown;
    Direction m_direction = Server;
    QHash<quint16, ZigbeeClusterAttribute> m_attributes;

    QHash<quint8, ZigbeeClusterReply *> m_pendingReplies;

    virtual void processDataIndication(ZigbeeClusterLibrary::Frame frame);

    virtual void setAttribute(const ZigbeeClusterAttribute &attribute);

    static quint8 newTransactionSequenceNumber();

signals:
    void attributeChanged(const ZigbeeClusterAttribute &attribute);
    void dataIndication(const ZigbeeClusterLibrary::Frame &frame);

public slots:
    void processApsDataIndication(const QByteArray &asdu, const ZigbeeClusterLibrary::Frame &frame);

};

QDebug operator<<(QDebug debug, ZigbeeCluster *cluster);
QDebug operator<<(QDebug debug, const ZigbeeClusterAttributeReport &attributeReport);

#endif // ZIGBEECLUSTER_H
