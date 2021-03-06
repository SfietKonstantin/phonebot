TEMPLATE = app
TARGET = phonebotd

QT = core dbus qml

include(../../config.pri)
include(../libs.pri)

INCLUDEPATH += ../../lib/core \
    ../../lib/daemon

SOURCES = \
    main.cpp

!CONFIG(harbour) {
    target.path = /usr/bin
    INSTALLS += target
}
