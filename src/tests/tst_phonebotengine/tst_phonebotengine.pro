TEMPLATE = app
TARGET = tst_phonebotengine

QT = core qml testlib

include(../../config.pri)

INCLUDEPATH += ../../lib/core
LIBS +=     -L../../lib/core -lphonebot

SOURCES += tst_phonebotengine.cpp

RESOURCES += res.qrc

OTHER_FILES += \
    dummy.qml \
    dummy_error.qml \
    no_rule.qml \
    dummyrule.qml
