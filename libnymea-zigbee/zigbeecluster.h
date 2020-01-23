/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
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
#include "zigbeeclusterattribute.h"

class ZigbeeCluster : public QObject
{
    Q_OBJECT

    friend class ZigbeeNode;

public:
    explicit ZigbeeCluster(Zigbee::ClusterId clusterId, QObject *parent = nullptr);

    Zigbee::ClusterId clusterId() const;
    QString clusterName() const;

    QList<ZigbeeClusterAttribute> attributes() const;
    bool hasAttribute(quint16 attributeId) const;

    ZigbeeClusterAttribute attribute(quint16 id);

private:
    Zigbee::ClusterId m_clusterId = Zigbee::ClusterIdUnknown;
    QHash<quint16, ZigbeeClusterAttribute> m_attributes;

protected:
    void setAttribute(const ZigbeeClusterAttribute &attribute);

signals:
    void attributeChanged(const ZigbeeClusterAttribute &attribute);

public slots:

};

QDebug operator<<(QDebug debug, ZigbeeCluster *cluster);

#endif // ZIGBEECLUSTER_H
