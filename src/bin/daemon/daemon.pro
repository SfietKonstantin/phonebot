TEMPLATE = app
TARGET = phonebotd

QT = core dbus qml

!CONFIG(harbour) {
    QMAKE_LFLAGS += -lprofile-qt5
    CONFIG += link_pkgconfig
    PKGCONFIG += keepalive
}

INCLUDEPATH += ../../lib/core \
    ../../lib/daemon

LIBS += -L../../plugins/debug -lphonebotdebug \
    -L../../plugins/profile -lphonebotprofile \
    -L../../plugins/time -lphonebottime \
    -L../../lib/daemon -lphonebotdaemon \
    -L../../lib/meta -lphonebotmeta \
    -L../../lib/core -lphonebot

SOURCES = \
    main.cpp

!CONFIG(harbour) {
    target.path = /usr/bin
    INSTALLS += target
}
