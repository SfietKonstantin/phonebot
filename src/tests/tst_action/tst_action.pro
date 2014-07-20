TEMPLATE = app
TARGET = tst_action

QT = core qml testlib

INCLUDEPATH += ../../lib/core
LIBS += -L../../lib/core -lphonebot

SOURCES += tst_action.cpp

RESOURCES += res.qrc

OTHER_FILES += \
    SimpleAction.qml \
    simpleactionrule.qml
