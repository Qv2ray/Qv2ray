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

SOURCES += \
        ./src/w_MainWindow.cpp \
        ./src/w_ConnectionEditWindow.cpp \
        ./src/w_ImportConfig.cpp \
        ./src/w_PrefrencesWindow.cpp \
        ./src/main.cpp \
        ./src/vinteract.cpp \
        ./src/utils.cpp \
        ./src/runguard.cpp

HEADERS += \
        ./src/w_MainWindow.h \
        ./src/w_ConnectionEditWindow.h \
        ./src/w_ImportConfig.h \
        ./src/w_PrefrencesWindow.h \
        ./src/constants.h \
        ./src/vinteract.h \
        ./src/utils.h \
        ./src/runguard.h

FORMS += \
        ./src/w_MainWindow.ui \
        ./src/w_ConnectionEditWindow.ui \
        ./src/w_ImportConfig.ui \
        ./src/w_PrefrencesWindow.ui

RESOURCES += \
        ./resources.qrc


TRANSLATIONS += \
        ./translations/zh-CN.ts \
        ./translations/en-US.ts

RC_ICONS += ./icons/Hv2ray.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

WITH_PYTHON = no
PYTHONVER = null

unix:!macx {
    # Python Headers check.
    exists( "/usr/include/python3.5m/Python.h" ) {
        PYTHONVER = 3.5
        WITH_PYTHON = yes
    }
    exists( "/usr/include/python3.6m/Python.h" ) {
        PYTHONVER = 3.6
        WITH_PYTHON = yes
    }
    exists( "/usr/include/python3.7m/Python.h" ) {
        PYTHONVER = 3.7
        WITH_PYTHON = yes
    }

    equals( WITH_PYTHON, "yes" ) {
        INCLUDEPATH += /usr/include/python$${PYTHONVER}m/
        LIBS += -lpython$${PYTHONVER}m
        message("Will build with python lib version $$PYTHONVER")
    }
}

macx {
    PYTHON_ROOT=/usr/local/Cellar/python
    exists( "$${PYTHON_ROOT}/3.6.5_1/" ) {
        PYTHONVER = 3.6
        PYLDPATH=$${PYTHON_ROOT}/3.6.5_1/Frameworks/Python.framework/Versions/$${PYTHONVER}
        WITH_PYTHON = yes
    }

    exists( "$$PYTHON_ROOT/3.7.3/" ) {
        PYTHONVER = 3.7
        PYLDPATH=$${PYTHON_ROOT}/3.7.3/Frameworks/Python.framework/Versions/$${PYTHONVER}
        WITH_PYTHON = yes
    }

    INCLUDEPATH += $${PYLDPATH}/include/python$${PYTHONVER}m/
    LIBS += -L$${PYLDPATH}/lib/python$${PYTHONVER}/config-$${PYTHONVER}m-darwin/ -lpython$${PYTHONVER}m
    message("Will build with python lib version $$PYTHONVER")
}

win32 {
    exists( "$$PWD/python37/libs/libpython37_mingw.a" ) {
        LIBS += -L$$PWD/python37/libs/ -lpython37_mingw
        INCLUDEPATH += $$PWD/python37/include
        WITH_PYTHON = yes
    }
}

equals(WITH_PYTHON, "no") {
    error("No Python3 libs found, did you install dev packages such as python3-dev ?")
}
