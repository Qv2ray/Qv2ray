#-------------------------------------------------
#
# Project created by QtCreator 2019-03-28T08:45:32
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = Qv2ray
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
        src/HUtils.cpp \
        src/w_MainWindow.cpp \
        src/w_ConnectionEditWindow.cpp \
        src/w_ImportConfig.cpp \
        src/w_PrefrencesWindow.cpp \
        src/main.cpp \
        src/vinteract.cpp \
        src/runguard.cpp

HEADERS += \
        src/HConfigObjects.hpp \
        src/HUtils.hpp \
        src/V2ConfigObjects.hpp \
        src/runguard.hpp \
        src/vinteract.hpp \
        src/w_MainWindow.h \
        src/w_ConnectionEditWindow.h \
        src/w_ImportConfig.h \
        src/w_PrefrencesWindow.h

FORMS += \
        src/w_MainWindow.ui \
        src/w_ConnectionEditWindow.ui \
        src/w_ImportConfig.ui \
        src/w_PrefrencesWindow.ui

RESOURCES += \
        resources.qrc


TRANSLATIONS += \
        translations/zh-CN.ts \
        translations/en-US.ts

RC_ICONS += ./icons/Qv2ray.ico

INCLUDEPATH += \
        3rdparty/\
        3rdparty/jsoncons/include


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
