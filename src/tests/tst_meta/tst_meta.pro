TEMPLATE = app
TARGET = tst_meta

QT = core dbus qml testlib
CONFIG += c++11

include(../../config.pri)

INCLUDEPATH += ../../lib/core \
    ../../lib/meta
LIBS += -L../../lib/core -lphonebot \
    -L../../lib/meta -lphonebotmeta

SOURCES += tst_meta.cpp
