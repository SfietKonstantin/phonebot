TEMPLATE = app
TARGET = tst_debugplugin

QT = core dbus qml testlib

include(../../config.pri)

INCLUDEPATH += ../../lib/core \
    ../../plugins/debug
LIBS += -L../../plugins/debug -lphonebotdebug \
    -L../../lib/core -lphonebot

SOURCES += tst_debugplugin.cpp

RESOURCES += res.qrc

OTHER_FILES += \
    debugrule.qml
