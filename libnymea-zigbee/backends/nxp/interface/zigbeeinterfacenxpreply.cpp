#include "zigbeeinterfacenxpreply.h"

ZigbeeNetworkRequest ZigbeeInterfaceNxpReply::networkRequest() const
{
    return m_networkRequest;
}

QString ZigbeeInterfaceNxpReply::requestName()
{
    return m_requestName;
}

Nxp::Command ZigbeeInterfaceNxpReply::command() const
{
    return m_command;
}

quint8 ZigbeeInterfaceNxpReply::sequenceNumber() const
{
    return m_sequenceNumber;
}

QByteArray ZigbeeInterfaceNxpReply::requestData() const
{
    return m_requestData;
}

QByteArray ZigbeeInterfaceNxpReply::responseData() const
{
    return m_responseData;
}

Nxp::Status ZigbeeInterfaceNxpReply::status() const
{
    return m_status;
}

bool ZigbeeInterfaceNxpReply::timendOut() const
{
    return m_timeout;
}

bool ZigbeeInterfaceNxpReply::aborted() const
{
    return m_aborted;
}

void ZigbeeInterfaceNxpReply::abort()
{
    m_timer->stop();
    m_aborted = true;
    emit finished();
}

ZigbeeInterfaceNxpReply::ZigbeeInterfaceNxpReply(Nxp::Command command, QObject *parent) :
    QObject(parent),
    m_timer(new QTimer(this)),
    m_command(command)
{
    m_timer->setInterval(5000);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &ZigbeeInterfaceNxpReply::onTimeout);
}

void ZigbeeInterfaceNxpReply::onTimeout()
{
    m_timeout = true;
    emit timeout();
    emit finished();
}

QDebug operator<<(QDebug debug, ZigbeeInterfaceNxpReply *reply)
{
    debug.nospace() << "InterfaceReply(" << reply->requestName() << ", " << reply->sequenceNumber() << ")";
    return debug.space();
}
