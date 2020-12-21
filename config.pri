QT += gui serialport sql

QMAKE_CXXFLAGS += -Werror -std=c++11 -z defs
QMAKE_LFLAGS += -std=c++11

sourceDir = $$PWD
buildDir = $$shadowed($$PWD)
