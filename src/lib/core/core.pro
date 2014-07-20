TEMPLATE = lib
TARGET = phonebot

QT = core qml

CONFIG += staticlib

HEADERS = rule.h \
    rule_p.h \
    trigger.h \
    trigger_p.h \
    action.h \
    condition.h \
    condition_p.h \
    action_p.h \
    phonebotengine_p.h \
    phonebotengine.h \
    phonebotextensioninterface.h \
    phonebotextensionplugin.h \
    jsaction.h

SOURCES = rule.cpp \
    trigger.cpp \
    action.cpp \
    condition.cpp \
    phonebotengine.cpp \
    phonebotextensionplugin.cpp \
    jsaction.cpp