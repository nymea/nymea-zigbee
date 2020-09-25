#ifndef ZIGBEEINTERFACENXPREPLY_H
#define ZIGBEEINTERFACENXPREPLY_H

#include <QObject>
#include <QTimer>

#include "nxp.h"
#include "zigbeenetworkrequest.h"

class ZigbeeInterfaceNxpReply : public QObject
{
    Q_OBJECT

    friend class ZigbeeBridgeControllerNxp;

public:
    // Request content
    ZigbeeNetworkRequest networkRequest() const;
    QString requestName();
    Nxp::Command command() const;
    quint8 sequenceNumber() const;
    QByteArray requestData() const;
    QByteArray responseData() const;

    // Response content
    Nxp::Status status() const;

    bool timendOut() const;
    bool aborted() const;
    void abort();

private:
    explicit ZigbeeInterfaceNxpReply(Nxp::Command command, QObject *parent = nullptr);

    ZigbeeNetworkRequest m_networkRequest;
    QTimer *m_timer = nullptr;
    bool m_timeout = false;
    bool m_aborted = false;

    // Request content
    QString m_requestName;
    Nxp::Command m_command;
    quint8 m_sequenceNumber = 0;
    QByteArray m_requestData;

    // Response content
    Nxp::Status m_status = Nxp::StatusUnknownCommand; // FIXME
    QByteArray m_responseData;

private slots:
    void onTimeout();

signals:
    void timeout();
    void finished();

};

QDebug operator<<(QDebug debug, ZigbeeInterfaceNxpReply *reply);


#endif // ZIGBEEINTERFACENXPREPLY_H
