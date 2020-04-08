include(../config.pri)

TARGET = nymea-zigbee1
TEMPLATE = lib

SOURCES += \
    deconz/interface/zigbeeinterfacedeconz.cpp \
    deconz/interface/zigbeeinterfacedeconzreply.cpp \
    deconz/zigbeebridgecontrollerdeconz.cpp \
    deconz/zigbeenetworkdeconz.cpp \
    deconz/zigbeenodedeconz.cpp \
    deconz/zigbeenodeendpointdeconz.cpp \
    nxp/interface/zigbeeinterface.cpp \
    nxp/interface/zigbeeinterfacemessage.cpp \
    nxp/interface/zigbeeinterfacerequest.cpp \
    nxp/interface/zigbeeinterfacereply.cpp \
    nxp/zigbeenetworknxp.cpp \
    nxp/zigbeebridgecontrollernxp.cpp \
    nxp/zigbeenodeendpointnxp.cpp \
    nxp/zigbeenodenxp.cpp \
    zigbeeadpu.cpp \
    zigbeebridgecontroller.cpp \
    zigbeechannelmask.cpp \
    zigbeecluster.cpp \
    zigbeeclusterattribute.cpp \
    zigbeedeviceprofile.cpp \
    zigbeemanufacturer.cpp \
    zigbeenetwork.cpp \
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

HEADERS += \
    deconz/interface/deconz.h \
    deconz/interface/zigbeeinterfacedeconz.h \
    deconz/interface/zigbeeinterfacedeconzreply.h \
    deconz/zigbeebridgecontrollerdeconz.h \
    deconz/zigbeenetworkdeconz.h \
    deconz/zigbeenodedeconz.h \
    deconz/zigbeenodeendpointdeconz.h \
    nxp/interface/zigbeeinterface.h \
    nxp/interface/zigbeeinterfacemessage.h \
    nxp/interface/zigbeeinterfacerequest.h \
    nxp/interface/zigbeeinterfacereply.h \
    nxp/zigbeenetworknxp.h \
    nxp/zigbeebridgecontrollernxp.h \
    nxp/zigbeenodeendpointnxp.h \
    nxp/zigbeenodenxp.h \
    zigbeeadpu.h \
    zigbeebridgecontroller.h \
    zigbeechannelmask.h \
    zigbeecluster.h \
    zigbeeclusterattribute.h \
    zigbeedeviceprofile.h \
    zigbeemanufacturer.h \
    zigbeenetwork.h \
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

