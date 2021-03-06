# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = PadsPartGenerator
DESTDIR = ../../executable/Plugins
QT += core xml widgets gui
CONFIG += release
DEFINES += QT_DLL QT_WIDGETS_LIB QT_XML_LIB
INCLUDEPATH += ./GeneratedFiles \
    . \
    ./GeneratedFiles/Release \
    $(QTDIR)/../qtxmlpatterns/include/QtXmlPatterns \
    $(QTDIR)/../qtxmlpatterns/include \
    ./../..
LIBS += -L"./../../executable" \
    -lKactus2 \
    -lIPXACTmodels
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(PadsPartGenerator.pri)

target.path = /usr/share/kactus2/plugins
INSTALLS += target
