TEMPLATE = app
TARGET = tst_parser

QT = core qml testlib

include(../../config.pri)

INCLUDEPATH += ../../lib/meta
LIBS +=     -L../../lib/meta -lphonebotmeta

SOURCES += tst_parser.cpp

RESOURCES += res.qrc

OTHER_FILES += simpletest.qml \
    Test.qml

