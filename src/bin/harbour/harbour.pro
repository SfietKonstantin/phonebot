TEMPLATE = app
TARGET = harbour-phonebot

QT = core gui qml quick dbus

!CONFIG(desktop) {
    CONFIG += sailfishapp
} else {
    DEFINES += DESKTOP
    RESOURCES += res.qrc
}

INCLUDEPATH += ../../lib/core \
    ../../lib/meta
LIBS += -L../../plugins/debug -lphonebotdebug \
    -L../../plugins/profile -lphonebotprofile \
    -L../../plugins/time -lphonebottime \
    -L../../lib/meta -lphonebotmeta \
    -L../../lib/core -lphonebot

HEADERS += \
    rulesmodel.h \
    ruledefinition.h \
    rulepropertiesvaluesmapper.h \
    componentdefinition.h \
    rulecomponentmodel.h \
    rulecomponentmodel_p.h \
    rulecomponentsmodel.h \
    rulecomponentsmodel_p.h \
    phonebothelper.h \
    ruledefinitionactionmodel.h \
    ruledefinitionactionmodel_p.h

SOURCES += \
    main.cpp \
    rulesmodel.cpp \
    ruledefinition.cpp \
    rulepropertiesvaluesmapper.cpp \
    componentdefinition.cpp \
    rulecomponentmodel.cpp \
    rulecomponentsmodel.cpp \
    phonebothelper.cpp \
    ruledefinitionactionmodel.cpp

OTHER_FILES += qml/main.qml \
    qml/Cover.qml \
    qml/MainPage.qml \
    qml/RuleDialog.qml \
    qml/RuleButton.qml \
    qml/ComponentConfigDialog.qml \
    qml/ComponentsDialog.qml \
    qml/typesolver.js \
    qml/TimePicker.qml \
    qml/TypeField.qml \
    qml/DialogButton.qml \
    qml/ComboBox.qml



