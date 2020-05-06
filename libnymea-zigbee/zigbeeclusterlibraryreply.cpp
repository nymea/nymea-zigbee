#include "zigbeeclusterlibraryreply.h"

ZigbeeNetworkRequest ZigbeeClusterLibraryReply::request() const
{
    return m_request;
}

ZigbeeClusterLibrary::Frame ZigbeeClusterLibraryReply::requestFrame() const
{
    return m_requestFrame;
}

quint8 ZigbeeClusterLibraryReply::transactionSequenceNumber() const
{
    return m_requestFrame.header.transactionSequenceNumber;
}

ZigbeeClusterLibraryReply::ZigbeeClusterLibraryReply(const ZigbeeNetworkRequest &request, ZigbeeClusterLibrary::Frame requestFrame, QObject *parent) :
    QObject(parent),
    m_request(request),
    m_requestFrame(requestFrame)
{

}
