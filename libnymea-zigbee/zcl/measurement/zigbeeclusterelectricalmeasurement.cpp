#include "zigbeeclusterelectricalmeasurement.h"

#include <QDateTime>
#include <QDataStream>
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(dcZigbeeCluster)

ZigbeeClusterElectricalMeasurement::ZigbeeClusterElectricalMeasurement(ZigbeeNetwork *network, ZigbeeNode *node, ZigbeeNodeEndpoint *endpoint, Direction direction, QObject *parent):
    ZigbeeCluster(network, node, endpoint, ZigbeeClusterLibrary::ClusterIdElectricalMeasurement, direction, parent)
{

}

quint16 ZigbeeClusterElectricalMeasurement::activePowerPhaseA() const
{
    return m_activePowerPhaseA;
}

quint16 ZigbeeClusterElectricalMeasurement::acPowerMultiplier() const
{
    return m_acPowerMultiplier;
}

quint16 ZigbeeClusterElectricalMeasurement::acPowerDivisor() const
{
    return m_acPowerDivisor;
}

void ZigbeeClusterElectricalMeasurement::setAttribute(const ZigbeeClusterAttribute &attribute)
{
    ZigbeeCluster::setAttribute(attribute);

    switch (attribute.id()) {
    case AttributeACPhaseAMeasurementActivePower:
        qCDebug(dcZigbeeCluster) << "Active power changed" << attribute.dataType() << attribute.dataType().toInt16();
        m_activePowerPhaseA = attribute.dataType().toInt16();
        emit activePowerPhaseAChanged(m_activePowerPhaseA);
        break;
    case AttributeACFormattingPowerMultiplier:
        m_acPowerMultiplier = attribute.dataType().toUInt16();
        break;
    case AttributeACFormattingPowerDivisor:
        m_acPowerDivisor = attribute.dataType().toUInt16();
        break;
    }
}

void ZigbeeClusterElectricalMeasurement::processDataIndication(ZigbeeClusterLibrary::Frame frame)
{
    switch (m_direction) {
    case Client:
        qCWarning(dcZigbeeCluster()) << "ElectricalMeasurement: Unhandled ZCL indication in" << m_node << m_endpoint << this << frame;
        break;
    case Server:
        ServerCommand command = static_cast<ServerCommand>(frame.header.command);
        switch (command) {
        case CommandGetProfileInfoResponse: {
            QDataStream stream(frame.payload);
            stream.setByteOrder(QDataStream::LittleEndian);
            quint8 profileCount, profileIntervalPeriod, maxNumberOfIntervals;
            QList<quint16> attributes;
            stream >> profileCount >> profileIntervalPeriod >> maxNumberOfIntervals;
            while (!stream.atEnd()) {
                stream >> attributes;
            }
            qCDebug(dcZigbeeCluster()) << "ElectricalMeasurement: GetProfileInfoResponse received:" << profileCount << static_cast<ProfileIntervalPeriod>(profileIntervalPeriod) << maxNumberOfIntervals << attributes;
            emit getProfileInfoResponse(profileCount, static_cast<ProfileIntervalPeriod>(profileIntervalPeriod), maxNumberOfIntervals, attributes);
            break;
        }
        case CommandGetMeasurementProfileResponse: {
            QDataStream stream(frame.payload);
            stream.setByteOrder(QDataStream::LittleEndian);
            quint32 startTime;
            // According to the spec, attributeId is 1 octet, however, normally an attributeId is 2 octets...
            quint8 status, profileIntervalPeriod, numberOfIntevalsDelivered, attributeId;
            QList<quint16> values;
            stream >> startTime >> status >> profileIntervalPeriod >> numberOfIntevalsDelivered >> attributeId;
            while (!stream.atEnd()) {
                quint16 value;
                stream >> value;
                values.append(value);
            }            
            qCDebug(dcZigbeeCluster()) << "ElectricalMeasurement: GetMeasurementProfileInfoResponse:" << QDateTime::fromMSecsSinceEpoch((qulonglong)startTime * 1000) << static_cast<MeasurementStatus>(status) << static_cast<ProfileIntervalPeriod>(profileIntervalPeriod) << numberOfIntevalsDelivered << attributeId << values;
            emit getMeasurementProfileInfoResponse(QDateTime::fromMSecsSinceEpoch((qulonglong)startTime * 1000), static_cast<MeasurementStatus>(status), static_cast<ProfileIntervalPeriod>(profileIntervalPeriod), numberOfIntevalsDelivered, attributeId, values);
            break;
        }
        default:
            qCInfo(dcZigbeeCluster()) << "ElectricalMeasurement: Ignoring out of spec command:" << m_node << m_endpoint << this << frame << m_direction;
        }

        break;
    }
}

ZigbeeClusterReply *ZigbeeClusterElectricalMeasurement::readFormatting()
{
    ZigbeeClusterReply *readDivisorReply = readAttributes({ZigbeeClusterElectricalMeasurement::AttributeACFormattingPowerDivisor, ZigbeeClusterElectricalMeasurement::AttributeACFormattingPowerMultiplier});
    connect(readDivisorReply, &ZigbeeClusterReply::finished, this, [=](){
        if (readDivisorReply->error() != ZigbeeClusterReply::ErrorNoError) {
            qCWarning(dcZigbeeCluster()) << "Failed to read formatting." << readDivisorReply->error();
            return;
        }
    });
    return readDivisorReply;

}
