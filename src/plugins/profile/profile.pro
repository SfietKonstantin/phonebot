TEMPLATE = lib
TARGET = phonebotprofile
QT = core dbus

INCLUDEPATH += ../../lib/core \
    ../../lib/meta \
    ../../lib/nemomw

CONFIG += c++11
CONFIG += plugin static

HEADERS = profileaction.h

SOURCES = plugin.cpp \
    profileaction.cpp

include(../../3rdparty/libnemomw/profile/profile-include.pri)
