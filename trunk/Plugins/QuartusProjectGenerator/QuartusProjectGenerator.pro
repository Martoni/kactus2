# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = QuartusProjectGenerator
DESTDIR = ../../executable/Plugins
QT += core xml widgets gui printsupport
CONFIG += plugin release
DEFINES += QUARTUSGENERATOR_LIB QT_DLL QT_HELP_LIB QT_WIDGETS_LIB
INCLUDEPATH += ./GeneratedFiles \
    . \
    ./../.. \
    $(QTDIR)/../qttools/include \
    $(QTDIR)/../qttools/include/QtHelp \
    ./GeneratedFiles/Release
LIBS += -L"./../../executable" \
    -lIPXACTmodels
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(QuartusProjectGenerator.pri)

target.path = /usr/share/kactus2/plugins
INSTALLS += target