#ifndef ZIGBEENODENXP_H
#define ZIGBEENODENXP_H

#include <QObject>
#include "../zigbeenode.h"

class ZigbeeNodeNxp : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeNodeNxp(QObject *parent = nullptr);

signals:

};

#endif // ZIGBEENODENXP_H
