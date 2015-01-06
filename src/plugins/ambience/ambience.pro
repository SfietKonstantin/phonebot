TEMPLATE = lib
TARGET = phonebotambience
QT = core dbus

INCLUDEPATH += ../../lib/core \
    ../../lib/meta \
    ../../lib/nemomw

CONFIG += plugin static

HEADERS = ambienceaction.h

SOURCES = plugin.cpp \
    ambienceaction.cpp
