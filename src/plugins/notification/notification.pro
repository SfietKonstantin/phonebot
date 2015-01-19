TEMPLATE = lib
TARGET = phonebotnotification
QT = core dbus

INCLUDEPATH += ../../lib/core \
    ../../lib/meta \
    ../../lib/nemomw

CONFIG += plugin static

HEADERS = notificationaction.h

SOURCES = plugin.cpp \
    notificationaction.cpp
