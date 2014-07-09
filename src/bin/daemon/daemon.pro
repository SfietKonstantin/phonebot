TEMPLATE = app
TARGET = phonebotd

QT = core dbus qml

!CONFIG(harbour) {
    QMAKE_LFLAGS += -lprofile-qt5
    CONFIG += link_pkgconfig
    PKGCONFIG += keepalive
}

INCLUDEPATH += ../../lib

LIBS += -L../../plugins/debug -lphonebotdebug \
    -L../../plugins/profile -lphonebotprofile \
    -L../../plugins/time -lphonebottime \
    -L../../lib -lphonebot

HEADERS = \
    enginemanager.h

SOURCES = \
    main.cpp \
    enginemanager.cpp

!CONFIG(harbour) {
    target.path = /usr/bin
    INSTALLS += target
}
