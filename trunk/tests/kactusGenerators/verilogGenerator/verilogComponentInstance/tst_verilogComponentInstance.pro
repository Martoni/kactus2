#-----------------------------------------------------------------------------
# File: tst_verilogComponentInstance.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.27.2014
#
# Description:
# Qt project file for running unit tests for VerilogComponentInstance.
#-----------------------------------------------------------------------------

win32-msvc {
    TEMPLATE = vcapp
}
else {
    TEMPLATE = app
}

TARGET = tst_verilogComponentInstance

QT += core xml gui testlib
CONFIG += testcase console

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodelsd
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}

INCLUDEPATH += $$PWD/../../../../
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../../../
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./GeneratedFiles
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(tst_verilogComponentInstance.pri)