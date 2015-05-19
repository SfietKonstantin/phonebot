include(../lib/nemomw/nemomw-deps.pri)

LIBS += -L../../plugins/debug -lphonebotdebug \
    -L../../plugins/profile -lphonebotprofile \
    -L../../plugins/time -lphonebottime \
    -L../../plugins/connman -lphonebotconnman \
    -L../../plugins/ambience -lphonebotambience \
    -L../../plugins/notifications -lphonebotnotifications \
    -L../../lib/nemomw -lnemomw \
    -L../../lib/config -lphonebotconfig \
    -L../../lib/daemon -lphonebotdaemon \
    -L../../lib/meta -lphonebotmeta \
    -L../../lib/core -lphonebot
