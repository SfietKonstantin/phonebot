TEMPLATE = lib
TARGET = phonebotconnman
QT = core qml

INCLUDEPATH += ../../lib/core \
    ../../lib/meta \
    ../../lib/nemomw

CONFIG += c++11
CONFIG += plugin static

HEADERS += dataswitchaction.h \
    wlanswitchaction.h

SOURCES = plugin.cpp \
    dataswitchaction.cpp \
    wlanswitchaction.cpp

include(../../3rdparty/libnemomw/connman/connman-include.pri)




