TEMPLATE = app
TARGET = tst_debugplugin

QT = core dbus qml testlib

INCLUDEPATH += ../../lib \
    ../../plugins/debug
LIBS += -L../../plugins/debug -lphonebotdebug \
    -L../../lib -lphonebot

SOURCES += tst_debugplugin.cpp

RESOURCES += res.qrc

OTHER_FILES += \
    debugrule.qml
