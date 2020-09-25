include(../config.pri)

CONFIG += console
CONFIG -= app_bundle

TARGET = zigbee-cli

LIBS += -L$$buildDir/libnymea-zigbee -lnymea-zigbee1

INCLUDEPATH += ../libnymea-zigbee/

target.path = /usr/bin
INSTALLS += target

SOURCES += main.cpp \
    #core.cpp \
    #terminalcommander.cpp \
    #terminalcommand.cpp

HEADERS += \
    #core.h \
    #terminalcommander.h \
    #terminalcommand.h
