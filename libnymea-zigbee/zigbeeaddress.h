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

#ifndef ZIGBEEADDRESS_H
#define ZIGBEEADDRESS_H

#include <QString>
#include <QByteArray>

class ZigbeeAddress
{
public:
    ZigbeeAddress();
    explicit ZigbeeAddress(quint64 address);
    explicit ZigbeeAddress(const QString &address);
    ZigbeeAddress(const ZigbeeAddress &other);

    quint64 toUInt64() const;
    QString toString() const;

    bool isNull() const;
    void clear();

    ZigbeeAddress &operator=(const ZigbeeAddress &other);
    bool operator<(const ZigbeeAddress &other) const;
    bool operator==(const ZigbeeAddress &other) const;
    bool operator!=(const ZigbeeAddress &other) const;

private:
     quint64 m_address = 0;

};

QDebug operator<<(QDebug debug, const ZigbeeAddress &address);

#endif // ZIGBEEADDRESS_H
