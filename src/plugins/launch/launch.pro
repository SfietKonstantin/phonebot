TEMPLATE = lib
TARGET = phonebotlaunch
QT = core gui qml

INCLUDEPATH += ../../lib/core \
    ../../lib/meta \
    ../../lib/nemomw

CONFIG += plugin static

HEADERS = \
    openurlaction.h \
    launchapplicationaction.h

SOURCES = plugin.cpp \
    openurlaction.cpp \
    launchapplicationaction.cpp

include(../../3rdparty/libnemomw/mlite/mlite-include.pri)
