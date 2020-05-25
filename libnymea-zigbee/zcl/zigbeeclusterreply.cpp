#include "zigbeeclusterreply.h"

ZigbeeClusterReply::Error ZigbeeClusterReply::error() const
{
    return m_error;
}

ZigbeeNetworkRequest ZigbeeClusterReply::request() const
{
    return m_request;
}

ZigbeeClusterLibrary::Frame ZigbeeClusterReply::requestFrame() const
{
    return m_requestFrame;
}

quint8 ZigbeeClusterReply::transactionSequenceNumber() const
{
    return m_transactionSequenceNumber;
}

QByteArray ZigbeeClusterReply::responseData() const
{
    return m_responseData;
}

ZigbeeClusterLibrary::Frame ZigbeeClusterReply::responseFrame() const
{
    return m_responseFrame;
}

bool ZigbeeClusterReply::isComplete() const
{
    return m_apsConfirmReceived && m_zclIndicationReceived;
}

ZigbeeClusterReply::ZigbeeClusterReply(const ZigbeeNetworkRequest &request, ZigbeeClusterLibrary::Frame requestFrame, QObject *parent) :
    QObject(parent),
    m_request(request),
    m_requestFrame(requestFrame)
{

}
