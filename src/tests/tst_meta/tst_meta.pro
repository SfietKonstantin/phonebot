TEMPLATE = app
TARGET = tst_meta

QT = core dbus qml testlib

!CONFIG(harbour) {
    QMAKE_LFLAGS += -lprofile-qt5
    CONFIG += link_pkgconfig
    PKGCONFIG += keepalive
}

INCLUDEPATH += ../../lib/core \
    ../../lib/meta \
    ../../plugins/profile \
    ../../plugins/time
LIBS += -L../../plugins/profile -lphonebotprofile \
    -L../../plugins/time -lphonebottime \
    -L../../lib/core -lphonebot \
    -L../../lib/meta -lphonebotmeta

SOURCES += tst_meta.cpp
