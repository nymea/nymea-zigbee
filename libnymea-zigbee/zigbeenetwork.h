#ifndef ZIGBEENETWORK_H
#define ZIGBEENETWORK_H

#include <QObject>

#include "zigbeenode.h"
#include "zigbeesecurityconfiguration.h"

class ZigbeeNetwork : public ZigbeeNode
{
    Q_OBJECT

public:
    enum ControllerType {
        ControlerTypeNxp
    };
    Q_ENUM(ControllerType)

    enum State {
        StateDisconnected,
        StateStarting,
        StateRunning,
        StateStopping
    };
    Q_ENUM(State)

    enum Error {
        ErrorNoError,
        ErrorHardwareUnavailable
    };
    Q_ENUM(Error)

    explicit ZigbeeNetwork(ControllerType controllerType, QObject *parent = nullptr);

    State state() const;
    ControllerType controlerType() const;

    Error error() const;

    // Serial port configuration
    QString serialPortName() const;
    void setSerialPortName(const QString &serialPortName);

    qint32 serialBaudrate() const;
    void setSerialBaudrate(qint32 baudrate);

    // Network configurations
    quint64 extendedPanId() const;
    void setExtendedPanId(quint64 extendedPanId);

    uint channel() const;
    void setChannel(uint channel);

    ZigbeeSecurityConfiguration securityConfiguration() const;
    void setSecurityConfiguration(const ZigbeeSecurityConfiguration &securityConfiguration);

    QList<ZigbeeNode *> nodes() const;

    ZigbeeNode *getZigbeeNode(quint16 shortAddress);
    ZigbeeNode *getZigbeeNode(ZigbeeAddress address);

private:
    ControllerType m_controllerType = ControlerTypeNxp;
    State m_state = StateDisconnected;
    Error m_error = ErrorNoError;

    // Serial port configuration
    QString m_serialPortName = "/dev/ttyUSB0";
    qint32 m_serialBaudrate = 115200;

    // Network configurations
    quint64 m_extendedPanId = 0;
    uint m_channel = 0;
    ZigbeeSecurityConfiguration m_securityConfiguration;
    ZigbeeNode::NodeType m_nodeType = ZigbeeNode::NodeTypeCoordinator;

    QList<ZigbeeNode *> m_nodes;

protected:
    void addNode(ZigbeeNode *node);
    void removeNode(ZigbeeNode *node);

    void setState(State state);
    void setError(Error error);

signals:
    void nodeAdded(ZigbeeNode *node);
    void nodeRemoved(ZigbeeNode *node);

    void stateChanged(State state);
    void errorOccured(Error error);

public slots:
    virtual void startNetwork() = 0;
    virtual void stopNetwork() = 0;

};

#endif // ZIGBEENETWORK_H
