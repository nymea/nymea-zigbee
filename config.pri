QT += serialport sql

QMAKE_CXXFLAGS += -Werror -std=c++11 -z defs
QMAKE_LFLAGS += -std=c++11

qtHaveModule(gui):!disablegui {
    QT += gui
    DEFINES += WITH_GUI
} else {
    message("Building without gui support")
    QT -= gui
}

gcc {
    COMPILER_VERSION = $$system($$QMAKE_CXX " -dumpversion")
    COMPILER_MAJOR_VERSION = $$str_member($$COMPILER_VERSION)
    greaterThan(COMPILER_MAJOR_VERSION, 7): QMAKE_CXXFLAGS += -Wno-deprecated-copy
}

sourceDir = $$PWD
buildDir = $$shadowed($$PWD)
