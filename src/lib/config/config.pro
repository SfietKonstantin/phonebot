TEMPLATE = lib
TARGET = phonebotconfig

system(qdbusxml2cpp ../daemon/dbus/org.SfietKonstantin.phonebot.xml -p proxy)

QT = core qml

CONFIG += staticlib

include(../../config.pri)

INCLUDEPATH += ../../lib/core \
    ../../lib/meta
LIBS += -L../../lib/meta -lphonebotmeta \
    -L../../lib/core -lphonebot

HEADERS = phonebothelper.h \
    proxy.h \
    ruledefinition.h \
    ruledefinitionactionmodel.h \
    ruledefinitionactionmodel_p.h \
    rulecomponentmodel_p.h \
    rulecomponentmodel.h \
    rulecomponentsmodel.h \
    rulecomponentsmodel_p.h \
    rulesmodel.h

SOURCES = phonebothelper.cpp \
    proxy.cpp \
    ruledefinition.cpp \
    ruledefinitionactionmodel.cpp \
    rulecomponentmodel.cpp \
    rulecomponentsmodel.cpp \
    rulesmodel.cpp
