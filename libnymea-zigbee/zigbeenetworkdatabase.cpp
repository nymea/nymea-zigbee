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
    m_connectionName = QFileInfo(m_databaseName).baseName();
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    m_db.setDatabaseName(m_databaseName);

    if (!m_db.isValid()) {
        qCWarning(dcZigbeeNetworkDatabase()) << "The zigbee network database is not valid" << m_db.databaseName();
        // FIXME: rotate database
        return;
    }

    qCDebug(dcZigbeeNetworkDatabase()) << "Opening zigbee network database" << m_db.databaseName();
    if (!initDatabase()) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Failed to initialize the database" << m_db.databaseName();
        // FIXME: rotate database
        return;
    }
}

ZigbeeNetworkDatabase::~ZigbeeNetworkDatabase()
{
    m_db.close();
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(m_connectionName);
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
    QSqlQuery nodesQuery(query, m_db);
    if (!nodesQuery.exec()) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Unable to execute SQL query" << query << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return QList<ZigbeeNode *>();
    }

    while (nodesQuery.next()) {
        QString ieeeAddress = nodesQuery.value("ieeeAddress").toString();
        quint16 shortAddress = nodesQuery.value("shortAddress").toUInt();
        QByteArray nodeDescriptor = QByteArray::fromBase64(nodesQuery.value("nodeDescriptor").toByteArray());
        quint16 powerDescriptor = nodesQuery.value("powerDescriptor").toUInt();
        quint8 lqi = nodesQuery.value("lqi").toUInt();
        quint64 lastSeen = nodesQuery.value("timestamp").toULongLong();

        // Build the node object
        ZigbeeNode *node = new ZigbeeNode(m_network, shortAddress, ZigbeeAddress(ieeeAddress), m_network);
        if (!nodeDescriptor.isEmpty()) {
            node->m_nodeDescriptor = ZigbeeDeviceProfile::parseNodeDescriptor(nodeDescriptor);
            node->m_nodeDescriptorAvailable = true;
        }
        node->m_macCapabilities = node->nodeDescriptor().macCapabilities;
        if (powerDescriptor != 0x0000) {
            node->m_powerDescriptor = ZigbeeDeviceProfile::parsePowerDescriptor(powerDescriptor);
            node->m_powerDescriptorAvailable = true;
        }
        node->m_lqi = lqi;
        node->m_lastSeen = QDateTime::fromMSecsSinceEpoch(lastSeen * 1000);

        qCDebug(dcZigbeeNetworkDatabase()) << "Loaded" << node;

        // Now load all endpoints for this node
        query = QString("SELECT * FROM endpoints WHERE ieeeAddress = \"%1\";").arg(ieeeAddress);

        QSqlQuery endpointsQuery(query, m_db);
        if (!endpointsQuery.exec()) {
            qCWarning(dcZigbeeNetworkDatabase()) << "Unable to execute SQL query" << query << m_db.lastError().databaseText() << m_db.lastError().driverText();
            return QList<ZigbeeNode *>();
        }

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

            QSqlQuery inputClustersQuery(query, m_db);
            if (!inputClustersQuery.exec()) {
                qCWarning(dcZigbeeNetworkDatabase()) << "Unable to execute SQL query" << query << m_db.lastError().databaseText() << m_db.lastError().driverText();
                return QList<ZigbeeNode *>();
            }

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

                QSqlQuery attributesQuery(query, m_db);
                if (!attributesQuery.exec()) {
                    qCWarning(dcZigbeeNetworkDatabase()) << "Unable to execute SQL query" << query << m_db.lastError().databaseText() << m_db.lastError().driverText();
                    return QList<ZigbeeNode *>();
                }

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

            // Set the basic cluster attributes if present to endpoint and node
            if (endpoint->hasInputCluster(ZigbeeClusterLibrary::ClusterIdBasic)) {
                ZigbeeClusterBasic *basicCluster = endpoint->inputCluster<ZigbeeClusterBasic>(ZigbeeClusterLibrary::ClusterIdBasic);

                if (basicCluster->hasAttribute(ZigbeeClusterBasic::AttributeManufacturerName)) {
                    endpoint->setManufacturerName(basicCluster->attribute(ZigbeeClusterBasic::AttributeManufacturerName).dataType().toString());
                    node->m_manufacturerName = endpoint->manufacturerName();
                }

                if (basicCluster->hasAttribute(ZigbeeClusterBasic::AttributeModelIdentifier)) {
                    endpoint->setModelIdentifier(basicCluster->attribute(ZigbeeClusterBasic::AttributeModelIdentifier).dataType().toString());
                    node->m_modelName = endpoint->modelIdentifier();
                }

                if (basicCluster->hasAttribute(ZigbeeClusterBasic::AttributeSwBuildId)) {
                    endpoint->setSoftwareBuildId(basicCluster->attribute(ZigbeeClusterBasic::AttributeSwBuildId).dataType().toString());
                    node->m_version = endpoint->softwareBuildId();
                }
            }

            // Load output clusters for this endpoint
            query = QString("SELECT * FROM clientClusters WHERE endpointId = (SELECT id FROM endpoints WHERE ieeeAddress = \"%1\" AND endpointId = \"%2\");")
                    .arg(ieeeAddress)
                    .arg(endpointId);

            QSqlQuery outputClustersQuery(query, m_db);
            if (!outputClustersQuery.exec()) {
                qCWarning(dcZigbeeNetworkDatabase()) << "Unable to execute SQL query" << query << m_db.lastError().databaseText() << m_db.lastError().driverText();
                return QList<ZigbeeNode *>();
            }

            while (outputClustersQuery.next()) {
                ZigbeeClusterLibrary::ClusterId clusterId = static_cast<ZigbeeClusterLibrary::ClusterId>(outputClustersQuery.value("clusterId").toUInt());
                ZigbeeCluster *cluster = endpoint->createCluster(clusterId, ZigbeeCluster::Client);
                qCDebug(dcZigbeeNetworkDatabase()) << "Loaded" << cluster;
                endpoint->addOutputCluster(cluster);
            }

            node->m_endpoints.append(endpoint);
            node->setupEndpointInternal(endpoint);
        }

        QSqlQuery bindingsQuery(m_db);
        bindingsQuery.prepare("SELECT * FROM bindings WHERE sourceAddress = ?;");
        bindingsQuery.addBindValue(node->extendedAddress().toString());
        bindingsQuery.exec();
        while (bindingsQuery.next()) {
            ZigbeeDeviceProfile::BindingTableListRecord record;
            record.sourceAddress = ZigbeeAddress(bindingsQuery.value("sourceAddress").toString());
            record.sourceEndpoint = bindingsQuery.value("sourceEndpointId").toUInt();
            record.clusterId = bindingsQuery.value("clusterId").toUInt();
            record.destinationAddressMode = static_cast<Zigbee::DestinationAddressMode>(bindingsQuery.value("destinationAddressMode").toUInt());
            record.destinationShortAddress = bindingsQuery.value("destinationShortAddress").toUInt();
            record.destinationIeeeAddress = ZigbeeAddress(bindingsQuery.value("destinationIeeeAddress").toString());
            record.destinationEndpoint = bindingsQuery.value("destinationEndpointId").toUInt();
            node->m_bindingTableRecords.append(record);
        }

        nodes.append(node);
    }

    return nodes;
}

bool ZigbeeNetworkDatabase::wipeDatabase()
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Wipe all database entries from" << m_db.databaseName();
    // Note: cascade will clean all other tables
    QSqlQuery deleteQuery("DELETE FROM nodes;", m_db);
    if (!deleteQuery.exec()) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Unable to execute SQL query" << deleteQuery.lastQuery() << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not delete all node database entries." << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    m_db.close();
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(m_connectionName);

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

    // TODO: check schema version fro compatibility or migration

    qCDebug(dcZigbeeNetworkDatabase()) << "Tables" << m_db.tables();
    if (m_db.tables().isEmpty()) {
        // Write pragmas
        QSqlQuery enableForeigenKeysQuery("PRAGMA foreign_keys = ON;", m_db);
        if (!enableForeigenKeysQuery.exec()) {
            qCWarning(dcZigbeeNetworkDatabase()) << "Unable to execute SQL query" << enableForeigenKeysQuery.lastQuery() << m_db.lastError().databaseText() << m_db.lastError().driverText();
            return false;
        }

        QSqlQuery setUserVersionQuery(QString("PRAGMA user_version = %1;").arg(DB_VERSION), m_db);
        if (!setUserVersionQuery.exec()) {
            qCWarning(dcZigbeeNetworkDatabase()) << "Unable to execute SQL query" << enableForeigenKeysQuery.lastQuery() << m_db.lastError().databaseText() << m_db.lastError().driverText();
            return false;
        }
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

    if (!m_db.tables().contains("bindings")) {
        createTable("bindings", "(sourceAddress TEXT NOT NULL, "
                                "sourceEndpointId INTEGER NOT NULL, "
                                "clusterId INTEGER NOT NULL, "
                                "destinationAddressMode INTEGER NOT NULL, "
                                "destinationShortAddress INTEGER, "
                                "destinationIeeeAddress TEXT, "
                                "destinationEndpointId INTEGER, "
                                "CONSTRAINT fk FOREIGN KEY(sourceAddress) REFERENCES nodes(ieeeAddress) ON DELETE CASCADE"
                                ")");
    }

    return true;
}

void ZigbeeNetworkDatabase::createTable(const QString &tableName, const QString &schema)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Creating table" << tableName << schema;
    QString query = QString("CREATE TABLE IF NOT EXISTS %1 %2;").arg(tableName).arg(schema);
    QSqlQuery createTableQuery(query, m_db);
    createTableQuery.exec();

    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not create table in database." << query << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return;
    }
}

void ZigbeeNetworkDatabase::createIndices(const QString &indexName, const QString &tableName, const QString &columns)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Creating table indices" << indexName << tableName << columns;
    QString query = QString("CREATE UNIQUE INDEX IF NOT EXISTS %1 ON %2(%3);").arg(indexName).arg(tableName).arg(columns);
    QSqlQuery createTableQuery(query, m_db);
    createTableQuery.exec();
    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not create indices in database." << query << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return;
    }
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
    QSqlQuery saveNodeEndpointQuery(queryString, m_db);
    saveNodeEndpointQuery.exec();
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

    QSqlQuery saveInputClusterQuery(queryString, m_db);
    saveInputClusterQuery.exec();
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

    QSqlQuery saveOutputClusterQuery(queryString, m_db);
    saveOutputClusterQuery.exec();
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

    QSqlQuery saveClusterAttributeQuery(queryString, m_db);
    saveClusterAttributeQuery.exec();

    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not save cluster attribute into database." << queryString << m_db.lastError().databaseText() << m_db.lastError().driverText();
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

    QSqlQuery saveNodeQuery(queryString, m_db);
    saveNodeQuery.exec();

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

    updateNodeBindingTable(node);

    return true;
}

bool ZigbeeNetworkDatabase::updateNodeLqi(ZigbeeNode *node, quint8 lqi)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Update node LQI" << node << lqi;
    QString queryString = QString("UPDATE nodes SET lqi = \"%1\" WHERE ieeeAddress = \"%2\";").arg(lqi).arg(node->extendedAddress().toString());
    QSqlQuery updateNodeLqiQuery(queryString, m_db);
    updateNodeLqiQuery.exec();

    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not update node LQI value in the database." << queryString << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    return true;
}

bool ZigbeeNetworkDatabase::updateNodeNetworkAddress(ZigbeeNode *node, quint16 networkAddress)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Update node network address" << node << ZigbeeUtils::convertUint16ToHexString(networkAddress);
    QString queryString = QString("UPDATE nodes SET shortAddress = \"%1\" WHERE ieeeAddress = \"%2\";").arg(networkAddress).arg(node->extendedAddress().toString());
    QSqlQuery updateNodeNetworkAddressQuery(queryString, m_db);
    updateNodeNetworkAddressQuery.exec();

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
    QSqlQuery updateNodeLastSeenQuery(queryString, m_db);
    updateNodeLastSeenQuery.exec();

    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not update node timestamp value in the database." << queryString << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    return true;
}

bool ZigbeeNetworkDatabase::updateNodeBindingTable(ZigbeeNode *node)
{
    bool error = false;
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM bindings WHERE sourceAddress = ?");
    query.addBindValue(node->extendedAddress().toString());
    query.exec();
    if (query.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Error clearing old binding table:" << query.executedQuery() << query.lastError().databaseText() << query.lastError().driverText();
        error = true;
    }

    foreach (const ZigbeeDeviceProfile::BindingTableListRecord &record, node->bindingTableRecords()) {
        QSqlQuery insertQuery(m_db);
        insertQuery.prepare("INSERT INTO bindings (sourceAddress, sourceEndpointId, clusterId, destinationAddressMode, destinationShortAddress, destinationIeeeAddress, destinationEndpointId) VALUES(?, ?, ?, ?, ?, ?, ?)");
        insertQuery.addBindValue(record.sourceAddress.toString());
        insertQuery.addBindValue(record.sourceEndpoint);
        insertQuery.addBindValue(record.clusterId);
        insertQuery.addBindValue(record.destinationAddressMode);
        insertQuery.addBindValue(record.destinationShortAddress);
        insertQuery.addBindValue(record.destinationIeeeAddress.toString());
        insertQuery.addBindValue(record.destinationEndpoint);
        insertQuery.exec();
        if (insertQuery.lastError().type() != QSqlError::NoError) {
            qCWarning(dcZigbeeNetworkDatabase()) << "Error inserting into binding table:" << query.executedQuery() << query.lastError().databaseText() << query.lastError().driverText();
            error = true;
        }
    }
    return error;
}

bool ZigbeeNetworkDatabase::removeNode(ZigbeeNode *node)
{
    qCDebug(dcZigbeeNetworkDatabase()) << "Remove" << node;
    // Note: cascade delete will clean up all other tables
    QString queryString = QString("DELETE FROM nodes WHERE ieeeAddress = \"%1\";").arg(node->extendedAddress().toString());
    QSqlQuery removeNodeQuery(queryString, m_db);
    removeNodeQuery.exec();

    if (m_db.lastError().type() != QSqlError::NoError) {
        qCWarning(dcZigbeeNetworkDatabase()) << "Could not remove node from database." << queryString << m_db.lastError().databaseText() << m_db.lastError().driverText();
        return false;
    }

    return true;
}


