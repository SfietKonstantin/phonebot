DEFINES += HARBOUR_BUILD

INCLUDEPATH += ../../../libdsme/include/ \
    ../../../mce-dev/include/ \
    ../../../libiphb/src/ \
    ../../../nemo-keepalive/lib/

HEADERS += mceiface.h \
    ../../../libdsme/include/dsme/messages.h \
    ../../../mce-dev/include/mce/dbus-names.h \
    ../../../libiphb/src/iphb_internal.h \
    ../../../libiphb/src/libiphb.h \
    ../../../nemo-keepalive/lib/backgroundactivity.h \
    ../../../nemo-keepalive/lib/backgroundactivity_p.h \
    ../../../nemo-keepalive/lib/common.h \
    ../../../nemo-keepalive/lib/displayblanking.h \
    ../../../nemo-keepalive/lib/displayblanking_p.h \
    ../../../nemo-keepalive/lib/heartbeat.h

SOURCES += mceiface.cpp \
    ../../../libiphb/src/libiphb.c \
    ../../../nemo-keepalive/lib/backgroundactivity.cpp \
    ../../../nemo-keepalive/lib/backgroundactivity_p.cpp \
    ../../../nemo-keepalive/lib/displayblanking.cpp \
    ../../../nemo-keepalive/lib/displayblanking_p.cpp \
    ../../../nemo-keepalive/lib/heartbeat.cpp

system(qdbusxml2cpp -p mceiface.h:mceiface.cpp ../../../nemo-keepalive/lib/mceiface.xml)
