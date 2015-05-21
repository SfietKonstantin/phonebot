TEMPLATE = subdirs
SUBDIRS = debug
!CONFIG(coverage):{
SUBDIRS  += \
    profile \
    time \
    connman \
    ambience \
    notifications
}
