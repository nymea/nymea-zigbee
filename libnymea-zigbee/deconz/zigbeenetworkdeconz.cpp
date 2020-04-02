/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea-zigbee.
* This project including source code and documentation is protected by copyright law, and
* remains the property of nymea GmbH. All rights, including reproduction, publication,
* editing and translation, are reserved. The use of this project is subject to the terms of a
* license agreement to be concluded with nymea GmbH in accordance with the terms
* of use of nymea GmbH, available under https://nymea.io/license
*
* GNU Lesser General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the terms of the GNU
* Lesser General Public License as published by the Free Software Foundation; version 3.
* this project is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License along with this project.
* If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under contact@nymea.io
* or see our FAQ/Licensing Information on https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "zigbeenetworkdeconz.h"
#include "loggingcategory.h"
#include "zigbeeutils.h"

ZigbeeNetworkDeconz::ZigbeeNetworkDeconz(QObject *parent) :
    ZigbeeNetwork(parent)
{
    m_controller = new ZigbeeBridgeControllerDeconz(this);
    //connect(m_controller, &ZigbeeBridgeControllerDeconz::messageReceived, this, &ZigbeeNetworkDeconz::onMessageReceived);
    connect(m_controller, &ZigbeeBridgeControllerDeconz::availableChanged, this, &ZigbeeNetworkDeconz::onControllerAvailableChanged);
}

ZigbeeBridgeController *ZigbeeNetworkDeconz::bridgeController() const
{
    if (m_controller)
        return qobject_cast<ZigbeeBridgeController *>(m_controller);

    return nullptr;
}

ZigbeeNode *ZigbeeNetworkDeconz::createNode(QObject *parent)
{
    //FIXME
    Q_UNUSED(parent)
    return nullptr;
}

void ZigbeeNetworkDeconz::setPermitJoiningInternal(bool permitJoining)
{
    //FIXME
    Q_UNUSED(permitJoining)
}

void ZigbeeNetworkDeconz::onControllerAvailableChanged(bool available)
{
    qCDebug(dcZigbeeNetwork()) << "Hardware controller is" << (available ? "now available" : "not available");

    if (!available) {
        //        foreach (ZigbeeNode *node, nodes()) {
        //            qobject_cast<ZigbeeNodeNxp *>(node)->setConnected(false);
        //        }

        setError(ErrorHardwareUnavailable);
        m_permitJoining = false;
        emit permitJoiningChanged(m_permitJoining);
        //setStartingState(StartingStateNone);
        setState(StateOffline);
    } else {
        m_error = ErrorNoError;
        m_permitJoining = false;
        emit permitJoiningChanged(m_permitJoining);
        // Note: if we are factory resetting, erase also the data on the controller before resetting
        //        if (m_factoryResetting) {
        //            setStartingState(StartingStateErase);
        //        } else {
        //            setStartingState(StartingStateReset);
        //        }

        setState(StateStarting);

        // FIXME: do this in the startig state machine
        ZigbeeInterfaceDeconzReply *reply = m_controller->requestVersion();
        connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
            if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                qCWarning(dcZigbeeController()) << "Request" << reply->command() << "finished with error" << reply->statusCode();
                // FIXME: set an appropriate error
                return;
            }
            qCDebug(dcZigbeeNetwork()) << "Version request finished" << reply->statusCode() << ZigbeeUtils::convertByteArrayToHexString(reply->responseData());
            // Note: version is an uint32 value, little endian, but we can read the individual bytes in reversed order
            quint8 majorVersion = static_cast<quint8>(reply->responseData().at(3));
            quint8 minorVersion = static_cast<quint8>(reply->responseData().at(2));
            Deconz::Platform platform = static_cast<Deconz::Platform>(reply->responseData().at(1));
            QString firmwareVersion = QString("%1.%2").arg(majorVersion).arg(minorVersion);
            qCDebug(dcZigbeeNetwork()) << "Firmware version" << firmwareVersion << platform;

            // Read all network parameters
            ZigbeeInterfaceDeconzReply *reply = m_controller->readNetworkParameters();
            connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                    qCWarning(dcZigbeeController()) << "Could not read network parameters during network start up." << reply->statusCode();
                    // FIXME: set an appropriate error
                    return;
                }

                qCDebug(dcZigbeeNetwork()) << "Reading network parameters finished successfully.";




            });
        });
    }
}

void ZigbeeNetworkDeconz::startNetwork()
{
    loadNetwork();

    // Check if we have to create a pan ID and select the channel
    if (extendedPanId() == 0) {
        setExtendedPanId(ZigbeeUtils::generateRandomPanId());
        qCDebug(dcZigbeeNetwork()) << "Created new PAN ID:" << extendedPanId();
    }

    if (securityConfiguration().networkKey().isNull()) {
        qCDebug(dcZigbeeNetwork()) << "Create a new network key";
        ZigbeeNetworkKey key = ZigbeeNetworkKey::generateKey();
        m_securityConfiguration.setNetworkKey(key);
    }

    qCDebug(dcZigbeeNetwork()) << "Using network link key" << securityConfiguration().networkKey();
    qCDebug(dcZigbeeNetwork()) << "Using global trust center link key" << securityConfiguration().globalTrustCenterLinkKey();

    // TODO: get desired channel, by default use all

    if (!m_controller->enable(serialPortName(), serialBaudrate())) {
        m_permitJoining = false;
        emit permitJoiningChanged(m_permitJoining);
        setState(StateOffline);
        //setStartingState(StartingStateNone);
        setError(ErrorHardwareUnavailable);
        return;
    }

    m_permitJoining = false;
    emit permitJoiningChanged(m_permitJoining);
    // Note: wait for the controller available signal and start the initialization there
}

void ZigbeeNetworkDeconz::stopNetwork()
{

}

void ZigbeeNetworkDeconz::reset()
{

}

void ZigbeeNetworkDeconz::factoryResetNetwork()
{

}
