#ifndef ZIGBEECLUSTERREPLY_H
#define ZIGBEECLUSTERREPLY_H

#include <QObject>

class ZigbeeClusterReply : public QObject
{
    Q_OBJECT
public:
    explicit ZigbeeClusterReply(QObject *parent = nullptr);

signals:

};

#endif // ZIGBEECLUSTERREPLY_H
