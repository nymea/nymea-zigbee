include(../config.pri)

TARGET = qtzigbee1
TEMPLATE = lib

target.path = /usr/lib

SOURCES += \
    interface/zigbeeinterface.cpp \
    interface/zigbeeinterfacemessage.cpp \
    interface/zigbeeinterfacerequest.cpp \
    interface/zigbeeinterfacereply.cpp \
    zigbeenetworkmanager.cpp \
    zigbee.cpp \
    loggingcategory.cpp \
    zigbeebridgecontroller.cpp \
    zigbeeutils.cpp \
    zigbeenode.cpp \
    zigbeeaddress.cpp \

HEADERS += \
    interface/zigbeeinterface.h \
    interface/zigbeeinterfacemessage.h \
    interface/zigbeeinterfacerequest.h \
    interface/zigbeeinterfacereply.h \
    zigbeenetworkmanager.h \
    zigbee.h \
    loggingcategory.h \
    zigbeebridgecontroller.h \
    zigbeeutils.h \
    zigbeenode.h \
    zigbeeaddress.h \

INSTALLS += target

# install header file with relative subdirectory
for(header, HEADERS) {
    path = /usr/include/qtzigbee/$${dirname(header)}
    eval(headers_$${path}.files += $${header})
    eval(headers_$${path}.path = $${path})
    eval(INSTALLS *= headers_$${path})
}
