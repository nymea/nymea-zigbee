QT -= gui
QT += serialport

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = qt-zigbee

target.path = /usr/bin
INSTALLS += target

SOURCES += main.cpp \
    core.cpp \
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
    zigbeeaddress.cpp

HEADERS += \
    core.h \
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
    zigbeeaddress.h
