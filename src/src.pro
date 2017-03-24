#-------------------------------------------------
#
# Project created by QtCreator 2013-09-10T16:42:53
#
#-------------------------------------------------

CONFIG(debug, debug|release) {
     DEFAULT_SUBDIR = debug
}
else: {
     DEFAULT_SUBDIR = release
}

QT       += network

QT       -= gui

TARGET = mixpanel-qt
TEMPLATE = lib

DESTDIR = $$PWD/../lib/$$DEFAULT_SUBDIR

DEFINES += MIXPANELQT_LIBRARY

SOURCES += mixpanel.cpp

HEADERS += mixpanel.h\
        mixpanel-qt_global.h

INCLUDEPATH += C:/Projects/qjson/src

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5
    INCLUDEPATH += /opt/local/include/
    LIBS += -L/opt/local/lib/
}

unix|win32: LIBS += -L"C:\Projects\qjson\build-vs\src\Debug" -lqjsond
