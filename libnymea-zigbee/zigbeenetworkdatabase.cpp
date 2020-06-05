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

#include "zigbeenetworkdatabase.h"
#include "loggingcategory.h"
#include "zigbeenetwork.h"
#include "zigbeeutils.h"
#include "zigbeenode.h"

#include <QSqlError>
#include <QSqlQuery>

ZigbeeNetworkDatabase::ZigbeeNetworkDatabase(ZigbeeNetwork *network, const QString &databaseName, QObject *parent) :
    QObject(parent),
    m_network(network)
{
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), "zigbee");
    m_db.setDatabaseName(databaseName);
    qCDebug(dcZigbeeNetworkDatabase()) << "Opening zigbee network database" << m_db.databaseName();

    if (!m_db.isValid()) {
        qCWarning(dcZigbeeNetworkDatabase()) << "The zigbee network database is not valid" << m_db.databaseName();
        // FIXME: rotate database
        return;
    }

    if (!initDatabase()) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Failed to initialize the database" << m_db.databaseName();
        // FIXME: rotate database
        return;
    }
}

QList<ZigbeeNode *> ZigbeeNetworkDatabase::loadNodes()
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Loading nodes from database" << m_db.databaseName();
    QList<ZigbeeNode *> nodes;
    QString query("SELECT * FROM nodes;");
    QSqlQuery nodesQuery = m_db.exec(query);
    while (nodesQuery.next()) {
        quint64 ieeeAddress = nodesQuery.value("ieeeAddress").toULongLong();
        quint16 shortAddress = nodesQuery.value("shortAddress").toUInt();
        QByteArray nodeDescriptor = QByteArray::fromBase64(nodesQuery.value("nodeDescriptor").toByteArray());
        quint16 powerDescriptor = nodesQuery.value("powerDescriptor").toUInt();

        // Build the node object
        ZigbeeNode *node = new ZigbeeNode(m_network, shortAddress, ZigbeeAddress(ieeeAddress), m_network);
        node->m_nodeDescriptor = ZigbeeDeviceProfile::parseNodeDescriptor(nodeDescriptor);
        node->m_powerDescriptor = ZigbeeDeviceProfile::parsePowerDescriptor(powerDescriptor);

        qCDebug(dcZigbeeNetworkDatabase()) << "Loaded node" << node;

        // Now load all endpoints for this node
        query = QString("SELECT * FROM endpoints WHERE ieeeAddress = \"%1\";").arg(ieeeAddress);
        QSqlQuery endpointsQuery = m_db.exec(query);
        while (endpointsQuery.next()) {
            quint8 endpointId = endpointsQuery.value("endpointId").toUInt();
            ZigbeeNodeEndpoint *endpoint = new ZigbeeNodeEndpoint(m_network, node, endpointId, node);
            endpoint->setProfile(static_cast<Zigbee::ZigbeeProfile>(endpointsQuery.value("profileId").toUInt()));
            endpoint->setDeviceId(static_cast<Zigbee::ZigbeeProfile>(endpointsQuery.value("deviceId").toUInt()));
            endpoint->setDeviceVersion(static_cast<Zigbee::ZigbeeProfile>(endpointsQuery.value("deviceVersion").toUInt()));

            qCDebug(dcZigbeeNetworkDatabase()) << "Loaded endpoint" << endpoint;

            // Load input clusters for this endpoint
            query = QString("SELECT * FROM serverClusters WHERE endpointId = (SELECT id FROM endpoints WHERE ieeeAddress = \"%1\" AND endpointId = \"%2\");")
                    .arg(ieeeAddress)
                    .arg(endpointId);
            QSqlQuery inputClustersQuery = m_db.exec(query);
            while (inputClustersQuery.next()) {
                Zigbee::ClusterId clusterId = static_cast<Zigbee::ClusterId>(inputClustersQuery.value("clusterId").toUInt());
                ZigbeeCluster *cluster = endpoint->createCluster(clusterId, ZigbeeCluster::Server);
                endpoint->addInputCluster(cluster);

                // Load cluster attributes for the server cluster
                query = QString("SELECT * FROM attributes WHERE clusterId = (SELECT id FROM serverClusters WHERE endpointId = (SELECT id FROM endpoints WHERE ieeeAddress = \"%1\" AND endpointId = \"%2\") AND clusterId = \"%3\"));")
                        .arg(ieeeAddress)
                        .arg(endpointId)
                        .arg(cluster->clusterId());
                QSqlQuery attributesQuery = m_db.exec(query);
                while (attributesQuery.next()) {
                    quint16 attributeId = attributesQuery.value("attributeId").toUInt();
                    Zigbee::DataType type = static_cast<Zigbee::DataType>(attributesQuery.value("dataType").toUInt());
                    QByteArray data = attributesQuery.value("data").toByteArray();
                    cluster->setAttribute(ZigbeeClusterAttribute(attributeId, ZigbeeDataType(type, data)));
                }
            }

            // Load output clusters for this endpoint
            query = QString("SELECT * FROM clientClusters WHERE endpointId = (SELECT id FROM endpoints WHERE ieeeAddress = \"%1\" AND endpointId = \"%2\");")
                    .arg(ieeeAddress)
                    .arg(endpointId);
            QSqlQuery outputClustersQuery = m_db.exec(query);
            while (outputClustersQuery.next()) {
                Zigbee::ClusterId clusterId = static_cast<Zigbee::ClusterId>(outputClustersQuery.value("clusterId").toUInt());
                ZigbeeCluster *cluster = endpoint->createCluster(clusterId, ZigbeeCluster::Client);
                endpoint->addOutputCluster(cluster);
            }

            node->m_endpoints.append(endpoint);
        }
        nodes.append(node);
    }

    return nodes;
}

bool ZigbeeNetworkDatabase::initDatabase()
{
    // Reopen the db to make sure it can be written
    m_db.close();
    if (!m_db.open()) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not open zigbee database" << m_db.databaseName() << "Initialization failed.";
        return false;
    }

    // Write pragmas
    m_db.exec("PRAGMA foreign_keys = ON;");

    // Create nodes table
    createTable("nodes",
                "(ieeeAddress INTEGER PRIMARY KEY, " // uint64
                "shortAddress INTEGER NOT NULL, " // uint16
                "nodeDescriptor BLOB NOT NULL, " // bytes as received from the node
                "powerDescriptor INTEGER NOT NULL)"); // uint16
    createIndices("ieeeAddressIndex", "nodes", "ieeeAddress");


    // Create endpoints table
    createTable("endpoints",
                "(id INTEGER PRIMARY KEY AUTOINCREMENT, " // for db relation
                "ieeeAddress INTEGER NOT NULL, " // // reference to nodes.ieeeAddress
                "endpointId INTEGER NOT NULL, " // uint8
                "profileId INTEGER NOT NULL, " // uint16
                "deviceId INTEGER NOT NULL, " // uint16
                "deviceVersion INTEGER, " // uint8
                "CONSTRAINT fk_ieeeAddress FOREIGN KEY(ieeeAddress) REFERENCES nodes(ieeeAddress) ON DELETE CASCADE)");
    createIndices("endpointIndex", "endpoints", "ieeeAddress, endpointId");


    // Create server cluster table
    createTable("serverClusters",
                "(id INTEGER PRIMARY KEY AUTOINCREMENT, " // for db relation
                "endpointId INTEGER NOT NULL, " // reference to endpoint.id
                "clusterId INTEGER NOT NULL, " // uint16
                "CONSTRAINT fk_endpoint FOREIGN KEY(endpointId) REFERENCES endpoints(id) ON DELETE CASCADE)");
    createIndices("serverClusterIndex", "serverClusters", "endpointId, clusterId");

    // Create client cluster table
    createTable("clientClusters",
                "(id INTEGER PRIMARY KEY AUTOINCREMENT, " // for db relation
                "endpointId INTEGER NOT NULL, " // reference to endpoint.id
                "clusterId INTEGER NOT NULL, " // uint16
                "CONSTRAINT fk_endpoint FOREIGN KEY(endpointId) REFERENCES endpoints(id) ON DELETE CASCADE)");
    createIndices("clientClusterIndex", "clientClusters", "endpointId, clusterId");

    // Create cluster attributes table
    createTable("attributes",
                "(id INTEGER PRIMARY KEY AUTOINCREMENT, " // for db relation
                "clusterId INTEGER NOT NULL, " // reference to serverClusters.id
                "attributeId INTEGER NOT NULL, " // uint16
                "dataType INTEGER NOT NULL, " // uint8
                "data BLOB NOT NULL, " // raw data from attribute
                "CONSTRAINT fk_cluster FOREIGN KEY(clusterId) REFERENCES serverClusters(id) ON DELETE CASCADE)");
    createIndices("attributesIndex", "attributes", "clusterId, attributeId");

    return true;
}

void ZigbeeNetworkDatabase::createTable(const QString &tableName, const QString &schema)
{
    m_db.exec(QString("CREATE TABLE IF NOT EXISTS %1 %2;").arg(tableName).arg(schema));
    m_db.exec(QString("PRAGMA schema_version = %1;").arg(DB_VERSION));
    m_db.exec(QString("PRAGMA user_version = %1;").arg(DB_VERSION));
}

void ZigbeeNetworkDatabase::createIndices(const QString &indexName, const QString &tableName, const QString &columns)
{
    m_db.exec(QString("CREATE UNIQUE INDEX IF NOT EXISTS %1 ON %2(%3);").arg(indexName).arg(tableName).arg(columns));
}

bool ZigbeeNetworkDatabase::saveNodeEndpoint(ZigbeeNodeEndpoint *endpoint)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Store" << endpoint;
    QString queryString = QString("INSERT OR REPLACE INTO endpoints (ieeeAddress, endpointId, profileId, deviceId, deviceVersion) "
                                  "VALUES (\"%1\", \"%2\", \"%3\", \"%4\", \"%5\");")
            .arg(endpoint->node()->extendedAddress().toUInt64())
            .arg(endpoint->endpointId())
            .arg(static_cast<quint16>(endpoint->profile()))
            .arg(static_cast<quint16>(endpoint->deviceId()))
            .arg(static_cast<quint8>(endpoint->deviceVersion()));

    m_db.exec(queryString);
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not save node into database." << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    // Save input/output clusters
    foreach(ZigbeeCluster *cluster, endpoint->inputClusters()) {
        if (!saveInputCluster(cluster)) {
            return false;
        }

        foreach(const ZigbeeClusterAttribute &attribute, cluster->attributes()) {
            if (!saveAttribute(cluster, attribute)) {
                return false;
            }
        }
    }

    foreach(ZigbeeCluster *cluster, endpoint->outputClusters()) {
        if (!saveOutputCluster(cluster)) {
            return false;
        }
    }

    return true;
}

bool ZigbeeNetworkDatabase::saveInputCluster(ZigbeeCluster *cluster)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Store" << cluster;
    QString endpointIdReferenceQuery = QString("(SELECT id FROM endpoints WHERE ieeeAddress = \"%1\" AND endpointId = \"%2\")")
            .arg(cluster->node()->extendedAddress().toUInt64())
            .arg(cluster->endpoint()->endpointId());
    QString queryString = QString("INSERT OR REPLACE INTO serverClusters (endpointId, clusterId) VALUES (%1, \"%2\");")
            .arg(endpointIdReferenceQuery)
            .arg(static_cast<quint16>(cluster->clusterId()));
    m_db.exec(queryString);
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not save input cluster into database." << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    return true;
}

bool ZigbeeNetworkDatabase::saveOutputCluster(ZigbeeCluster *cluster)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Store" << cluster;
    QString endpointIdReferenceQuery = QString("(SELECT id FROM endpoints WHERE ieeeAddress = \"%1\" AND endpointId = \"%2\")")
            .arg(cluster->node()->extendedAddress().toUInt64())
            .arg(cluster->endpoint()->endpointId());
    QString queryString = QString("INSERT OR REPLACE INTO clientClusters (endpointId, clusterId) VALUES (%1, \"%2\");")
            .arg(endpointIdReferenceQuery)
            .arg(static_cast<quint16>(cluster->clusterId()));

    m_db.exec(queryString);
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not save output cluster into database." << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    return true;
}

bool ZigbeeNetworkDatabase::saveAttribute(ZigbeeCluster *cluster, const ZigbeeClusterAttribute &attribute)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Store" << cluster << attribute;
    QString serverClusterIdReferenceQuery = QString("(SELECT id FROM serverClusters "
                                                    "WHERE endpointId = (SELECT id FROM endpoints WHERE ieeeAddress = \"%1\" AND endpointId = \"%2\")"
                                                    "AND clusterId = \"%3\")")
            .arg(cluster->node()->extendedAddress().toUInt64())
            .arg(cluster->endpoint()->endpointId())
            .arg(cluster->clusterId());

    QString queryString = QString("INSERT OR REPLACE INTO attributes (clusterId, attributeId, dataType, data) "
                                  "VALUES (%1, \"%2\", \"%3\", \"%4\");")
            .arg(serverClusterIdReferenceQuery)
            .arg(static_cast<quint16>(attribute.id()))
            .arg(static_cast<quint8>(attribute.dataType().dataType()))
            .arg((attribute.dataType().data().toBase64().data()));

    m_db.exec(queryString);
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not save cluster cluster attribute into database." << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    return true;
}

bool ZigbeeNetworkDatabase::saveNode(ZigbeeNode *node)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Store" << node;
    QString queryString = QString("INSERT OR REPLACE INTO nodes (ieeeAddress, shortAddress, nodeDescriptor, powerDescriptor) "
                                  "VALUES (\"%1\", \"%2\", \"%3\", \"%4\");")
            .arg(node->extendedAddress().toUInt64())
            .arg(node->shortAddress())
            .arg(node->nodeDescriptor().descriptorRawData.toBase64().data()) // Note: convert to base64 for saving zeros as string
            .arg(node->powerDescriptor().powerDescriptoFlag);

    m_db.exec(queryString);
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not save node into database." << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    // Save endpoints
    foreach (ZigbeeNodeEndpoint *endpoint, node->endpoints()) {
        if (!saveNodeEndpoint(endpoint)) {
            return false;
        }
    }

    return true;
}

bool ZigbeeNetworkDatabase::removeNode(ZigbeeNode *node)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Remove" << node;
    // Note: cascade delete will clean up all other tables
    QString queryString = QString("DELETE FROM nodes WHERE ieeeAddress = %1;").arg(node->extendedAddress().toUInt64());
    m_db.exec(queryString);
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not remove node from database." << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    return true;
}


