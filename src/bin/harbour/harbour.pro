TEMPLATE = app
TARGET = harbour-phonebot

QT = core gui qml quick dbus

!CONFIG(desktop) {
    CONFIG += sailfishapp
} else {
    DEFINES += DESKTOP
    RESOURCES += res.qrc
}

SOURCES += main.cpp

OTHER_FILES += qml/main.qml \
    qml/Cover.qml \
    qml/MainPage.qml


