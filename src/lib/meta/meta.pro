TEMPLATE = lib
TARGET = phonebotmeta

QT = core qml qml-private

CONFIG += staticlib

# From QtCreator
include(meta.pri)

HEADERS += \
    phonebotmeta.h \
    abstractmetadata.h \
    metacomponent.h \
    metaproperty.h \
    qmldocument.h

SOURCES += \
    abstractmetadata.cpp \
    metacomponent.cpp \
    metaproperty.cpp \
    qmldocument.cpp
