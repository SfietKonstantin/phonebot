TEMPLATE = lib
TARGET = phonebotdebug
QT = core dbus qml

system(qdbusxml2cpp dbus/org.SfietKonstantin.phonebotdebug.xml -i debugtrigger.h -a adaptor)

INCLUDEPATH += ../../lib/core \
    ../../lib/meta

CONFIG += c++11
CONFIG += plugin static

include(../../config.pri)

HEADERS = debugtrigger.h \
    adaptor.h \
    loggeraction.h

SOURCES = plugin.cpp \
    debugtrigger.cpp \
    adaptor.cpp \
    loggeraction.cpp
