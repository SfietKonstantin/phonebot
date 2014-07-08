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
} else {
    QMAKE_CXXFLAGS += -I/usr/include/profile-qt5
    QMAKE_LFLAGS += -lprofile-qt5
}

