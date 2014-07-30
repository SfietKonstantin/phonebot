TEMPLATE = app
TARGET = tst_meta

QT = core dbus qml testlib

INCLUDEPATH += ../../lib/core \
    ../../lib/meta \
    ../../plugins/profile \
    ../../plugins/time
LIBS += -L../../plugins/profile -lphonebotprofile \
    -L../../plugins/time -lphonebottime \
    -L../../lib/core -lphonebot \
    -L../../lib/meta -lphonebotmeta

SOURCES += tst_meta.cpp
