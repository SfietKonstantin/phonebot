TEMPLATE = lib
TARGET = phonebotdaemon

system(qdbusxml2cpp dbus/org.SfietKonstantin.phonebot.xml -i enginemanager.h -a adaptor)

QT = core qml

CONFIG += staticlib

include(../../config.pri)

INCLUDEPATH += ../../lib/core
LIBS += -L../../lib/core -lphonebot

HEADERS += \
    adaptor.h \
    enginemanager.h

SOURCES += \
    adaptor.cpp \
    enginemanager.cpp

