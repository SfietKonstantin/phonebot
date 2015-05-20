TEMPLATE = app
TARGET = tst_rule

QT = core qml testlib

include(../../config.pri)

INCLUDEPATH += ../../lib/core
LIBS +=     -L../../lib/core -lphonebot

SOURCES += tst_rule.cpp

RESOURCES += res.qrc

OTHER_FILES += \
    simplerule.qml \
    SimpleJsCondition.qml \
    SimpleJsAction.qml \
    simpleactionrule.qml \
    mapperrule.qml

