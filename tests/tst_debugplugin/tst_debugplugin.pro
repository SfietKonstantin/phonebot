TEMPLATE = app
TARGET = tst_debugplugin

QT = core dbus qml testlib

INCLUDEPATH += ../../src/lib \
    ../../src/plugins/debug
LIBS += -L../../src/plugins/debug -lphonebotdebug \
    -L../../src/lib -lphonebot

SOURCES += tst_debugplugin.cpp

RESOURCES += res.qrc

OTHER_FILES += \
    debugrule.qml
