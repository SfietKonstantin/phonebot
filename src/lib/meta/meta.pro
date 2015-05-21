# Independant plugin used to parse
# rule QML format and get metadata
# from components

TEMPLATE = lib
TARGET = phonebotmeta

QT = core qml qml-private

CONFIG += staticlib

include(../../config.pri)

INCLUDEPATH += ../core/

HEADERS += \
    phonebotmeta.h \
    abstractmetadata.h \
    metaproperty.h \
    qmldocument.h \
    metatypecache.h \
    choicemodel.h \
    choicemodel_p.h

SOURCES += \
    abstractmetadata.cpp \
    metaproperty.cpp \
    qmldocument.cpp \
    metatypecache.cpp \
    choicemodel.cpp
