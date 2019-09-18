#-------------------------------------------------
#
# Project created by QtCreator 2019-03-28T08:45:32
#
#-------------------------------------------------

QT += core gui widgets network

TARGET = Qv2ray
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11 openssl openssl-linked lrelease embed_translations

win32: QMAKE_TARGET_DESCRIPTION = "Qv2ray, a cross-platform v2ray GUI client."
win32: QMAKE_TARGET_PRODUCT = "Qv2ray"

VERSION = 2.0
DEFINES += QV_MAJOR_VERSION=\"\\\"$${VERSION}\\\"\"

SOURCES += \
        src/QvConfigUpgrade.cpp \
        src/QvCoreConfigOperations_Convertion.cpp \
        src/QvCoreConfigOperations_Generation.cpp \
        src/QvCoreConfigOperations_Verification.cpp \
        src/QvCoreInteractions.cpp \
        src/QvHTTPRequestHelper.cpp \
        src/QvRunguard.cpp \
        src/QvUtils.cpp \
        src/w_MainWindow.cpp \
        src/w_ConnectionEditWindow.cpp \
        src/w_ImportConfig.cpp \
        src/w_PrefrencesWindow.cpp \
        src/main.cpp \
        src/w_RouteEditor.cpp \
        src/w_SubscribeEditor.cpp

HEADERS += \
        src/QJsonObjectInsertMacros.h \
        src/Qv2rayBase.h \
        src/QvCoreConfigObjects.h \
        src/QvCoreConfigOperations.h \
        src/QvCoreInteractions.h \
        src/QvHTTPRequestHelper.h \
        src/QvRunguard.h \
        src/QvTinyLog.h \
        src/QvUtils.h \
        src/w_MainWindow.h \
        src/w_ConnectionEditWindow.h \
        src/w_ImportConfig.h \
        src/w_PrefrencesWindow.h \
        src/w_RouteEditor.h \
        src/w_SubscribeEditor.h

FORMS += \
        src/w_MainWindow.ui \
        src/w_ConnectionEditWindow.ui \
        src/w_ImportConfig.ui \
        src/w_PrefrencesWindow.ui \
        src/w_RouteEditor.ui \
        src/w_SubscribeEditor.ui

RESOURCES += \
        resources.qrc

# ------------------------------------------ Begin to detect language files.

QM_FILES_RESOURCE_PREFIX = "translations"

message("Detecting Translation files.....")

for(var, $$list($$files("*.ts", true))) {
    LOCALE_FILENAME = $$basename(var)
    message(Found: $$LOCALE_FILENAME)

    !equals(LOCALE_FILENAME, "en-US.ts") {
        # ONLY USED IN LRELEASE CONTEXT - en-US is not EXTRA...
        EXTRA_TRANSLATIONS += translations/$$LOCALE_FILENAME
    }
}


TRANSLATIONS += \
        translations/en-US.ts

message("Translations:" $$TRANSLATIONS)
message("EXTRA Translations:" $$EXTRA_TRANSLATIONS)


RC_ICONS += ./icons/Qv2ray.ico
ICON = ./icons/Qv2ray.icns

INCLUDEPATH += \
        3rdparty/

win32: QMAKE_CXXFLAGS += "-Wno-missing-field-initializers"

qnx: target.path = /tmp/$${TARGET}/bin
unix: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

desktop.files += ./icons/Qv2ray.desktop
desktop.path = /opt/$${TARGET}/share/applications/
icon.files += ./icons/Qv2ray.png
icon.path = /opt/$${TARGET}/share/icons/hicolor/256x256/apps/

INSTALLS += desktop
INSTALLS += icon
