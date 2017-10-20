#include "zigbeebridgecontroller.h"
#include "loggingcategory.h"

ZigbeeBridgeController::ZigbeeBridgeController(const QString &serialPort, QObject *parent) :
    QObject(parent)
{
    m_interface = new ZigbeeInterface(this);

    connect(m_interface, &ZigbeeInterface::messageReceived, this, &ZigbeeBridgeController::onMessageReceived);

    if (!m_interface->enable(serialPort)) {
        qCWarning(dcZigbeeController()) << "Could not enable ZigbeeInterface on" << serialPort;
        return;
    }
}

bool ZigbeeBridgeController::available() const
{
    return m_interface->available();
}

void ZigbeeBridgeController::sendMessage(ZigbeeInterfaceReply *reply)
{
    if (!reply)
        return;

    m_currentReply = reply;
    qCDebug(dcZigbeeController()) << "Sending request:" << reply->request().description();

    m_interface->sendMessage(reply->request().message());
    reply->startTimer(reply->request().timeoutIntervall());
}

void ZigbeeBridgeController::onMessageReceived(const ZigbeeInterfaceMessage &message)
{
    // Check if we have a current reply
    if (m_currentReply) {
        if (message.messageType() == Zigbee::MessageTypeStatus) {
            // We have a status message for the current reply
            m_currentReply->setStatusMessage(message);

            // TODO: check if success, if not, finish reply

        } else if (message.messageType() == m_currentReply->request().expectedAdditionalMessageType()) {
            m_currentReply->setAdditionalMessage(message);
        }

        // Check if request is complete
        if (m_currentReply->isComplete()) {
            m_currentReply->setFinished();
            // Note: the request class has to take care about the reply object
            m_currentReply = nullptr;

            if (!m_replyQueue.isEmpty())
                sendMessage(m_replyQueue.dequeue());

            return;
        }
    }

    // Not a reply message
    emit messageReceived(message);
}

void ZigbeeBridgeController::onReplyTimeout()
{
    m_currentReply->setFinished();
    m_currentReply = nullptr;

    if (!m_replyQueue.isEmpty())
        sendMessage(m_replyQueue.dequeue());

}

ZigbeeInterfaceReply *ZigbeeBridgeController::sendRequest(const ZigbeeInterfaceRequest &request)
{
    // Create Reply
    ZigbeeInterfaceReply *reply = new ZigbeeInterfaceReply(request);
    connect(reply, &ZigbeeInterfaceReply::timeout, this, &ZigbeeBridgeController::onReplyTimeout);

    // If reply running, enqueue, else send request
    if (m_currentReply) {
        m_replyQueue.enqueue(reply);
    } else {
        sendMessage(reply);
    }

    return reply;
}
