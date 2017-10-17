QT -= gui
QT += serialport

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = zigbee-daemon

target.path = /usr/bin
INSTALLS += target

SOURCES += main.cpp \
    core.cpp \
    zigbeeinterface.cpp \
    zigbeemanager.cpp \
    zigbee.cpp

HEADERS += \
    core.h \
    zigbeeinterface.h \
    zigbeemanager.h \
    zigbee.h
