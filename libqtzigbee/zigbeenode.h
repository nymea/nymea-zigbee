#ifndef ZIGBEENODE_H
#define ZIGBEENODE_H

#include <QObject>

#include "zigbee.h"
#include "zigbeeaddress.h"
#include "zigbeebridgecontroller.h"

class ZigbeeNetworkManager;

class ZigbeeNode : public QObject
{
    Q_OBJECT

    friend class ZigbeeNetworkManager;

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

    quint16 shortAddress() const;
    ZigbeeAddress extendedAddress() const;

    // Information from node descriptor
    NodeType nodeType() const;
    FrequencyBand frequencyBand() const;

    bool canBeCoordinator() const;

    void init();

private:
    ZigbeeBridgeController *m_controller;

    quint16 m_shortAddress = 0;
    ZigbeeAddress m_extendedAddress;
    NodeType m_nodeType;

    void requestNodeDescription();
    void requestSimpleNodeDescription();
    void requestPowerDescriptor();


protected:
    ZigbeeNode(ZigbeeBridgeController *controller, QObject *parent = nullptr);

    ZigbeeBridgeController *controller();

    void setShortAddress(const quint16 &shortAddress);
    void setExtendedAddress(const ZigbeeAddress &extendedAddress);

signals:

private slots:
    void onRequestNodeDescriptionFinished();
    void onRequestSimpleNodeDescriptionFinished();
    void onRequestPowerDescriptorFinished();


public slots:

};

QDebug operator<<(QDebug debug, ZigbeeNode *node);


#endif // ZIGBEENODE_H
