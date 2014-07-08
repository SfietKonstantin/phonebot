TEMPLATE = lib
TARGET = phonebotprofile

QT = core dbus
INCLUDEPATH += ../../lib

CONFIG += plugin static

HEADERS = \
    profileaction.h

SOURCES = plugin.cpp \
    profileaction.cpp

CONFIG(harbour) {
    include(libprofile-qt.pri)
}

