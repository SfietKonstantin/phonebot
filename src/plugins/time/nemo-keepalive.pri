DEFINES += HARBOUR_BUILD

INCLUDEPATH += ../../3rdparty/libdsme/include/ \
    ../../3rdparty/mce-dev/include/ \
    ../../3rdparty/libiphb/src/ \
    ../../3rdparty/nemo-keepalive/lib/

HEADERS += mceiface.h \
    ../../3rdparty/libdsme/include/dsme/messages.h \
    ../../3rdparty/mce-dev/include/mce/dbus-names.h \
    ../../3rdparty/libiphb/src/iphb_internal.h \
    ../../3rdparty/libiphb/src/libiphb.h \
    ../../3rdparty/nemo-keepalive/lib/backgroundactivity.h \
    ../../3rdparty/nemo-keepalive/lib/backgroundactivity_p.h \
    ../../3rdparty/nemo-keepalive/lib/common.h \
    ../../3rdparty/nemo-keepalive/lib/displayblanking.h \
    ../../3rdparty/nemo-keepalive/lib/displayblanking_p.h \
    ../../3rdparty/nemo-keepalive/lib/heartbeat.h

SOURCES += mceiface.cpp \
    ../../3rdparty/libiphb/src/libiphb.c \
    ../../3rdparty/nemo-keepalive/lib/backgroundactivity.cpp \
    ../../3rdparty/nemo-keepalive/lib/backgroundactivity_p.cpp \
    ../../3rdparty/nemo-keepalive/lib/displayblanking.cpp \
    ../../3rdparty/nemo-keepalive/lib/displayblanking_p.cpp \
    ../../3rdparty/nemo-keepalive/lib/heartbeat.cpp

system(qdbusxml2cpp -p mceiface.h:mceiface.cpp ../../3rdparty/nemo-keepalive/lib/mceiface.xml)
