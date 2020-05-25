#ifndef ZIGBEECLUSTERREPLY_H
#define ZIGBEECLUSTERREPLY_H

#include <QObject>

#include "zigbeenetworkrequest.h"
#include "zigbeeclusterlibrary.h"

class ZigbeeClusterReply : public QObject
{
    Q_OBJECT

    friend class ZigbeeCluster;

public:
    enum Error {
        ErrorNoError, // All OK, no error occured, the message was transported successfully
        ErrorTimeout, // The request timeouted
        ErrorZigbeeApsStatusError, // An APS transport error occured. See zigbeeApsStatus()
        ErrorInterfaceError, // A transport interface error occured. Could not communicate with the hardware.
        ErrorNetworkOffline // The network is offline. Cannot send any requests
    };
    Q_ENUM(Error)

    Error error() const;

    ZigbeeNetworkRequest request() const;
    ZigbeeClusterLibrary::Frame requestFrame() const;

    quint8 transactionSequenceNumber() const;

    QByteArray responseData() const;
    ZigbeeClusterLibrary::Frame responseFrame() const;

    bool isComplete() const;

private:
    explicit ZigbeeClusterReply(const ZigbeeNetworkRequest &request, ZigbeeClusterLibrary::Frame requestFrame, QObject *parent = nullptr);

    Error m_error = ErrorNoError;

    // Request
    quint8 m_transactionSequenceNumber = 0;
    ZigbeeNetworkRequest m_request;
    ZigbeeClusterLibrary::Frame m_requestFrame;

    // Response
    bool m_apsConfirmReceived = false;
    Zigbee::ZigbeeApsStatus m_zigbeeApsStatus = Zigbee::ZigbeeApsStatusSuccess;

    ZigbeeClusterLibrary::Command m_expectedResponse;

    bool m_zclIndicationReceived = false;
    QByteArray m_responseData;
    ZigbeeClusterLibrary::Frame m_responseFrame;

signals:
    void finished();

};

#endif // ZIGBEECLUSTERREPLY_H
