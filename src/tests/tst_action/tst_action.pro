TEMPLATE = app
TARGET = tst_action

QT = core qml testlib

INCLUDEPATH += ../../lib
LIBS += -L../../lib -lphonebot

SOURCES += tst_action.cpp

RESOURCES += res.qrc

OTHER_FILES += \
    SimpleAction.qml \
    simpleactionrule.qml
