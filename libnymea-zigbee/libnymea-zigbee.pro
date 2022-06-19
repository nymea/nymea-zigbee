include(../config.pri)

TARGET = nymea-zigbee
TEMPLATE = lib

CONFIG += link_pkgconfig
packagesExist(libudev) {
    message(Build with libudev support)
    PKGCONFIG += libudev
} else {
    message(Build without libudev support)
    DEFINES += DISABLE_UDEV
}

PKGCONFIG += qca2-qt5

SOURCES += \
    backends/deconz/interface/zigbeeinterfacedeconz.cpp \
    backends/deconz/interface/zigbeeinterfacedeconzreply.cpp \
    backends/deconz/zigbeebridgecontrollerdeconz.cpp \
    backends/deconz/zigbeenetworkdeconz.cpp \
    backends/nxp/firmwareupdatehandlernxp.cpp \
    backends/nxp/interface/zigbeeinterfacenxp.cpp \
    backends/nxp/interface/zigbeeinterfacenxpreply.cpp \
    backends/nxp/zigbeebridgecontrollernxp.cpp \
    backends/nxp/zigbeenetworknxp.cpp \
    backends/ti/interface/zigbeeinterfaceti.cpp \
    backends/ti/interface/zigbeeinterfacetireply.cpp \
    backends/ti/zigbeebridgecontrollerti.cpp \
    backends/ti/zigbeenetworkti.cpp \
    zcl/closures/zigbeeclusterdoorlock.cpp \
    zcl/general/zigbeeclusteranaloginput.cpp \
    zcl/general/zigbeeclusteranalogoutput.cpp \
    zcl/general/zigbeeclusteranalogvalue.cpp \
    zcl/general/zigbeeclusterbinaryinput.cpp \
    zcl/general/zigbeeclusterbinaryoutput.cpp \
    zcl/general/zigbeeclusterbinaryvalue.cpp \
    zcl/general/zigbeeclustergroups.cpp \
    zcl/general/zigbeeclusteridentify.cpp \
    zcl/general/zigbeeclusterlevelcontrol.cpp \
    zcl/general/zigbeeclustermultistateinput.cpp \
    zcl/general/zigbeeclustermultistateoutput.cpp \
    zcl/general/zigbeeclustermultistatevalue.cpp \
    zcl/general/zigbeeclusteronoff.cpp \
    zcl/general/zigbeeclusterpowerconfiguration.cpp \
    zcl/general/zigbeeclusterscenes.cpp \
    zcl/general/zigbeeclustertime.cpp \
    zcl/hvac/zigbeeclusterthermostat.cpp \
    zcl/lighting/zigbeeclustercolorcontrol.cpp \
    zcl/manufacturerspecific/philips/zigbeeclustermanufacturerspecificphilips.cpp \
    zcl/measurement/zigbeeclusterelectricalmeasurement.cpp \
    zcl/measurement/zigbeeclusterilluminancemeasurement.cpp \
    zcl/measurement/zigbeeclusteroccupancysensing.cpp \
    zcl/measurement/zigbeeclusterpressuremeasurement.cpp \
    zcl/measurement/zigbeeclusterrelativehumiditymeasurement.cpp \
    zcl/measurement/zigbeeclustertemperaturemeasurement.cpp \
    zcl/ota/zigbeeclusterota.cpp \
    zcl/security/zigbeeclusteriaswd.cpp \
    zcl/security/zigbeeclusteriaszone.cpp \
    zcl/smartenergy/zigbeeclustermetering.cpp \
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
    zigbeereply.cpp \
    zigbeesecurityconfiguration.cpp \
    zigbeeuartadapter.cpp \
    zigbeeuartadaptermonitor.cpp \
    zigbeeutils.cpp \
    zigbeenode.cpp \
    zigbeeaddress.cpp

HEADERS += \
    backends/deconz/interface/deconz.h \
    backends/deconz/interface/zigbeeinterfacedeconz.h \
    backends/deconz/interface/zigbeeinterfacedeconzreply.h \
    backends/deconz/zigbeebridgecontrollerdeconz.h \
    backends/deconz/zigbeenetworkdeconz.h \
    backends/nxp/firmwareupdatehandlernxp.h \
    backends/nxp/interface/nxp.h \
    backends/nxp/interface/zigbeeinterfacenxp.h \
    backends/nxp/interface/zigbeeinterfacenxpreply.h \
    backends/nxp/zigbeebridgecontrollernxp.h \
    backends/nxp/zigbeenetworknxp.h \
    backends/ti/interface/ti.h \
    backends/ti/interface/zigbeeinterfaceti.h \
    backends/ti/interface/zigbeeinterfacetireply.h \
    backends/ti/zigbeebridgecontrollerti.h \
    backends/ti/zigbeenetworkti.h \
    zcl/closures/zigbeeclusterdoorlock.h \
    zcl/general/zigbeeclusteranaloginput.h \
    zcl/general/zigbeeclusteranalogoutput.h \
    zcl/general/zigbeeclusteranalogvalue.h \
    zcl/general/zigbeeclusterbinaryinput.h \
    zcl/general/zigbeeclusterbinaryoutput.h \
    zcl/general/zigbeeclusterbinaryvalue.h \
    zcl/general/zigbeeclustergroups.h \
    zcl/general/zigbeeclusteridentify.h \
    zcl/general/zigbeeclusterlevelcontrol.h \
    zcl/general/zigbeeclustermultistateinput.h \
    zcl/general/zigbeeclustermultistateoutput.h \
    zcl/general/zigbeeclustermultistatevalue.h \
    zcl/general/zigbeeclusteronoff.h \
    zcl/general/zigbeeclusterpowerconfiguration.h \
    zcl/general/zigbeeclusterscenes.h \
    zcl/general/zigbeeclustertime.h \
    zcl/hvac/zigbeeclusterthermostat.h \
    zcl/lighting/zigbeeclustercolorcontrol.h \
    zcl/manufacturerspecific/philips/zigbeeclustermanufacturerspecificphilips.h \
    zcl/measurement/zigbeeclusterelectricalmeasurement.h \
    zcl/measurement/zigbeeclusterilluminancemeasurement.h \
    zcl/measurement/zigbeeclusteroccupancysensing.h \
    zcl/measurement/zigbeeclusterpressuremeasurement.h \
    zcl/measurement/zigbeeclusterrelativehumiditymeasurement.h \
    zcl/measurement/zigbeeclustertemperaturemeasurement.h \
    zcl/ota/zigbeeclusterota.h \
    zcl/security/zigbeeclusteriaswd.h \
    zcl/security/zigbeeclusteriaszone.h \
    zcl/smartenergy/zigbeeclustermetering.h \
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
    zigbeereply.h \
    zigbeesecurityconfiguration.h \
    zigbeeuartadapter.h \
    zigbeeuartadaptermonitor.h \
    zigbeeutils.h \
    zigbeenode.h \
    zigbeeaddress.h

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

