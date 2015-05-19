TEMPLATE = app
TARGET = phonebotd

QT = core dbus qml

include(../../config.pri)
include(../../lib/nemomw/nemomw-deps.pri)

INCLUDEPATH += ../../lib/core \
    ../../lib/daemon

LIBS += -L../../plugins/debug -lphonebotdebug \
    -L../../plugins/profile -lphonebotprofile \
    -L../../plugins/time -lphonebottime \
    -L../../plugins/connman -lphonebotconnman \
    -L../../plugins/ambience -lphonebotambience \
    -L../../plugins/notifications -lphonebotnotifications \
    -L../../lib/nemomw -lnemomw \
    -L../../lib/daemon -lphonebotdaemon \
    -L../../lib/meta -lphonebotmeta \
    -L../../lib/core -lphonebot

SOURCES = \
    main.cpp

!CONFIG(harbour) {
    target.path = /usr/bin
    INSTALLS += target
}
