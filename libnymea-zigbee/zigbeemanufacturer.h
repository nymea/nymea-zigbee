#ifndef ZIGBEEMANUFACTURER_H
#define ZIGBEEMANUFACTURER_H

#include <QString>

class ZigbeeManufacturer
{
public:
    ZigbeeManufacturer();
    ZigbeeManufacturer(quint16 id);

    quint16 id() const;
    QString name() const;

private:
    quint16 m_id = 0;
    QString m_name;

};

#endif // ZIGBEEMANUFACTURER_H
