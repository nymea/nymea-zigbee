include(../config.pri)

TARGET = nymea-zigbee1
TEMPLATE = lib

SOURCES += \
    backends/deconz/interface/zigbeeinterfacedeconz.cpp \
    backends/deconz/interface/zigbeeinterfacedeconzreply.cpp \
    backends/deconz/zigbeebridgecontrollerdeconz.cpp \
    backends/deconz/zigbeenetworkdeconz.cpp \
    zcl/general/zigbeeclusteridentify.cpp \
    zcl/general/zigbeeclusterlevelcontrol.cpp \
    zcl/general/zigbeeclusteronoff.cpp \
    zcl/lighting/zigbeeclustercolorcontrol.cpp \
    zcl/measurement/zigbeeclusterilluminancemeasurment.cpp \
    zcl/measurement/zigbeeclusteroccupancysensing.cpp \
    zcl/measurement/zigbeeclusterrelativehumiditymeasurement.cpp \
    zcl/measurement/zigbeeclustertemperaturemeasurement.cpp \
    zcl/security/zigbeeclusteriaszone.cpp \
    zcl/zigbeecluster.cpp \
    zcl/zigbeeclusterattribute.cpp \
    zcl/zigbeeclusterlibrary.cpp \
    zcl/zigbeeclusterreply.cpp \
    zcl/general/zigbeeclusterbasic.cpp \
    zdo/zigbeedeviceobject.cpp \
    zdo/zigbeedeviceobjectreply.cpp \
    zdo/zigbeedeviceprofile.cpp \
    zigbeeadpu.cpp \
    zigbeebridgecontroller.cpp \
    zigbeechannelmask.cpp \
    zigbeedatatype.cpp \
    zigbeemanufacturer.cpp \
    zigbeenetwork.cpp \
    zigbeenetworkdatabase.cpp \
    zigbeenetworkkey.cpp \
    zigbeenetworkmanager.cpp \
    zigbee.cpp \
    loggingcategory.cpp \
    zigbeenetworkreply.cpp \
    zigbeenetworkrequest.cpp \
    zigbeenodeendpoint.cpp \
    zigbeesecurityconfiguration.cpp \
    zigbeeutils.cpp \
    zigbeenode.cpp \
    zigbeeaddress.cpp \
    #    nxp/interface/zigbeeinterface.cpp \
    #    nxp/interface/zigbeeinterfacemessage.cpp \
    #    nxp/interface/zigbeeinterfacerequest.cpp \
    #    nxp/interface/zigbeeinterfacereply.cpp \
    #    nxp/zigbeenetworknxp.cpp \
    #    nxp/zigbeebridgecontrollernxp.cpp \
    #    nxp/zigbeenodeendpointnxp.cpp \
    #    nxp/zigbeenodenxp.cpp \

HEADERS += \
    backends/deconz/interface/deconz.h \
    backends/deconz/interface/zigbeeinterfacedeconz.h \
    backends/deconz/interface/zigbeeinterfacedeconzreply.h \
    backends/deconz/zigbeebridgecontrollerdeconz.h \
    backends/deconz/zigbeenetworkdeconz.h \
    zcl/general/zigbeeclusteridentify.h \
    zcl/general/zigbeeclusterlevelcontrol.h \
    zcl/general/zigbeeclusteronoff.h \
    zcl/lighting/zigbeeclustercolorcontrol.h \
    zcl/measurement/zigbeeclusterilluminancemeasurment.h \
    zcl/measurement/zigbeeclusteroccupancysensing.h \
    zcl/measurement/zigbeeclusterrelativehumiditymeasurement.h \
    zcl/measurement/zigbeeclustertemperaturemeasurement.h \
    zcl/security/zigbeeclusteriaszone.h \
    zcl/zigbeecluster.h \
    zcl/zigbeeclusterattribute.h \
    zcl/zigbeeclusterlibrary.h \
    zcl/zigbeeclusterreply.h \
    zcl/general/zigbeeclusterbasic.h \
    zdo/zigbeedeviceobject.h \
    zdo/zigbeedeviceobjectreply.h \
    zdo/zigbeedeviceprofile.h \
    zigbeeadpu.h \
    zigbeebridgecontroller.h \
    zigbeechannelmask.h \
    zigbeedatatype.h \
    zigbeemanufacturer.h \
    zigbeenetwork.h \
    zigbeenetworkdatabase.h \
    zigbeenetworkkey.h \
    zigbeenetworkmanager.h \
    zigbee.h \
    loggingcategory.h \
    zigbeenetworkreply.h \
    zigbeenetworkrequest.h \
    zigbeenodeendpoint.h \
    zigbeesecurityconfiguration.h \
    zigbeeutils.h \
    zigbeenode.h \
    zigbeeaddress.h \
    #    nxp/interface/zigbeeinterface.h \
    #    nxp/interface/zigbeeinterfacemessage.h \
    #    nxp/interface/zigbeeinterfacerequest.h \
    #    nxp/interface/zigbeeinterfacereply.h \
    #    nxp/zigbeenetworknxp.h \
    #    nxp/zigbeebridgecontrollernxp.h \
    #    nxp/zigbeenodeendpointnxp.h \
    #    nxp/zigbeenodenxp.h \

# install header file with relative subdirectory
for (header, HEADERS) {
    path = $$[QT_INSTALL_PREFIX]/include/nymea-zigbee/$${dirname(header)}
    eval(headers_$${path}.files += $${header})
    eval(headers_$${path}.path = $${path})
    eval(INSTALLS *= headers_$${path})
}

# define install target
target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

# Create pkgconfig file
CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_NAME = libnymea-zigbee
QMAKE_PKGCONFIG_DESCRIPTION = nymea-zigbee development library
QMAKE_PKGCONFIG_PREFIX = $$[QT_INSTALL_PREFIX]
QMAKE_PKGCONFIG_INCDIR = $$[QT_INSTALL_PREFIX]/include/nymea-zigbee/
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_VERSION = 1.0.0
QMAKE_PKGCONFIG_FILE = nymea-zigbee
QMAKE_PKGCONFIG_DESTDIR = pkgconfig

