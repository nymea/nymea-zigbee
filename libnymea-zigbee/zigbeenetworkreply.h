#ifndef ZIGBEENETWORKREPLY_H
#define ZIGBEENETWORKREPLY_H

#include <QObject>

class ZigbeeNetworkReply : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeNetworkReply(QObject *parent = nullptr);

signals:

};

#endif // ZIGBEENETWORKREPLY_H
