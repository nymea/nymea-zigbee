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
    m_network(network),
    m_databaseName(databaseName)
{
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), "zigbee");
    m_db.setDatabaseName(m_databaseName);
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

ZigbeeNetworkDatabase::~ZigbeeNetworkDatabase()
{
    if (m_db.isOpen()) {
        qCDebug(dcZigbeeNetworkDatabase()) << "Closing database" << m_db.databaseName();
        m_db.close();
    }
}

QString ZigbeeNetworkDatabase::databaseName() const
{
    return m_databaseName;
}

QList<ZigbeeNode *> ZigbeeNetworkDatabase::loadNodes()
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Loading nodes from database" << m_db.databaseName();
    QList<ZigbeeNode *> nodes;
    QString query("SELECT * FROM nodes;");
    QSqlQuery nodesQuery = m_db.exec(query);
    while (nodesQuery.next()) {
        QString ieeeAddress = nodesQuery.value("ieeeAddress").toString();
        quint16 shortAddress = nodesQuery.value("shortAddress").toUInt();
        QByteArray nodeDescriptor = QByteArray::fromBase64(nodesQuery.value("nodeDescriptor").toByteArray());
        quint16 powerDescriptor = nodesQuery.value("powerDescriptor").toUInt();
        quint8 lqi = nodesQuery.value("lqi").toUInt();
        quint64 lastSeen = nodesQuery.value("timestamp").toULongLong();

        // Build the node object
        ZigbeeNode *node = new ZigbeeNode(m_network, shortAddress, ZigbeeAddress(ieeeAddress), m_network);
        node->m_nodeDescriptor = ZigbeeDeviceProfile::parseNodeDescriptor(nodeDescriptor);
        node->m_macCapabilities = node->nodeDescriptor().macCapabilities;
        node->m_powerDescriptor = ZigbeeDeviceProfile::parsePowerDescriptor(powerDescriptor);
        node->m_lqi = lqi;
        node->m_lastSeen = QDateTime::fromMSecsSinceEpoch(lastSeen * 1000);

        qCDebug(dcZigbeeNetworkDatabase()) << "Loaded" << node;

        // Now load all endpoints for this node
        query = QString("SELECT * FROM endpoints WHERE ieeeAddress = \"%1\";").arg(ieeeAddress);
        QSqlQuery endpointsQuery = m_db.exec(query);
        while (endpointsQuery.next()) {
            quint8 endpointId = endpointsQuery.value("endpointId").toUInt();
            ZigbeeNodeEndpoint *endpoint = new ZigbeeNodeEndpoint(m_network, node, endpointId, node);
            endpoint->setProfile(static_cast<Zigbee::ZigbeeProfile>(endpointsQuery.value("profileId").toUInt()));
            endpoint->setDeviceId(static_cast<Zigbee::ZigbeeProfile>(endpointsQuery.value("deviceId").toUInt()));
            endpoint->setDeviceVersion(static_cast<Zigbee::ZigbeeProfile>(endpointsQuery.value("deviceVersion").toUInt()));

            qCDebug(dcZigbeeNetworkDatabase()) << "Loaded" << endpoint;

            // Load input clusters for this endpoint
            query = QString("SELECT * FROM serverClusters WHERE endpointId = (SELECT id FROM endpoints WHERE ieeeAddress = \"%1\" AND endpointId = \"%2\");")
                    .arg(ieeeAddress)
                    .arg(endpointId);
            QSqlQuery inputClustersQuery = m_db.exec(query);
            while (inputClustersQuery.next()) {
                ZigbeeClusterLibrary::ClusterId clusterId = static_cast<ZigbeeClusterLibrary::ClusterId>(inputClustersQuery.value("clusterId").toUInt());
                ZigbeeCluster *cluster = endpoint->createCluster(clusterId, ZigbeeCluster::Server);
                endpoint->addInputCluster(cluster);

                qCDebug(dcZigbeeNetworkDatabase()) << "Loaded" << cluster;

                // Load cluster attributes for this server cluster
                query = QString("SELECT * FROM attributes WHERE clusterId = (SELECT id FROM serverClusters WHERE endpointId = (SELECT id FROM endpoints WHERE ieeeAddress = \"%1\" AND endpointId = \"%2\") AND clusterId = \"%3\");")
                        .arg(ieeeAddress)
                        .arg(endpointId)
                        .arg(cluster->clusterId());
                QSqlQuery attributesQuery = m_db.exec(query);
                if (m_db.lastError().type() != QSqlError::NoError) {
                    qCWarning(dcZigbeeNetworkDatabase()) << "Could not fetch attributes from database entries." << query << m_db.lastError().databaseText() << m_db.lastError().driverText();
                    continue;
                }


                while (attributesQuery.next()) {
                    quint16 attributeId = attributesQuery.value("attributeId").toUInt();
                    Zigbee::DataType type = static_cast<Zigbee::DataType>(attributesQuery.value("dataType").toUInt());
                    QByteArray data = QByteArray::fromBase64(attributesQuery.value("data").toByteArray());
                    ZigbeeClusterAttribute attribute(attributeId, ZigbeeDataType(type, data));
                    qCDebug(dcZigbeeNetworkDatabase()) << "Loaded" << attribute;
                    cluster->setAttribute(attribute);
                }
            }

            // Set the basic cluster attributes if present
            if (endpoint->hasInputCluster(ZigbeeClusterLibrary::ClusterIdBasic)) {
                ZigbeeClusterBasic *basicCluster = endpoint->inputCluster<ZigbeeClusterBasic>(ZigbeeClusterLibrary::ClusterIdBasic);

                if (basicCluster->hasAttribute(ZigbeeClusterBasic::AttributeManufacturerName))
                    endpoint->m_manufacturerName = basicCluster->attribute(ZigbeeClusterBasic::AttributeManufacturerName).dataType().toString();

                if (basicCluster->hasAttribute(ZigbeeClusterBasic::AttributeModelIdentifier))
                    endpoint->m_modelIdentifier = basicCluster->attribute(ZigbeeClusterBasic::AttributeModelIdentifier).dataType().toString();

                if (basicCluster->hasAttribute(ZigbeeClusterBasic::AttributeSwBuildId))
                    endpoint->m_softwareBuildId = basicCluster->attribute(ZigbeeClusterBasic::AttributeSwBuildId).dataType().toString();

            }

            // Load output clusters for this endpoint
            query = QString("SELECT * FROM clientClusters WHERE endpointId = (SELECT id FROM endpoints WHERE ieeeAddress = \"%1\" AND endpointId = \"%2\");")
                    .arg(ieeeAddress)
                    .arg(endpointId);
            QSqlQuery outputClustersQuery = m_db.exec(query);
            while (outputClustersQuery.next()) {
                ZigbeeClusterLibrary::ClusterId clusterId = static_cast<ZigbeeClusterLibrary::ClusterId>(outputClustersQuery.value("clusterId").toUInt());
                ZigbeeCluster *cluster = endpoint->createCluster(clusterId, ZigbeeCluster::Client);
                qCDebug(dcZigbeeNetworkDatabase()) << "Loaded" << cluster;
                endpoint->addOutputCluster(cluster);
            }

            node->m_endpoints.append(endpoint);
        }
        nodes.append(node);
    }

    return nodes;
}

bool ZigbeeNetworkDatabase::wipeDatabase()
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Wipe all database entries from" << m_db.databaseName();
    // Note: cascade will clean all other tables
    m_db.exec("DELETE FROM nodes;");
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not delete all node database entries." << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }
    m_db.close();

    // Delete database file
    QFile databaseFile(m_databaseName);
    if (databaseFile.exists()) {
        if (!databaseFile.remove()) {
            qCWarning(dcZigbeeNetworkDatabase()) << "Could not delete database file" << m_databaseName;
            return false;
        }
    }

    return true;
}

bool ZigbeeNetworkDatabase::initDatabase()
{
    // Reopen the db to make sure it can be written
    m_db.close();
    if (!m_db.open()) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not open zigbee database" << m_db.databaseName() << "Initialization failed.";
        return false;
    }


    // FIXME: check schema version fro compatibility or migration

    qCDebug(dcZigbeeNetworkDatabase()) << "Tables" << m_db.tables();
    if (m_db.tables().isEmpty()) {
        // Write pragmas
        m_db.exec("PRAGMA foreign_keys = ON;");
        m_db.exec(QString("PRAGMA user_version = %1;").arg(DB_VERSION));
    }

    // Create nodes table
    if (!m_db.tables().contains("nodes")) {
        createTable("nodes",
                    "(ieeeAddress TEXT PRIMARY KEY, " // ieeeAddress to string
                    "shortAddress INTEGER NOT NULL, " // uint16
                    "nodeDescriptor BLOB NOT NULL, " // bytes as received from the node
                    "powerDescriptor INTEGER NOT NULL, " // uint16
                    "lqi INTEGER NOT NULL," // uint8
                    "timestamp INTEGER NOT NULL)"); // unix timestamp with the last communication
        createIndices("ieeeAddressIndex", "nodes", "ieeeAddress");
    }

    // Create endpoints table
    if (!m_db.tables().contains("endpoints")) {
        createTable("endpoints",
                    "(id INTEGER PRIMARY KEY AUTOINCREMENT, " // for db relation
                    "ieeeAddress INTEGER NOT NULL, " // // reference to nodes.ieeeAddress
                    "endpointId INTEGER NOT NULL, " // uint8
                    "profileId INTEGER NOT NULL, " // uint16
                    "deviceId INTEGER NOT NULL, " // uint16
                    "deviceVersion INTEGER, " // uint8
                    "CONSTRAINT fk_ieeeAddress FOREIGN KEY(ieeeAddress) REFERENCES nodes(ieeeAddress) ON DELETE CASCADE)");
        createIndices("endpointIndex", "endpoints", "ieeeAddress, endpointId");
    }

    // Create server cluster table
    if (!m_db.tables().contains("serverClusters")) {
        createTable("serverClusters",
                    "(id INTEGER PRIMARY KEY AUTOINCREMENT, " // for db relation
                    "endpointId INTEGER NOT NULL, " // reference to endpoint.id
                    "clusterId INTEGER NOT NULL, " // uint16
                    "CONSTRAINT fk_endpoint FOREIGN KEY(endpointId) REFERENCES endpoints(id) ON DELETE CASCADE)");
        createIndices("serverClusterIndex", "serverClusters", "endpointId, clusterId");
    }

    // Create client cluster table
    if (!m_db.tables().contains("clientClusters")) {
        createTable("clientClusters",
                    "(id INTEGER PRIMARY KEY AUTOINCREMENT, " // for db relation
                    "endpointId INTEGER NOT NULL, " // reference to endpoint.id
                    "clusterId INTEGER NOT NULL, " // uint16
                    "CONSTRAINT fk_endpoint FOREIGN KEY(endpointId) REFERENCES endpoints(id) ON DELETE CASCADE)");
        createIndices("clientClusterIndex", "clientClusters", "endpointId, clusterId");
    }

    // Create cluster attributes table
    if (!m_db.tables().contains("attributes")) {
        createTable("attributes",
                    "(id INTEGER PRIMARY KEY AUTOINCREMENT, " // for db relation
                    "clusterId INTEGER NOT NULL, " // reference to serverClusters.id
                    "attributeId INTEGER NOT NULL, " // uint16
                    "dataType INTEGER NOT NULL, " // uint8
                    "data BLOB NOT NULL, " // raw data from attribute
                    "CONSTRAINT fk_cluster FOREIGN KEY(clusterId) REFERENCES serverClusters(id) ON DELETE CASCADE)");
        createIndices("attributesIndex", "attributes", "clusterId, attributeId");
    }

    return true;
}

void ZigbeeNetworkDatabase::createTable(const QString &tableName, const QString &schema)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Creating table" << tableName << schema;
    QString query = QString("CREATE TABLE IF NOT EXISTS %1 %2;").arg(tableName).arg(schema);
    m_db.exec(query);
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not create table in database." << query << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return;
    }
}

void ZigbeeNetworkDatabase::createIndices(const QString &indexName, const QString &tableName, const QString &columns)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Creating table indices" << indexName << tableName << columns;
    m_db.exec(QString("CREATE UNIQUE INDEX IF NOT EXISTS %1 ON %2(%3);").arg(indexName).arg(tableName).arg(columns));
}

bool ZigbeeNetworkDatabase::saveNodeEndpoint(ZigbeeNodeEndpoint *endpoint)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Save" << endpoint;
    QString queryString = QString("INSERT OR REPLACE INTO endpoints (ieeeAddress, endpointId, profileId, deviceId, deviceVersion) "
                                  "VALUES (\"%1\", \"%2\", \"%3\", \"%4\", \"%5\");")
            .arg(endpoint->node()->extendedAddress().toString())
            .arg(endpoint->endpointId())
            .arg(static_cast<quint16>(endpoint->profile()))
            .arg(static_cast<quint16>(endpoint->deviceId()))
            .arg(static_cast<quint8>(endpoint->deviceVersion()));

    qCDebug(dcZigbeeNetworkDatabase()) << queryString;
    m_db.exec(queryString);
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not save endpoint into database." << queryString << m_db.lastError().databaseText() << m_db.lastError().driverText();
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
    qCDebug(dcZigbeeNetworkDatabase()) << "Save" << cluster;
    QString endpointIdReferenceQuery = QString("(SELECT id FROM endpoints WHERE ieeeAddress = \"%1\" AND endpointId = \"%2\")")
            .arg(cluster->node()->extendedAddress().toString())
            .arg(cluster->endpoint()->endpointId());
    QString queryString = QString("INSERT OR REPLACE INTO serverClusters (endpointId, clusterId) VALUES (%1, \"%2\");")
            .arg(endpointIdReferenceQuery)
            .arg(static_cast<quint16>(cluster->clusterId()));
    m_db.exec(queryString);
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not save input cluster into database." << queryString << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    return true;
}

bool ZigbeeNetworkDatabase::saveOutputCluster(ZigbeeCluster *cluster)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Save" << cluster;
    QString endpointIdReferenceQuery = QString("(SELECT id FROM endpoints WHERE ieeeAddress = \"%1\" AND endpointId = \"%2\")")
            .arg(cluster->node()->extendedAddress().toString())
            .arg(cluster->endpoint()->endpointId());
    QString queryString = QString("INSERT OR REPLACE INTO clientClusters (endpointId, clusterId) VALUES (%1, \"%2\");")
            .arg(endpointIdReferenceQuery)
            .arg(static_cast<quint16>(cluster->clusterId()));

    m_db.exec(queryString);
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not save output cluster into database." << queryString << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    return true;
}

bool ZigbeeNetworkDatabase::saveAttribute(ZigbeeCluster *cluster, const ZigbeeClusterAttribute &attribute)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Save" << attribute;
    QString serverClusterIdReferenceQuery = QString("(SELECT id FROM serverClusters "
                                                    "WHERE endpointId = (SELECT id FROM endpoints WHERE ieeeAddress = \"%1\" AND endpointId = \"%2\")"
                                                    "AND clusterId = \"%3\")")
            .arg(cluster->node()->extendedAddress().toString())
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
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not save cluster cluster attribute into database." << queryString << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    return true;
}

bool ZigbeeNetworkDatabase::saveNode(ZigbeeNode *node)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Save" << node;
    QString queryString = QString("INSERT OR REPLACE INTO nodes (ieeeAddress, shortAddress, nodeDescriptor, powerDescriptor, lqi, timestamp) "
                                  "VALUES (\"%1\", \"%2\", \"%3\", \"%4\", \"%5\", \"%6\");")
            .arg(node->extendedAddress().toString())
            .arg(node->shortAddress())
            .arg(node->nodeDescriptor().descriptorRawData.toBase64().data()) // Note: convert to base64 for saving zeros as string
            .arg(node->powerDescriptor().powerDescriptoFlag)
            .arg(node->lqi())
            .arg(node->lastSeen().toMSecsSinceEpoch() / 1000);

    qCDebug(dcZigbeeNetworkDatabase()) << queryString;
    m_db.exec(queryString);
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not save node into database." << queryString << m_db.lastError().databaseText() << m_db.lastError().driverText();
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

bool ZigbeeNetworkDatabase::updateNodeLqi(ZigbeeNode *node, quint8 lqi)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Update nod LQI" << node << lqi;
    QString queryString = QString("UPDATE nodes SET lqi = \"%1\" WHERE ieeeAddress = \"%2\";").arg(lqi).arg(node->extendedAddress().toString());
    m_db.exec(queryString);
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not update node LQI value in the database." << queryString << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    return true;
}

bool ZigbeeNetworkDatabase::updateNodeLastSeen(ZigbeeNode *node, const QDateTime &lastSeen)
{
    quint64 timestamp = lastSeen.toMSecsSinceEpoch() / 1000;
    qCDebug(dcZigbeeNetworkDatabase()) << "Update node last seen UTC timestamp" << node << timestamp;
    QString queryString = QString("UPDATE nodes SET timestamp = \"%1\" WHERE ieeeAddress = \"%2\";").arg(timestamp).arg(node->extendedAddress().toString());
    m_db.exec(queryString);
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not update node timestamp value in the database." << queryString << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    return true;
}

bool ZigbeeNetworkDatabase::removeNode(ZigbeeNode *node)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Remove" << node;
    // Note: cascade delete will clean up all other tables
    QString queryString = QString("DELETE FROM nodes WHERE ieeeAddress = \"%1\";").arg(node->extendedAddress().toString());
    m_db.exec(queryString);
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not remove node from database." << queryString << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    return true;
}


