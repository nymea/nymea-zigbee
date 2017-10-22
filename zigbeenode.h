#ifndef ZIGBEENODE_H
#define ZIGBEENODE_H

#include <QObject>

#include "zigbee.h"

class ZigbeeNode : public QObject
{
    Q_OBJECT
public:
    enum NodeType {
        NodeTypeCoordinator = 0,
        NodeTypeRouter = 1,
        NodeTypeEndDevice = 2
    };
    Q_ENUM(NodeType)

    enum FrequencyBand {
        FrequencyBand868Mhz,
        FrequencyBand902Mhz,
        FrequencyBand2400Mhz
    };
    Q_ENUM(FrequencyBand)

    explicit ZigbeeNode(QObject *parent = nullptr);

    quint16 shortAddress() const;
    quint64 extendedAddress() const;

    // Information from node descriptor
    NodeType nodeType() const;
    FrequencyBand frequencyBand() const;

    bool canBeCoordinator() const;

private:
    quint16 m_shortAddress = 0;
    quint64 m_extendedAddress = 0;
    NodeType m_nodeType;

protected:
    void setShortAddress(const quint16 &shortAddress);
    void setExtendedAddress(const quint64 &extendedAddress);


signals:

public slots:

};

#endif // ZIGBEENODE_H
