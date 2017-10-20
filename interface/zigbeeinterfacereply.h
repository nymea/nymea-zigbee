#ifndef ZIGBEEINTERFACEREPLY_H
#define ZIGBEEINTERFACEREPLY_H

#include <QObject>
#include <QTimer>

#include "zigbeeinterfacerequest.h"

class ZigbeeBridgeController;

class ZigbeeInterfaceReply : public QObject
{
    Q_OBJECT
    friend class ZigbeeBridgeController;

public:
    enum Status {
        Success = 0,
        InvalidParameter = 1,
        UnhandledCommand = 2,
        CommandFailed = 3,
        Busy = 4,
        StackAlreadyStarted = 5,
        ZigbeeErrorEvent = 6,
        Timeouted = 7
    };
    Q_ENUM(Status)

    explicit ZigbeeInterfaceReply(const ZigbeeInterfaceRequest &request, QObject *parent = nullptr);

    ZigbeeInterfaceRequest request() const;
    ZigbeeInterfaceMessage statusMessage() const;
    ZigbeeInterfaceMessage additionalMessage() const;

    bool timeouted() const;

    Status status() const;
    quint8 statusCode() const;
    quint8 sequenceNumber() const;
    QString statusErrorMessage() const;

private:
    QTimer *m_timer = nullptr;
    bool m_timeouted = false;

    ZigbeeInterfaceRequest m_request;
    ZigbeeInterfaceMessage m_statusMessage;
    ZigbeeInterfaceMessage m_additionalMessage;

    // Status content
    Status m_status;
    quint8 m_statusCode;
    quint8 m_sequenceNumber;
    QString m_statusErrorMessage;

    // Called by ZigbeeBridgeController
    void setStatusMessage(const ZigbeeInterfaceMessage &statusMessage);
    void setAdditionalMessage(const ZigbeeInterfaceMessage &additionalMessage);

    bool isComplete() const;
    void startTimer(const int &timeout = 500);
    void setFinished();

signals:
    void finished();
    void timeout();

private slots:
    void onTimeout();

};

#endif // ZIGBEEINTERFACEREPLY_H
