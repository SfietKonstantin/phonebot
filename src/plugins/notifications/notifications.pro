TEMPLATE = lib
TARGET = phonebotnotifications
QT = core dbus

INCLUDEPATH += ../../lib/core \
    ../../lib/meta \
    ../../lib/nemomw

CONFIG += plugin static

HEADERS = notificationaction.h

SOURCES = plugin.cpp \
    notificationaction.cpp

include(../../3rdparty/libnemomw/notifications/notifications-include.pri)
