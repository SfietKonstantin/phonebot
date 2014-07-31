TEMPLATE = lib
TARGET = phonebotconfig

QT = core qml

CONFIG += staticlib

INCLUDEPATH += ../../lib/core \
    ../../lib/meta
LIBS += -L../../lib/meta -lphonebotmeta \
    -L../../lib/core -lphonebot

HEADERS = phonebothelper.h \
    ruledefinition.h \
    ruledefinitionactionmodel.h \
    ruledefinitionactionmodel_p.h \
    rulecomponentmodel_p.h \
    rulecomponentmodel.h \
    rulecomponentsmodel.h \
    rulecomponentsmodel_p.h \
    rulesmodel.h

SOURCES = phonebothelper.cpp \
    ruledefinition.cpp \
    ruledefinitionactionmodel.cpp \
    rulecomponentmodel.cpp \
    rulecomponentsmodel.cpp \
    rulesmodel.cpp
