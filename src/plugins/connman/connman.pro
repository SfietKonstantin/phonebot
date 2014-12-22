TEMPLATE = lib
TARGET = phonebotconnman
QT = core qml

INCLUDEPATH += ../../lib/core \
    ../../lib/meta \
    ../../lib/nemomw

CONFIG += plugin static

HEADERS += dataswitchaction.h

SOURCES = plugin.cpp \
    dataswitchaction.cpp

include(../../3rdparty/libnemomw/connman/connman-include.pri)




