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

#include <QDataStream>

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

void ZigbeeNetworkDeconz::startNetworkInternally()
{
    qCDebug(dcZigbeeNetwork()) << "Start network internally";

    // Check if we have to create a pan ID and select the channel
    if (extendedPanId() == 0) {
        m_createNewNetwork = true;
        setExtendedPanId(ZigbeeUtils::generateRandomPanId());
        qCDebug(dcZigbeeNetwork()) << "Created new PAN ID:" << extendedPanId();
    }

    if (securityConfiguration().networkKey().isNull()) {
        m_createNewNetwork = true;
        qCDebug(dcZigbeeNetwork()) << "Create a new network key";
        ZigbeeNetworkKey key = ZigbeeNetworkKey::generateKey();
        m_securityConfiguration.setNetworkKey(key);
    }

    qCDebug(dcZigbeeNetwork()) << "Using" << securityConfiguration().networkKey() << "network link key";
    qCDebug(dcZigbeeNetwork()) << "Using" <<  securityConfiguration().globalTrustCenterLinkKey() << "global trust center link key";


    // - Read the firmware version
    // - Read the network configuration parameters
    // - Read the network state

    // - If network running and we don't have configurations, write them
    // - If network running and configurations match, we are done

    // Read the firmware version
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
        connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply, firmwareVersion](){
            if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                qCWarning(dcZigbeeController()) << "Could not read network parameters during network start up." << reply->statusCode();
                // FIXME: set an appropriate error
                return;
            }

            qCDebug(dcZigbeeNetwork()) << "Reading network parameters finished successfully.";
            QString protocolVersion = QString("%1.%2").arg(m_controller->networkConfiguration().protocolVersion & 0xFF00)
                    .arg(m_controller->networkConfiguration().protocolVersion & 0x00FF);

            m_controller->setFirmwareVersionString(QString("%1 - %2").arg(firmwareVersion).arg(protocolVersion));

            ZigbeeInterfaceDeconzReply *reply = m_controller->requestDeviceState();
            connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
                if (reply->statusCode() != Deconz::StatusCodeSuccess) {
                    qCWarning(dcZigbeeController()) << "Could not read device state during network start up." << reply->statusCode();
                    // FIXME: set an appropriate error
                    return;
                }

                qCDebug(dcZigbeeNetwork()) << "Read device state finished successfully";
                QDataStream stream(reply->responseData());


            });



            if (m_createNewNetwork) {
                // Write the configurations which need to be changed


                // Initialize coordinator node


            } else {
                // Get the network state and start the network if required



            }



        });
    });
}

void ZigbeeNetworkDeconz::onControllerAvailableChanged(bool available)
{
    qCDebug(dcZigbeeNetwork()) << "Hardware controller is" << (available ? "now available" : "not available");

    if (!available) {
        setError(ErrorHardwareUnavailable);
        m_permitJoining = false;
        emit permitJoiningChanged(m_permitJoining);
        setState(StateOffline);
    } else {
        m_error = ErrorNoError;
        m_permitJoining = false;
        emit permitJoiningChanged(m_permitJoining);
        setState(StateStarting);
        startNetworkInternally();
    }
}

void ZigbeeNetworkDeconz::startNetwork()
{
    loadNetwork();

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
    ZigbeeInterfaceDeconzReply *reply = m_controller->requestChangeNetworkState(Deconz::NetworkStateOffline);
    setState(StateStopping);
    connect(reply, &ZigbeeInterfaceDeconzReply::finished, this, [this, reply](){
        if (reply->statusCode() != Deconz::StatusCodeSuccess) {
            qCWarning(dcZigbeeController()) << "Could not leave network." << reply->statusCode();
            // FIXME: set an appropriate error
            return;
        }

        qCDebug(dcZigbeeNetwork()) << "Network left successfully";
        setState(StateOffline);
    });
}

void ZigbeeNetworkDeconz::reset()
{
    // TODO
}

void ZigbeeNetworkDeconz::factoryResetNetwork()
{
    // Wipe settings, and reconfigure network
}
