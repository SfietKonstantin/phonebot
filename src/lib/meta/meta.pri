INCLUDEPATH += \
    ../../3rdparty \
    ../../3rdparty/qmljs/parser

HEADERS += \
    ../../3rdparty/qmljs/qmljsconstants.h \
    ../../3rdparty/qmljs/parser/qmljsast_p.h \
    ../../3rdparty/qmljs/parser/qmljsastfwd_p.h \
    ../../3rdparty/qmljs/parser/qmljsastvisitor_p.h \
    ../../3rdparty/qmljs/parser/qmljsengine_p.h \
    ../../3rdparty/qmljs/parser/qmljsgrammar_p.h \
    ../../3rdparty/qmljs/parser/qmljslexer_p.h \
    ../../3rdparty/qmljs/parser/qmljsmemorypool_p.h \
    ../../3rdparty/qmljs/parser/qmljsparser_p.h \
    ../../3rdparty/qmljs/parser/qmljsglobal_p.h \
    ../../3rdparty/qmljs/parser/qmldirparser_p.h \
    ../../3rdparty/qmljs/parser/qmlerror.h \
    ../../3rdparty/qmljs/parser/qmljskeywords_p.h

SOURCES += \
    ../../3rdparty/qmljs/parser/qmljsast.cpp \
    ../../3rdparty/qmljs/parser/qmljsastvisitor.cpp \
    ../../3rdparty/qmljs/parser/qmljsengine_p.cpp \
    ../../3rdparty/qmljs/parser/qmljsgrammar.cpp \
    ../../3rdparty/qmljs/parser/qmljslexer.cpp \
    ../../3rdparty/qmljs/parser/qmljsparser.cpp \
    ../../3rdparty/qmljs/parser/qmldirparser.cpp \
    ../../3rdparty/qmljs/parser/qmlerror.cpp

OTHER_FILES += \
    ../../3rdparty/qmljs/parser/qmljs.g

