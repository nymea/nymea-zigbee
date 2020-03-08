QT += gui serialport

QMAKE_CXXFLAGS += -Werror -std=c++11
QMAKE_LFLAGS += -std=c++11

sourceDir = $$PWD
buildDir = $$shadowed($$PWD)
