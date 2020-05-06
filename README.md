# nymea-zigbee
----------------------

This repository contains the nymea-zigbee library and tools.

# Supported hardware

Depending on your available hardware following gateway modules are supported

## NXP

* JN5168 (SoM)
* JN5169 (USB Stick)

## deCONZ

* ConBee
* RaspBee
* ConBee II
* RaspBee II


## Structure

ZigbeeNetwork
  - ZDO handler
  - ZCL handler
    - ZHA handler
    - ZLL handler
    - GP handler

  - [ZigbeeNode]
    - [ZigbeeNodeEndpoints]
        - Profile
