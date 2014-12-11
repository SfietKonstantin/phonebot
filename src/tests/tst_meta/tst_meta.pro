TEMPLATE = app
TARGET = tst_meta

QT = core dbus qml testlib

include(../../lib/nemomw/nemomw-deps.pri)

INCLUDEPATH += ../../lib/core \
    ../../lib/meta \
    ../../plugins/profile \
    ../../plugins/time
LIBS += -L../../plugins/profile -lphonebotprofile \
    -L../../plugins/time -lphonebottime \
    -L../../lib/nemomw -lnemomw \
    -L../../lib/core -lphonebot \
    -L../../lib/meta -lphonebotmeta

SOURCES += tst_meta.cpp
