#ifndef ZIGBEECLUSTERLIBRARYREPLY_H
#define ZIGBEECLUSTERLIBRARYREPLY_H

#include <QObject>

#include "zigbeenetworkrequest.h"
#include "zigbeeclusterlibrary.h"

class ZigbeeClusterLibraryReply : public QObject
{
    Q_OBJECT

    friend class ZigbeeCluster;

public:
    ZigbeeNetworkRequest request() const;
    ZigbeeClusterLibrary::Frame requestFrame() const;

    quint8 transactionSequenceNumber() const;

private:
    explicit ZigbeeClusterLibraryReply(const ZigbeeNetworkRequest &request, ZigbeeClusterLibrary::Frame requestFrame, QObject *parent = nullptr);

    ZigbeeNetworkRequest m_request;
    ZigbeeClusterLibrary::Frame m_requestFrame;
    ZigbeeClusterLibrary::Command m_expectedResponse;

    ZigbeeClusterLibrary::Frame m_responseFrame;

signals:
    void finished();

};

#endif // ZIGBEECLUSTERLIBRARYREPLY_H
