TEMPLATE = app
TARGET = phonebotd

QT = core gui dbus qml

include(../../lib/nemomw/nemomw-deps.pri)

INCLUDEPATH += ../../lib/core \
    ../../lib/daemon

LIBS += -L../../plugins/debug -lphonebotdebug \
    -L../../plugins/profile -lphonebotprofile \
    -L../../plugins/time -lphonebottime \
    -L../../plugins/connman -lphonebotconnman \
    -L../../plugins/ambience -lphonebotambience \
    -L../../plugins/launch -lphonebotlaunch \
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
