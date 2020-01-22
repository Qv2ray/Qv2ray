#-------------------------------------------------
#
# Project created by QtCreator 2019-03-28T08:45:32
#
#-------------------------------------------------

QT += core gui widgets network charts

TARGET = qv2ray
TEMPLATE = app

# Now read build number file.
_BUILD_NUMBER=$$cat(Build.Counter)
VERSION = 1.99.7.$$_BUILD_NUMBER

no_increase_build_number {
    message("Build.Counter will not be increased")
} else {
    _BUILD_NUMBER = $$num_add($$_BUILD_NUMBER, 1)
    write_file("Build.Counter", _BUILD_NUMBER)
}

isEmpty(PREFIX) {
    PREFIX=/usr/local
}

DEFINES += QT_DEPRECATED_WARNINGS QV2RAY_VERSION_STRING=\"\\\"v$${VERSION}\\\"\" QAPPLICATION_CLASS=QApplication

# Don't merge those configs with below.
CONFIG += enable_decoder_qr_code enable_encoder_qr_code qt c++11 openssl-linked

include(3rdparty/qzxing/src/QZXing-components.pri)
include(3rdparty/SingleApplication/singleapplication.pri)
include(3rdparty/QNodeEditor/QNodeEditor.pri)
include(3rdparty/x2struct/x2struct.pri)

# Main config
CONFIG += lrelease embed_translations

# Win32 support.
win32:CONFIG += win
win64:CONFIG += win

SOURCES += \
        src/components/QvComponentsHandler.cpp \
        src/components/QvCore/QvCommandLineArgs.cpp \
        src/components/QvKernelInteractions.cpp \
        src/components/QvLaunchAtLoginConfigurator.cpp \
        src/components/QvPACHandler.cpp \
        src/components/QvSystemProxyConfigurator.cpp \
        src/components/QvTCPing.cpp \
        src/main.cpp \
        src/components/QvGFWPACConverter.cpp \
        src/components/QvHTTPRequestHelper.cpp \
        src/components/QvLogHighlighter.cpp \
        src/QvCoreConfigOperations.cpp \
        src/QvConfigUpgrade.cpp \
        src/QvCoreConfigOperations_Convertion.cpp \
        src/QvCoreConfigOperations_Generation.cpp \
        src/QvUtils.cpp \
        src/ui/routeNodeModels/QvInboundNodeModel.cpp \
        src/ui/routeNodeModels/QvOutboundNodeModel.cpp \
        src/ui/routeNodeModels/QvRuleNodeModel.cpp \
        src/ui/w_MainWindow_extra.cpp \
        src/ui/w_PreferencesWindow.cpp \
        src/ui/w_RoutesEditor_extra.cpp \
        src/utils/QvHelpers.cpp \
        src/utils/QJsonModel.cpp \
        src/ui/w_ExportConfig.cpp \
        src/ui/w_InboundEditor.cpp \
        src/ui/w_OutboundEditor.cpp \
        src/ui/w_RoutesEditor.cpp \
        src/ui/w_SubscriptionEditor.cpp \
        src/ui/w_JsonEditor.cpp \
        src/ui/w_MainWindow.cpp \
        src/ui/w_ImportConfig.cpp \
        src/ui/w_ScreenShot_Core.cpp \
        src/ui/NetSpeedBar/QvNetSpeedBar.cpp

INCLUDEPATH += \
        3rdparty/ \
        src/ \
        src/components \
        src/ui/ \
        src/utils/ \
        libs/gen/

HEADERS += \
        src/Qv2rayBase.hpp \
        src/Qv2rayFeatures.hpp \
        src/QvCoreConfigObjects.hpp \
        src/QvCoreConfigOperations.hpp \
        src/QvUtils.hpp \
        src/components/QvComponentsHandler.hpp \
        src/components/QvCore/QvCommandLineArgs.hpp \
        src/components/QvHTTPRequestHelper.hpp \
        src/components/QvKernelInteractions.hpp \
        src/components/QvLaunchAtLoginConfigurator.hpp \
        src/components/QvLogHighlighter.hpp \
        src/components/QvNetSpeedPlugin.hpp \
        src/components/QvPACHandler.hpp \
        src/components/QvSystemProxyConfigurator.hpp \
        src/components/QvTCPing.hpp \
        src/ui/routeNodeModels/QvInboundNodeModel.hpp \
        src/ui/routeNodeModels/QvNodeModelsBase.hpp \
        src/ui/routeNodeModels/QvOutboundNodeModel.hpp \
        src/ui/routeNodeModels/QvRuleNodeModel.hpp \
        src/ui/w_ExportConfig.hpp \
        src/ui/w_ImportConfig.hpp \
        src/ui/w_InboundEditor.hpp \
        src/ui/w_JsonEditor.hpp \
        src/ui/w_MainWindow.hpp \
        src/ui/w_OutboundEditor.hpp \
        src/ui/w_PreferencesWindow.hpp \
        src/ui/w_RoutesEditor.hpp \
        src/ui/w_SubscriptionEditor.hpp \
        src/ui/w_ScreenShot_Core.hpp \
        src/utils/QvHelpers.hpp \
        src/utils/QvTinyLog.hpp \
        src/utils/QJsonModel.hpp \
        src/utils/QJsonObjectInsertMacros.h

FORMS += \
        src/ui/w_ExportConfig.ui \
        src/ui/w_ImportConfig.ui \
        src/ui/w_InboundEditor.ui \
        src/ui/w_JsonEditor.ui \
        src/ui/w_MainWindow.ui \
        src/ui/w_OutboundEditor.ui \
        src/ui/w_PreferencesWindow.ui \
        src/ui/w_RoutesEditor.ui \
        src/ui/w_ScreenShot_Core.ui \
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


RC_ICONS += ./assets/icons/qv2ray.ico
ICON = ./assets/icons/qv2ray.icns

with_new_backend {
    !exists($$PWD/libs/libqvb/build/libqvb.h) {
        message(" ")
        message("Cannot continue: ")
        message("  --> Qv2ray is configured to use custom backend, but: ")
        message("      libs/libqvb/build/libqvb.h is missing. ")
        error("! ABORTING THE BUILD !")
        message(" ")
    }

    message("Qv2ray will use custom API backend.")
    message("  --> Adding libqvb header.")
    HEADERS += libs/libqvb/build/libqvb.h

    # ==-- OS Specific configurations for libqvb --==
    win {
        message("  --> Linking libqvb static library, for Windows platform.")
        LIBS += -L$$PWD/libs/ -lqvb-win64
    }
    unix:!macx {
        message("  --> Linking libqvb static library, for Linux platform.")
        LIBS += -L$$PWD/libs/ -lqvb-linux64
    }
    macx {
        message("  --> Linking libqvb static library, for macOS platform.")
        LIBS += -L$$PWD/libs/ -lqvb-darwin
    }
} else {
    DEFINES += WITH_LIB_GRPCPP
    message("Qv2ray will use libgRPC as API backend")

    # ------------------------------------------ Begin checking gRPC and protobuf headers.
    !exists($$PWD/libs/gen/v2ray_api_commands.grpc.pb.h) || !exists($$PWD/libs/gen/v2ray_api_commands.grpc.pb.cc) || !exists($$PWD/libs/gen/v2ray_api_commands.pb.h) || !exists($$PWD/libs/gen/v2ray_api_commands.pb.cc) {
        message(" ")
        message("-----------------------------------------------")
        message("Cannot continue: ")
        message("  --> Qv2ray is not properly configured yet: ")
        message("      gRPC and protobuf headers for v2ray API is missing.")
        message("  --> Please run gen_grpc.sh gen_grpc.bat or deps_macOS.sh located in tools/")
        message("  --> Or consider reading the build wiki: https://github.com/lhy0403/Qv2ray/wiki/Manually-Build-Qv2ray")
        message("-----------------------------------------------")
        message(" ")
        warning("IF YOU THINK IT'S A MISTAKE, PLEASE OPEN AN ISSUE")
        error("! ABORTING THE BUILD !")
        message(" ")
    }

    SOURCES += libs/gen/v2ray_api_commands.pb.cc \
               libs/gen/v2ray_api_commands.grpc.pb.cc

    HEADERS += libs/gen/v2ray_api_commands.pb.h \
               libs/gen/v2ray_api_commands.grpc.pb.h

    # ==-- OS Specific configurations for libgRPC and libprotobuf --==
    win {
        # A hack for protobuf header.
        message("  --> Applying a hack for protobuf header")
        DEFINES += _WIN32_WINNT=0x600

        message("  --> Linking against gRPC and protobuf library.")
        DEPENDPATH  += $$PWD/libs/gRPC-win32/include
        INCLUDEPATH += $$PWD/libs/gRPC-win32/include
        LIBS += -L$$PWD/libs/gRPC-win32/lib/ \
                -llibprotobuf.dll \
                -llibgrpc++.dll
    }
    unix {
        # For gRPC and protobuf in linux and macOS
        message("  --> Linking against gRPC and protobuf library.")
        LIBS += -L/usr/local/lib -lgrpc++ -lprotobuf -lgrpc
    }
    macx {
        message("  --> Linking libgpr and libupb.")
        LIBS += -lgpr -lupb
    }
}

message(" ")
# ------------------------------------------ Begin to detect language files.
message("Looking for language support.")
QM_FILES_RESOURCE_PREFIX = "translations"
for(var, $$list($$files("translations/*.ts", true))) {
    LOCALE_FILENAME = $$basename(var)
    message("  --> Found:" $$LOCALE_FILENAME)
    !equals(LOCALE_FILENAME, "en-US.ts") {
        # ONLY USED IN LRELEASE CONTEXT
        # en-US is not EXTRA...
        EXTRA_TRANSLATIONS += translations/$$LOCALE_FILENAME
    }
}
message("Qv2ray will build with" $${replace(EXTRA_TRANSLATIONS, "translations/", "")})
TRANSLATIONS += translations/en-US.ts

message(" ")
QMAKE_CXXFLAGS += -Wno-missing-field-initializers -Wno-unused-parameter -Wno-unused-variable

message("Adding QHttpServer Support")
message("  --> Adding qhttpserver")
HEADERS += \
    $$PWD/3rdparty/qhttpserver/src/qhttpconnection.h \
    $$PWD/3rdparty/qhttpserver/src/qhttprequest.h \
    $$PWD/3rdparty/qhttpserver/src/qhttpresponse.h \
    $$PWD/3rdparty/qhttpserver/src/qhttpserver.h \
    $$PWD/3rdparty/qhttpserver/src/qhttpserverapi.h \
    $$PWD/3rdparty/qhttpserver/src/qhttpserverfwd.h
SOURCES += \
    $$PWD/3rdparty/qhttpserver/src/qhttpconnection.cpp \
    $$PWD/3rdparty/qhttpserver/src/qhttprequest.cpp \
    $$PWD/3rdparty/qhttpserver/src/qhttpresponse.cpp \
    $$PWD/3rdparty/qhttpserver/src/qhttpserver.cpp

INCLUDEPATH += 3rdparty/qhttpserver/src/

message("  --> Adding http parser")
HEADERS += 3rdparty/qhttpserver/http-parser/http_parser.h
SOURCES += 3rdparty/qhttpserver/http-parser/http_parser.c
INCLUDEPATH += 3rdparty/qhttpserver/http-parser/

message(" ")
win {
    message("Configuring for win32 environment")
    DEFINES += QHTTPSERVER_EXPORT
    message("  --> Setting up target descriptions")
    QMAKE_TARGET_DESCRIPTION = "Qv2ray, a cross-platform v2ray GUI client."
    QMAKE_TARGET_PRODUCT = "Qv2ray"

    message("  --> Adding Taskbar Toolbox CPP files.")
    SOURCES += src/ui/NetSpeedBar/QvNetSpeedBar_win.cpp
    
    message("  --> Linking against winHTTP and winSock2.")
    LIBS += -lwinhttp -lwininet -lws2_32
}

macx {
    # For Linux and macOS
    message("Configuring for macOS specific environment")
    LIBS += -framework Carbon -framework Cocoa
    
    with_new_backend {
        LIBS += -framework Security
    }
}

# Reuse unix for macx as well
unix {
    # For Linux and macOS
    message("Configuring for unix-like environment")
    # macOS homebrew include path
    message("  --> Adding local include folder to search path")
    INCLUDEPATH += /usr/local/include/

    message("  --> Adding Plasma Toolbox CPP files.")
    SOURCES += $$PWD/src/ui/NetSpeedBar/QvNetSpeedBar_linux.cpp

    message("  --> Generating desktop dependency.")
    desktop.files += ./assets/qv2ray.desktop
    desktop.path = $$PREFIX/share/applications/

    message("  --> Generating icons dependency.")
    icon.files += ./assets/icons/qv2ray.png
    icon.path = $$PREFIX/share/icons/hicolor/256x256/apps/

    target.path = $$PREFIX/bin/
    INSTALLS += target desktop icon
}

with_metainfo {
    message("  --> Generating metainfo dependency.")
    appdataXml.files += ./assets/qv2ray.metainfo.xml
    appdataXml.path = $$PREFIX/share/metainfo/
    INSTALLS += appdataXml
}

message(" ")
message("Done configuring Qv2ray project. Build output will be at:" $$OUT_PWD)
message("Type `make` or `mingw32-make` to start building Qv2ray")
