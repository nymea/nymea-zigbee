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
    bool updateNodeLastSeen(ZigbeeNode *node, const QDateTime &lastSeen);
    bool removeNode(ZigbeeNode *node);

};

#endif // ZIGBEENETWORKDATABASE_H
