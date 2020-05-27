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
    Zigbee::ClusterId clusterId;
    quint16 attributeId;
    Zigbee::ZigbeeStatus attributeStatus;
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

public:
    enum Direction {
        Input,
        Output
    };
    Q_ENUM(Direction)


//    // Power configuration cluster 0x0001

//    enum PowerConfigurationAttribute {
//        PowerConfigurationAttributeMainsInformation = 0x0000,
//        PowerConfigurationAttributeMainsSettings = 0x0001,
//        PowerConfigurationAttributeBatteryInformation = 0x0002,
//        PowerConfigurationAttributeBatterySettings = 0x0003,
//        PowerConfigurationAttributeBatterySource2Information = 0x0004,
//        PowerConfigurationAttributeBattterySource2Settings = 0x0005,
//        PowerConfigurationAttributeBatterySource3Information = 0x0006,
//        PowerConfigurationAttributeBattterySource3Settings = 0x0007
//    };
//    Q_ENUM(PowerConfigurationAttribute)


//    // Level cluster 0x0008

//    enum LevelClusterAttribute {
//        LevelClusterAttributeCurrentLevel = 0x0000,
//        LevelClusterAttributeRemainingTime = 0x0001,
//        LevelClusterAttributeOnOffTransitionTime = 0x0010,
//        LevelClusterAttributeOnLevel = 0x0011,
//        LevelClusterAttributeOnTransitionTime = 0x0012,
//        LevelClusterAttributeOffTransitionTime = 0x0013,
//        LevelClusterAttributeDefaultMoveRate = 0x0014
//    };
//    Q_ENUM(LevelClusterAttribute)

//    enum LevelClusterCommand {
//        LevelClusterCommandMoveToLevel = 0x00,
//        LevelClusterCommandMove = 0x01,
//        LevelClusterCommandStep = 0x02,
//        LevelClusterCommandStop = 0x03,
//        LevelClusterCommandMoveToLevelWithOnOff = 0x04,
//        LevelClusterCommandMoveWithOnOff = 0x05,
//        LevelClusterCommandStepWithOnOff = 0x06,
//        LevelClusterCommandStopWithOnOff = 0x07,
//    };
//    Q_ENUM(LevelClusterCommand)

//    // Color cluster 0x0300

//    enum ColorControlClusterAttribute {
//        ColorControlClusterAttributeCurrentHue = 0x0000,
//        ColorControlClusterAttributeCurrentSaturation = 0x0001,
//        ColorControlClusterAttributeRemainingTime = 0x0002,
//        ColorControlClusterAttributeCurrentX = 0x0003,
//        ColorControlClusterAttributeCurrentY = 0x0004,
//        ColorControlClusterAttributeDriftCompensation = 0x0005,
//        ColorControlClusterAttributeCompensationText = 0x0006,
//        ColorControlClusterAttributeColorTemperatureMireds = 0x0007,
//        ColorControlClusterAttributeColorMode = 0x0008,
//        ColorControlClusterAttributeEnhancedCurrentHue = 0x4000,
//        ColorControlClusterAttributeEnhancedColorMode = 0x4001,
//        ColorControlClusterAttributeColorLoopActive = 0x4002,
//        ColorControlClusterAttributeColorLoopDirection = 0x4003,
//        ColorControlClusterAttributeColorLoopTime = 0x4004,
//        ColorControlClusterAttributeColorLoopStartEnhancedHue = 0x4005,
//        ColorControlClusterAttributeColorLoopStoredEnhancedHue = 0x4006,
//        ColorControlClusterAttributeColorCapabilities = 0x400a,
//        ColorControlClusterAttributeColorTempPhysicalMinMireds = 0x400b,
//        ColorControlClusterAttributeColorTempPhysicalMaxMireds = 0x400c
//    };
//    Q_ENUM(ColorControlClusterAttribute)

//    enum ColorControlClusterColorMode {
//        ColorControlClusterColorModeHueSaturation = 0x00,
//        ColorControlClusterColorModeXY = 0x01,
//        ColorControlClusterColorModeColorTemperatureMired = 0x02
//    };
//    Q_ENUM(ColorControlClusterColorMode)

    explicit ZigbeeCluster(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Zigbee::ClusterId clusterId, Direction direction, QObject *parent = nullptr);

    Direction direction() const;

    Zigbee::ClusterId clusterId() const;
    QString clusterName() const;

    QList<ZigbeeClusterAttribute> attributes() const;
    bool hasAttribute(quint16 attributeId) const;
    ZigbeeClusterAttribute attribute(quint16 attributeId);

    // ZCL global commands
    ZigbeeClusterReply *readAttributes(QList<quint16> attributes);

protected:
    ZigbeeNetwork *m_network = nullptr;
    ZigbeeNode *m_node = nullptr;
    ZigbeeNodeEndpoint *m_endpoint= nullptr;

    Zigbee::ClusterId m_clusterId = Zigbee::ClusterIdUnknown;
    Direction m_direction = Input;
    QHash<quint16, ZigbeeClusterAttribute> m_attributes;

    ZigbeeNetworkRequest createGeneralRequest();
    quint8 m_transactionSequenceNumber = 0;
    QHash<quint8, ZigbeeClusterReply *> m_pendingReplies;
    ZigbeeClusterReply *createClusterReply(const ZigbeeNetworkRequest &request, ZigbeeClusterLibrary::Frame frame);

    bool verifyNetworkError(ZigbeeClusterReply *zclReply, ZigbeeNetworkReply *networkReply);
    void finishZclReply(ZigbeeClusterReply *zclReply);

    virtual void processDataIndication(ZigbeeClusterLibrary::Frame frame);

private:
    virtual void setAttribute(const ZigbeeClusterAttribute &attribute);

signals:
    void attributeChanged(const ZigbeeClusterAttribute &attribute);

public slots:
    void processApsDataIndication(QByteArray payload);

};

QDebug operator<<(QDebug debug, ZigbeeCluster *cluster);
QDebug operator<<(QDebug debug, const ZigbeeClusterAttributeReport &attributeReport);

#endif // ZIGBEECLUSTER_H
