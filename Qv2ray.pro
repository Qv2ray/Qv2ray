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

VERSION = 1.999.999.999
DEFINES += QV_MAJOR_VERSION=\"\\\"$${VERSION}\\\"\"

SOURCES += \
        src/main.cpp \
        src/QvConfigUpgrade.cpp \
        src/QvCoreConfigOperations_Convertion.cpp \
        src/QvCoreConfigOperations_Generation.cpp \
        src/QvCoreConfigOperations_Verification.cpp \
        src/QvCoreInteractions.cpp \
        src/QvUtils.cpp \
        src/utils/QPingModel.cpp \
        src/utils/QvHTTPRequestHelper.cpp \
        src/utils/QvRunguard.cpp \
        src/utils/QJsonModel.cpp \
        src/ui/w_JsonEditor.cpp \
        src/ui/w_RouteEditor.cpp \
        src/ui/w_SubscribeEditor.cpp \
        src/ui/w_MainWindow.cpp \
        src/ui/w_ConnectionEditWindow.cpp \
        src/ui/w_ImportConfig.cpp \
        src/ui/w_PrefrencesWindow.cpp \
        libs/gen/v2ray_api_commands.pb.cc \
        libs/gen/v2ray_api_commands.grpc.pb.cc

INCLUDEPATH += \
        /usr/local/include/ \
        3rdparty/ \
        src/ \
        src/ui/ \
        src/utils/ \
        libs/gen/

HEADERS += \
        src/Qv2rayBase.h \
        src/QvCoreConfigObjects.h \
        src/QvCoreConfigOperations.h \
        src/QvCoreInteractions.h \
        src/QvUtils.h \
        src/utils/QJsonObjectInsertMacros.h \
        src/utils/QPingModel.h \
        src/utils/QvHTTPRequestHelper.h \
        src/utils/QvRunguard.h \
        src/utils/QvTinyLog.h \
        src/utils/QJsonModel.h \
        src/ui/w_JsonEditor.h \
        src/ui/w_ConnectionEditWindow.h \
        src/ui/w_ImportConfig.h \
        src/ui/w_MainWindow.h \
        src/ui/w_PrefrencesWindow.h \
        src/ui/w_RouteEditor.h \
        src/ui/w_SubscribeEditor.h \
        libs/gen/v2ray_api_commands.pb.h \
        libs/gen/v2ray_api_commands.grpc.pb.h

FORMS += \
        src/ui/w_ConnectionEditWindow.ui \
        src/ui/w_ImportConfig.ui \
        src/ui/w_JsonEditor.ui \
        src/ui/w_MainWindow.ui \
        src/ui/w_PrefrencesWindow.ui \
        src/ui/w_RouteEditor.ui \
        src/ui/w_SubscribeEditor.ui

RESOURCES += \
        resources.qrc

# ------------------------------------------ Begin to detect language files.

QM_FILES_RESOURCE_PREFIX = "translations"

for(var, $$list($$files("*.ts", true))) {
    LOCALE_FILENAME = $$basename(var)

    !equals(LOCALE_FILENAME, "en-US.ts") {
        # ONLY USED IN LRELEASE CONTEXT - en-US is not EXTRA...
        EXTRA_TRANSLATIONS += translations/$$LOCALE_FILENAME
    }
}


TRANSLATIONS += \
        translations/en-US.ts

message("CORE  Translations:" $$TRANSLATIONS)
message("EXTRA Translations:" $$EXTRA_TRANSLATIONS)

RC_ICONS += ./icons/Qv2ray.ico
ICON = ./icons/Qv2ray.icns

message(Files will be generated to: $$OUT_PWD)

# For gRPC and protobuf in linux and macOS
unix: LIBS += -lgrpc++ -lprotobuf
unix: LIBS += -L/usr/local/lib

QMAKE_CXXFLAGS += "-Wno-missing-field-initializers" "-Wno-unused-parameter"
win32 {
    # A hack for protobuf header.
    DEFINES += _WIN32_WINNT=0x600
    LIBS += -L$$PWD/libs/gRPC-win32/lib/ -llibgrpc++.dll -llibprotobuf.dll
    INCLUDEPATH += $$PWD/libs/gRPC-win32/include
    DEPENDPATH  += $$PWD/libs/gRPC-win32/include
    # Some files issue.
    QMAKE_PRE_LINK += forfiles /s /p $${replace(PWD, /, \\)}\libs\ /m "*.dll" /c \"cmd.exe /c copy @file $${replace(OUT_PWD, /, \\)}\\debug\ & copy @file $${replace(OUT_PWD, /, \\)}\\release\\\"
    PRE_TARGETDEPS += $$PWD/libs/gRPC-win32/lib/libgrpc++.dll.a $$PWD/libs/gRPC-win32/lib/libprotobuf.dll.a
}
# Installations
qnx: target.path = /tmp/$${TARGET}/bin
unix: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

desktop.files += ./icons/Qv2ray.desktop
desktop.path = /opt/$${TARGET}/share/applications/
icon.files += ./icons/Qv2ray.png
icon.path = /opt/$${TARGET}/share/icons/hicolor/256x256/apps/

INSTALLS += desktop
INSTALLS += icon
