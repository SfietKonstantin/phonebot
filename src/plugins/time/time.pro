TEMPLATE = lib
TARGET = phonebottime
QT = core dbus qml

INCLUDEPATH += ../../lib/core

CONFIG += plugin static

HEADERS = timetrigger.h

SOURCES = plugin.cpp \
    timetrigger.cpp

include(nemo-keepalive-plugin.pri)
CONFIG(harbour) {
    include(nemo-keepalive.pri)
} else {
    CONFIG += build_pkgconfig
    PKGCONFIG += keepalive
}


