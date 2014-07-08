TEMPLATE = app
TARGET = tst_debugplugin

QT = core dbus qml testlib

INCLUDEPATH += ../../src/lib \
    ../../src/plugins/debug
LIBS += -L../../src/lib -lphonebot -L../../src/plugins/debug -lphonebotdebug

SOURCES += tst_debugplugin.cpp

RESOURCES += res.qrc

OTHER_FILES += \
    debugrule.qml
