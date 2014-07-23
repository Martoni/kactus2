# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = MemoryMapHeaderGenerator
DESTDIR = ../../executable/Plugins
QT += core xml widgets gui printsupport
CONFIG += plugin release
DEFINES += MEMORYMAPHEADERGENERATOR_LIB QT_DLL QT_HELP_LIB QT_PRINTSUPPORT_LIB QT_WIDGETS_LIB QT_XML_LIB QT_XMLPATTERNS_LIB
INCLUDEPATH += ./GeneratedFiles \
    ./GeneratedFiles/$(Configuration) \
    . \
    ./../.. \
    $(QTDIR)/../qttools/include \
    $(QTDIR)/../qttools/include/QtHelp \
    $(QTDIR)/../qtxmlpatterns/include/QtXmlPatterns \
    $(QTDIR)/../qtxmlpatterns/include \
    ./GeneratedFiles/Release
LIBS += -L"./../../executable" \
    -lKactus2 \
    -lIPXACTmodels
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(MemoryMapHeaderGenerator.pri)

target.path = /usr/share/kactus2/plugins
INSTALLS += target
