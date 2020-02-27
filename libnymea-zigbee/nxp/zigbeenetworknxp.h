#ifndef ZIGBEENETWORKNXP_H
#define ZIGBEENETWORKNXP_H

#include <QObject>
#include "../zigbeenetwork.h"

class ZigbeeNetworkNxp : public ZigbeeNetwork
{
    Q_OBJECT
public:
    explicit ZigbeeNetworkNxp(QObject *parent = nullptr);

signals:

public slots:
    void startNetwork() override;
    void stopNetwork() override;

    void factoryResetNetwork() override;

};

#endif // ZIGBEENETWORKNXP_H
