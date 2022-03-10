# nymea-zigbee
----------------------

This repository contains the nymea-zigbee library and tools.

nymea-zigbee is a general purpose ZigBee coordinator library to build ZigBee coordinators/gateways.
The provided zigbee-cli is a minimal ZigBee coordinator implementation which allows to host a ZigBee
network for devices to join and interact with each other but without interacting with the devices.

For a full fetaured ZigBee coordinator/gateway implementation based on this library, please see 
https://github.com/nymea/nymea.


# Supported ZigBee adapters

## TI z-Stack

All USB and serial port adapters based on the Texas Instruments CC1352/CC2652
chipset are supported, provided they are flashed with the z-Stack coordinator firmware.

Pre-built binaries of the firmware are provided by Koenkk:
https://github.com/Koenkk/Z-Stack-firmware/tree/master/coordinator

## NXP

The following NXP chip based adapters are supported, provided they are flashed with
the nymea coordinator firmware found in this repository.

* JN5168 (SoM)
* JN5169 (USB Stick)

## deCONZ

All deCONZ based adapters are supported, with the standard firmware preinstalled.
It is recommended to update to the latest firmware.

* ConBee
* RaspBee
* ConBee II
* RaspBee II


## Hidden base groups

Each sensor/remote cluster will be bound automatically to the sensors group in order to receive commands from the device. The lights group can be used to switch all lights within the network with one command.

- Sensors: `0xfff0`
- Light: `0xfff1`


