TEMPLATE = app

QT += qml quick

SOURCES += src/main.cpp \
    src/hexgrid.cpp \
    src/hex.cpp \
    src/territory.cpp \
    src/player.cpp \
    src/diceroll.cpp

RESOURCES += qml/qml.qrc \
    pixmaps/pixmaps.qrc \
    fonts/fonts.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    src/hexgrid.h \
    src/hex.h \
    src/territory.h \
    src/player.h \
    src/diceroll.h

# This is not needed for VS 2017
# QMAKE_CXXFLAGS += -std=c++11

RC_ICONS = Hex.ico
