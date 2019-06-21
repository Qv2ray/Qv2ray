#-------------------------------------------------
#
# Project created by QtCreator 2019-03-28T08:45:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Hv2ray
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

VPATH += ./src

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        confedit.cpp \
        importconf.cpp \
        inbound_settings.cpp \
        vinteract.cpp \
        import_vmess.cpp \
        utils.cpp \
        runguard.cpp

HEADERS += \
        mainwindow.h \
        confedit.h \
        importconf.h \
        inbound_settings.h \
        vinteract.h \
        import_vmess.h \
        utils.h \
        runguard.h

FORMS += \
        mainwindow.ui \
        confedit.ui \
        importconf.ui \
        inbound_settings.ui \
        import_vmess.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = Himeki.ico

WITH_PYTHON = no

unix: exists( "/usr/include/python3.7m/Python.h" ) {
    equals(WITH_PYTHON, "no") {
        message("Will build with python lib version 3.7.")
        INCLUDEPATH += /usr/include/python3.7m/
        LIBS += -lpython3.7m
        WITH_PYTHON = yes
    }
}

unix: exists( "/usr/include/python3.6m/Python.h" ) {
    equals(WITH_PYTHON, "no") {
        message("Will build with python lib version 3.6.")
        INCLUDEPATH += /usr/include/python3.6m/
        LIBS += -lpython3.6m
        WITH_PYTHON = yes
    }
}

unix: exists( "/usr/include/python3.5m/Python.h" ) {
    equals(WITH_PYTHON, "no") {
        message("Will build with python lib version 3.5.")
        INCLUDEPATH += /usr/include/python3.5m/
        LIBS += -lpython3.5m
        WITH_PYTHON = yes
    }
}

unix: equals(WITH_PYTHON, "no") {
    error("No python libs found, did you install python3 dev package?")
}

win32: LIBS += -L$$PWD/python37/libs/ -lpython37_mingw
win32: INCLUDEPATH += $$PWD/python37/include
