TEMPLATE = lib
TARGET = phonebotdebug

system(qdbusxml2cpp dbus/org.SfietKonstantin.phonebotdebug.xml -i debugtrigger.h -a adaptor)

INCLUDEPATH += ../../lib

CONFIG += plugin static

HEADERS = debugtrigger.h \
    adaptor.h \
    loggeraction.h

SOURCES = plugin.cpp \
    debugtrigger.cpp \
    adaptor.cpp \
    loggeraction.cpp
