# nymea-zigbee
----------------------

This repository contains the nymea-zigbee library and tools.

# Supported hardware

Depending on your available hardware following gateway modules are supported

## NXP

> Note: the firmware erquires an entire rework and implement the APS layer

* ~~JN5168 (SoM)~~
* ~~JN5169 (USB Stick)~~

## deCONZ

* ConBee
* RaspBee
* ConBee II
* RaspBee II


## Hidden base groups

Each sensor/remote cluster will be bound automatically to the sensors group in order to receive commands from the device. The lights group can be used to switch all lights within the network with one command.

- Sensors: `0xfff0`
- Light: `0xfff1`


