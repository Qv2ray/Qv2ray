#-------------------------------------------------
#
# Project created by QtCreator 2019-03-28T08:45:32
#
#-------------------------------------------------

QT += core gui widgets network

TARGET = Qv2ray
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11 openssl-linked lrelease embed_translations

VERSION = 1.99.99.4
DEFINES += QV_MAJOR_VERSION=\"\\\"$${VERSION}\\\"\"

SOURCES += \
        src/main.cpp \
        src/QvConfigUpgrade.cpp \
        src/QvCoreConfigOperations_Convertion.cpp \
        src/QvCoreConfigOperations_Generation.cpp \
        src/QvCoreConfigOperations_Verification.cpp \
        src/QvCoreInteractions.cpp \
        src/QvUtils.cpp \
        src/ui/w_InboundEditor.cpp \
        src/ui/w_OutboundEditor.cpp \
        src/ui/w_RoutesEditor.cpp \
        src/ui/w_SubscriptionEditor.cpp \
        src/utils/QPingModel.cpp \
        src/utils/QvHTTPRequestHelper.cpp \
        src/utils/QvRunguard.cpp \
        src/utils/QJsonModel.cpp \
        src/ui/w_JsonEditor.cpp \
        src/ui/w_MainWindow.cpp \
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
        src/ui/w_InboundEditor.h \
        src/ui/w_OutboundEditor.h \
        src/ui/w_RoutesEditor.h \
        src/ui/w_SubscriptionEditor.h \
        src/utils/QJsonObjectInsertMacros.h \
        src/utils/QPingModel.h \
        src/utils/QvHTTPRequestHelper.h \
        src/utils/QvRunguard.h \
        src/utils/QvTinyLog.h \
        src/utils/QJsonModel.h \
        src/ui/w_JsonEditor.h \
        src/ui/w_ImportConfig.h \
        src/ui/w_MainWindow.h \
        src/ui/w_PrefrencesWindow.h \
        libs/gen/v2ray_api_commands.pb.h \
        libs/gen/v2ray_api_commands.grpc.pb.h

FORMS += \
        src/ui/w_ImportConfig.ui \
        src/ui/w_InboundEditor.ui \
        src/ui/w_JsonEditor.ui \
        src/ui/w_MainWindow.ui \
        src/ui/w_OutboundEditor.ui \
        src/ui/w_PrefrencesWindow.ui \
        src/ui/w_RoutesEditor.ui \
        src/ui/w_SubscriptionEditor.ui

RESOURCES += \
        resources.qrc

# Fine......
message(" ")
message("Qv2ray Version: $${VERSION}")
message("|-------------------------------------------------|")
message("| Qv2ray, A Cross Platform v2ray Qt GUI Client.   |")
message("| Licenced under GPLv3                            |")
message("|                                                 |")
message("| You may only use this program to the extent     |")
message("| permitted by local law.                         |")
message("|                                                 |")
message("| See: https://www.gnu.org/licenses/gpl-3.0.html  |")
message("|-------------------------------------------------|")
message(" ")
RC_ICONS += ./icons/Qv2ray.ico
ICON = ./icons/Qv2ray.icns

# ------------------------------------------ Begin checking gRPC and protobuf headers.
!exists(libs/gen/v2ray_api_commands.grpc.pb.h) || !exists(libs/gen/v2ray_api_commands.grpc.pb.cc) || !exists(libs/gen/v2ray_api_commands.pb.h) || !exists(libs/gen/v2ray_api_commands.pb.cc) {
    message(" ")
    message("-----------------------------------------------")
    message("Cannot continue: ")
    message("  --> Qv2ray is not properly configured yet: ")
    message("      gRPC and protobuf headers for v2ray API is missing.")
    message("  --> Please run gen_grpc.sh gen_grpc.bat or deps_macOS.sh located in tools/")
    message("  --> Or consider reading https://github.com/lhy0403/Qv2ray/blob/master/BUILDING.md")
    message("-----------------------------------------------")
    message(" ")
    warning("IF YOU THINK IT'S A MISTAKE, PLEASE OPEN AN ISSUE")
    error("! NOW THE BUILD WILL ABORT !")
    message(" ")
}

# ------------------------------------------ Begin to detect language files.
message("Looking for language support.")
QM_FILES_RESOURCE_PREFIX = "translations"
for(var, $$list($$files("translations/*.ts", true))) {
    LOCALE_FILENAME = $$basename(var)
    message("  --> Found:" $$LOCALE_FILENAME)
    !equals(LOCALE_FILENAME, "en-US.ts") {
        # ONLY USED IN LRELEASE CONTEXTen-US is not EXTRA...
        EXTRA_TRANSLATIONS += translations/$$LOCALE_FILENAME
    }
}
message("Qv2ray will build with" $${replace(EXTRA_TRANSLATIONS, "translations/", "")})
TRANSLATIONS += translations/en-US.ts

QMAKE_CXXFLAGS += "-Wno-missing-field-initializers" "-Wno-unused-parameter"
win32 {
    message("Configuring for win32 environment")
    message("  --> Setting up target descriptions")
    QMAKE_TARGET_DESCRIPTION = "Qv2ray, a cross-platform v2ray GUI client."
    QMAKE_TARGET_PRODUCT = "Qv2ray"

    # A hack for protobuf header.
    message("  --> Applying a hack for protobuf header")
    DEFINES += _WIN32_WINNT=0x600

    message("  --> Linking against gRPC and protobuf library.")
    LIBS += -L$$PWD/libs/gRPC-win32/lib/ -llibgrpc++.dll -llibprotobuf.dll

    INCLUDEPATH += $$PWD/libs/gRPC-win32/include
    DEPENDPATH  += $$PWD/libs/gRPC-win32/include
    # Some files issue.

    CONFIG(release, debug|release) {
        message("  --> Appending scripts for copying gRPC and protobuf dll to RELEASE directory.")
        QMAKE_PRE_LINK += forfiles /s /p $${replace(PWD, /, \\)}\libs\ /m "*.dll" /c \"cmd.exe /c copy @file $${replace(OUT_PWD, /, \\)}\\release\\\"
    }

    CONFIG(debug, debug|release) {
        message("  --> Appending scripts for copying gRPC and protobuf dll to DEBUG directory.")
        QMAKE_PRE_LINK += forfiles /s /p $${replace(PWD, /, \\)}\libs\ /m "*.dll" /c \"cmd.exe /c copy @file $${replace(OUT_PWD, /, \\)}\\debug\\\"
    }
    PRE_TARGETDEPS += $$PWD/libs/gRPC-win32/lib/libgrpc++.dll.a $$PWD/libs/gRPC-win32/lib/libprotobuf.dll.a
}

unix {
    # For Linux and macOS
    message("Configuring for unix (macOS and linux) environment")
    # For gRPC and protobuf in linux and macOS
    message("  --> Linking against gRPC and protobuf library.")
    LIBS += -L/usr/local/lib -lgrpc++ -lprotobuf

    message("  --> Generating desktop dependency.")
    desktop.files += ./icons/Qv2ray.desktop
    desktop.path = /opt/$${TARGET}/share/applications/

    message("  --> Generating icons dependency.")
    icon.files += ./icons/Qv2ray.png
    icon.path = /opt/$${TARGET}/share/icons/hicolor/256x256/apps/

    target.path = /opt/$${TARGET}/bin
    INSTALLS += target desktop icon
}

message("Done configuring Qv2ray project. Build output will be at:" $$OUT_PWD)
message("Type `make` or `mingw32-make` to start building Qv2ray")
