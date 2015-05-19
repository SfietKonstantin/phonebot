TEMPLATE = subdirs
SUBDIRS = debug
!CONFIG(testing):{
SUBDIRS  += \
    profile \
    time \
    connman \
    ambience \
    notifications
}
