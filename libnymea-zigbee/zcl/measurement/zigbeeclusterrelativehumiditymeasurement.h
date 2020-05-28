#ifndef ZIGBEECLUSTERRELATIVEHUMIDITYMEASUREMENT_H
#define ZIGBEECLUSTERRELATIVEHUMIDITYMEASUREMENT_H

#include <QObject>

class ZigbeeClusterRelativeHumidityMeasurement : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeClusterRelativeHumidityMeasurement(QObject *parent = nullptr);

signals:

};

#endif // ZIGBEECLUSTERRELATIVEHUMIDITYMEASUREMENT_H
