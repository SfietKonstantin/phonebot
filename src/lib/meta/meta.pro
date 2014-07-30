TEMPLATE = lib
TARGET = phonebotmeta

QT = core qml qml-private

CONFIG += staticlib

INCLUDEPATH += ../core/

# From QtCreator
include(meta.pri)

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
