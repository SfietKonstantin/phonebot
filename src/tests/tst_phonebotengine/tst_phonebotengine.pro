TEMPLATE = app
TARGET = tst_phonebotengine

QT = core qml testlib
CONFIG += c++11

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
