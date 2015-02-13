# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

# This is a reminder that you are using a generated .pro file.
# Remove it when you are finished editing this file.
message("You are running qmake on a generated .pro file. This may not work!")


HEADERS += ./veriloggeneratorplugin_global.h \
    ./VerilogGeneratorPlugin.h \
    ./VerilogGenerator/VerilogGenerator.h \
    ./common/Writer.h \
    ./common/WriterGroup.h \
    ./ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h \
    ./ComponentVerilogWriter/ComponentVerilogWriter.h \
    ./ModelParameterVerilogWriter/ModelParameterVerilogWriter.h \
    ./PortSorter/InterfaceDirectionNameSorter.h \
    ./PortSorter/PortSorter.h \
    ./PortVerilogWriter/PortVerilogWriter.h \
    ./VerilogHeaderWriter/VerilogHeaderWriter.h \
    ./VerilogWireWriter/VerilogWireWriter.h \
    ./CommentWriter/CommentWriter.h \
    ../../editors/ComponentEditor/common/ComponentParameterFinder.h \
    ../../editors/ComponentEditor/common/ExpressionFormatter.h \
    ../../editors/ComponentEditor/common/ParameterFinder.h
SOURCES += ./VerilogGeneratorPlugin.cpp \
    ./VerilogGenerator/VerilogGenerator.cpp \
    ./ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.cpp \
    ./ComponentVerilogWriter/ComponentVerilogWriter.cpp \
    ./ModelParameterVerilogWriter/ModelParameterVerilogWriter.cpp \
    ./PortSorter/InterfaceDirectionNameSorter.cpp \
    ./PortVerilogWriter/PortVerilogWriter.cpp \
    ./VerilogHeaderWriter/VerilogHeaderWriter.cpp \
    ./VerilogWireWriter/VerilogWireWriter.cpp \
    ./CommentWriter/CommentWriter.cpp \
    ./common/WriterGroup.cpp \
    ../../editors/ComponentEditor/common/ComponentParameterFinder.cpp \
    ../../editors/ComponentEditor/common/ExpressionFormatter.cpp
RESOURCES += VerilogGenerator.qrc
