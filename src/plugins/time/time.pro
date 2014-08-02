TEMPLATE = lib
TARGET = phonebottime
QT = core dbus qml

INCLUDEPATH += ../../lib/core \
    ../../lib/meta

CONFIG += plugin static

HEADERS = timetrigger.h \
    weekdaycondition.h

SOURCES = plugin.cpp \
    timetrigger.cpp \
    weekdaycondition.cpp

include(nemo-keepalive-plugin.pri)
CONFIG(harbour) {
    include(nemo-keepalive.pri)
} else {
    CONFIG += build_pkgconfig
    PKGCONFIG += keepalive
}


