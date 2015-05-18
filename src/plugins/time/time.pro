TEMPLATE = lib
TARGET = phonebottime
QT = core qml

INCLUDEPATH += ../../lib/core \
    ../../lib/meta \
    ../../lib/nemomw

CONFIG += c++11
CONFIG += plugin static

HEADERS = timetrigger.h \
    weekdaycondition.h

SOURCES = plugin.cpp \
    timetrigger.cpp \
    weekdaycondition.cpp

include(../../3rdparty/libnemomw/keepalive/keepalive-include.pri)



