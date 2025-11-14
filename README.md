# nymea-zigbee
----------------------

nymea-zigbee provides a reusable ZigBee coordinator library that can be embedded
in nymea-based products or standalone gateway projects. It provides a clean Qt
API to handle network management, ZCL data types, and manufacturer specific
extensions.

## Highlights

* Modular backend architecture with native implementations for TI, NXP and deCONZ adapters.
* Full ZigBee Cluster Library implementation with HVAC, lighting, smart energy and security clusters.
* QML/Qt-friendly C++ API, including a pkg-config file for downstream projects.
* Optional QCA2 integration for TI adapters (enabled automatically when the dependency is available).

For a full featured ZigBee coordinator/gateway implementation based on this
library, please see https://github.com/nymea/nymea.

## Building from source

nymea-zigbee builds with qmake against either Qt 5.15 or newer Qt 6 releases.
Typical build dependencies on Debian/Ubuntu are: `qtbase5-dev` (or `qt6-base-dev`),
`qtserialport`, `libudev-dev`, and optionally `libqca2-dev` for TI adapter
support. A minimal build looks like this:

```bash
mkdir build
cd build
qmake ..
make -j$(nproc)
```

You can install the resulting library with `make install` or by using the
packaging rules under `debian/`.

## Supported ZigBee adapters

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

## License

nymea-zigbee is licensed under the terms of the GNU Lesser General Public
License version 3 or (at your option) any later version. See the SPDX headers in
each source file and `LICENSE.LGPL3` for the complete text.

