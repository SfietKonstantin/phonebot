TEMPLATE = app
TARGET = tst_action

QT = core qml testlib

INCLUDEPATH += ../../src/lib
LIBS += -L../../src/lib -lphonebot

SOURCES += tst_action.cpp

RESOURCES += res.qrc

OTHER_FILES += \
    SimpleAction.qml \
    simpleactionrule.qml
