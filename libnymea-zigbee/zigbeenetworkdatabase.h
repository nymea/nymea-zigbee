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

#ifndef ZIGBEENETWORKDATABASE_H
#define ZIGBEENETWORKDATABASE_H

#include <QObject>
#include <QSqlDatabase>

#define DB_VERSION 1

class ZigbeeNode;
class ZigbeeCluster;
class ZigbeeNetwork;
class ZigbeeNodeEndpoint;
class ZigbeeClusterAttribute;

class QSqlDatabase;

class ZigbeeNetworkDatabase : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeNetworkDatabase(ZigbeeNetwork *network, const QString &databaseName, QObject *parent = nullptr);
    ~ZigbeeNetworkDatabase();

    QString databaseName() const;

    QList<ZigbeeNode *> loadNodes();

    bool wipeDatabase();

private:
    ZigbeeNetwork *m_network = nullptr;
    QString m_databaseName;
    QString m_connectionName;
    QSqlDatabase m_db;

    bool initDatabase();
    void createTable(const QString &tableName, const QString &schema);
    void createIndices(const QString &indexName, const QString &tableName, const QString &columns);

public slots:
    bool saveNodeEndpoint(ZigbeeNodeEndpoint *endpoint);
    bool saveInputCluster(ZigbeeCluster *cluster);
    bool saveOutputCluster(ZigbeeCluster *cluster);
    bool saveAttribute(ZigbeeCluster *cluster, const ZigbeeClusterAttribute &attribute);
    bool saveNode(ZigbeeNode *node);
    bool updateNodeLqi(ZigbeeNode *node, quint8 lqi);
    bool updateNodeNetworkAddress(ZigbeeNode *node, quint16 networkAddress);
    bool updateNodeLastSeen(ZigbeeNode *node, const QDateTime &lastSeen);
    bool updateNodeBindingTable(ZigbeeNode *node);
    bool removeNode(ZigbeeNode *node);

};

#endif // ZIGBEENETWORKDATABASE_H
