TEMPLATE = app
TARGET = tst_phonebotengine

QT = core qml testlib

INCLUDEPATH += ../../lib/core
LIBS +=     -L../../lib/core -lphonebot

SOURCES += tst_phonebotengine.cpp

RESOURCES += res.qrc

OTHER_FILES += \
    dummy.qml \
    dummy_error.qml \
    no_rule.qml \
    simplerule.qml

