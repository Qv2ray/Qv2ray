#-------------------------------------------------
#
# Project created by QtCreator 2019-03-28T08:45:32
#
#-------------------------------------------------

QT += core gui widgets network

TARGET = qv2ray
TEMPLATE = app

# Now read build number file.
QV2RAY_VERSION=$$cat($$PWD/makespec/VERSION)
QV2RAY_BUILD_VERSION=$$cat($$PWD/makespec/BUILDVERSION)

VERSION = $${QV2RAY_VERSION}.$${QV2RAY_BUILD_VERSION}

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
message("| Project Homepage: https://github.com/Qv2ray     |")
message("| Welcome to contribute!                          |")
message("|-------------------------------------------------|")
message(" ")

# Distinguish debug and release builds.
CONFIG(release, debug|release) {
    CONFIG+=Qv2ray_release no_increase_build_number
}
CONFIG(debug, debug|release) {
    CONFIG+=Qv2ray_debug
}

# Qv2ray basic configuration
CONFIG += qt c++17 openssl-linked
include($$PWD/makespec/00-deps.pri)

# lrelease will not work when adding BEFORE 00-deps.pri
CONFIG += lrelease embed_translations
DEFINES += QT_DEPRECATED_WARNINGS QV2RAY_VERSION_STRING=\"\\\"v$${VERSION}\\\"\" QAPPLICATION_CLASS=QApplication XTOSTRUCT_QT

# Source file parser
include($$PWD/makespec/01-sourcesparser.pri)

# Main config
Qv2rayAddSource(base, _, GlobalInstances, hpp)
Qv2rayAddSource(base, _, JsonHelpers, hpp)
Qv2rayAddSource(base, _, Qv2rayBase, hpp)
Qv2rayAddSource(base, _, Qv2rayFeatures, hpp)
Qv2rayAddSource(base, _, Qv2rayLog, cpp, hpp)
Qv2rayAddSource(base, models, CoreObjectModels, hpp)
Qv2rayAddSource(base, models, QvConfigModel, hpp)
Qv2rayAddSource(base, models, QvConfigIdentifier, hpp)
Qv2rayAddSource(base, models, QvSafeType, hpp)
Qv2rayAddSource(base, models, QvRuntimeConfig, hpp)
Qv2rayAddSource(base, models, QvStartupConfig, hpp)
Qv2rayAddSource(common, _, CommandArgs, cpp, hpp)
Qv2rayAddSource(common, _, HTTPRequestHelper, cpp, hpp)
Qv2rayAddSource(common, _, LogHighlighter, cpp, hpp)
Qv2rayAddSource(common, _, QJsonModel, cpp, hpp)
Qv2rayAddSource(common, _, QvHelpers, cpp, hpp)
Qv2rayAddSource(common, _, QvTranslator, hpp)
Qv2rayAddSource(components, autolaunch, QvAutoLaunch, cpp, hpp)
Qv2rayAddSource(components, pac, QvGFWPACConverter, cpp)
Qv2rayAddSource(components, pac, QvPACHandler, cpp, hpp)
Qv2rayAddSource(components, plugins/toolbar, QvToolbar, cpp, hpp)
Qv2rayAddSource(components, plugins/toolbar, QvToolbar_linux, cpp)
Qv2rayAddSource(components, plugins/toolbar, QvToolbar_win, cpp)
Qv2rayAddSource(components, proxy, QvProxyConfigurator, cpp, hpp)
Qv2rayAddSource(components, speedchart, speedwidget, cpp, hpp)
Qv2rayAddSource(components, speedchart, speedplotview, cpp, hpp)
Qv2rayAddSource(components, geosite, QvGeositeReader, cpp, hpp)
Qv2rayAddSource(core, settings, SettingsBackend, cpp, hpp)
Qv2rayAddSource(core, settings, SettingsUpgrade, cpp)
Qv2rayAddSource(core, connection, ConnectionIO, cpp, hpp)
Qv2rayAddSource(core, connection, Generation, cpp, hpp)
Qv2rayAddSource(core, connection, Serialization, cpp, hpp)
Qv2rayAddSource(core, connection, Serialization_vmess, cpp)
Qv2rayAddSource(core, connection, Serialization_ss, cpp)
Qv2rayAddSource(core, kernel, KernelInteractions, cpp, hpp)
Qv2rayAddSource(core, kernel, APIBackend, cpp, hpp)
Qv2rayAddSource(core, tcping, QvTCPing, cpp, hpp)
# Connection Handlers
Qv2rayAddSource(core, handler, ConfigHandler, cpp, hpp)
Qv2rayAddSource(core, handler, V2rayInstanceHandler, cpp)
#
Qv2rayAddSource(core, _, CoreUtils, cpp, hpp)
Qv2rayAddSource(core, _, CoreSafeTypes, hpp)
Qv2rayAddSource(ui, editors, w_InboundEditor, cpp, hpp, ui)
Qv2rayAddSource(ui, editors, w_JsonEditor, cpp, hpp, ui)
Qv2rayAddSource(ui, editors, w_OutboundEditor, cpp, hpp, ui)
Qv2rayAddSource(ui, editors, w_RoutesEditor, cpp, hpp, ui)
Qv2rayAddSource(ui, editors, w_RoutesEditor_extra, cpp)
Qv2rayAddSource(ui, models, InboundNodeModel, cpp, hpp)
Qv2rayAddSource(ui, models, OutboundNodeModel, cpp, hpp)
Qv2rayAddSource(ui, models, RuleNodeModel, cpp, hpp)
Qv2rayAddSource(ui, models, NodeModelsBase, hpp)
Qv2rayAddSource(ui, messaging, QvMessageBus, cpp, hpp)
Qv2rayAddSource(ui, _, w_ExportConfig, cpp, hpp, ui)
Qv2rayAddSource(ui, _, w_ImportConfig, cpp, hpp, ui)
Qv2rayAddSource(ui, _, w_MainWindow, cpp, hpp, ui)
Qv2rayAddSource(ui, _, w_MainWindow_extra, cpp)
Qv2rayAddSource(ui, _, w_PreferencesWindow, cpp, hpp, ui)
Qv2rayAddSource(ui, _, w_ScreenShot_Core, cpp, hpp, ui)
Qv2rayAddSource(ui, _, w_SubscriptionManager, cpp, hpp, ui)
Qv2rayAddSource(ui, widgets, StreamSettingsWidget, cpp, hpp, ui)
Qv2rayAddSource(ui, widgets, ConnectionItemWidget, cpp, hpp, ui)
Qv2rayAddSource(ui, widgets, ConnectionInfoWidget, cpp, hpp, ui)

SOURCES += $$PWD/src/main.cpp
HEADERS +=
FORMS +=
INCLUDEPATH += $$PWD/src
RESOURCES += $$PWD/resources.qrc
ICON = $$PWD/assets/icons/qv2ray.icns
RC_ICONS += $$PWD/assets/icons/qv2ray.ico

include($$PWD/makespec/02-translations.pri)

unix {
    include($$PWD/makespec/03-unix.pri)
    # Sub-process of Qv2ray per-OS build
    !macx: include($$PWD/makespec/04-unix-linux.pri)
    macx: include($$PWD/makespec/04-unix-macOS.pri)
} else {
    include($$PWD/makespec/03-Windows.pri)
}

# ------------------------------------------ Begin checking protobuf domain list headers.
!exists($$PWD/libs/gen/v2ray_geosite.pb.cc) || !exists($$PWD/libs/gen/v2ray_geosite.pb.cc) {
    Qv2rayQMakeError("Protobuf headers for v2ray geosite is missing.")
}

SOURCES += $$PWD/libs/gen/v2ray_geosite.pb.cc
HEADERS += $$PWD/libs/gen/v2ray_geosite.pb.h

# General header and source files for gRPC and libQvb
message(" ")
use_grpc {
    DEFINES += WITH_LIB_GRPCPP
    message("Qv2ray will use gRPC as API backend")

    !exists($$PWD/libs/gen/v2ray_api.grpc.pb.h) || !exists($$PWD/libs/gen/v2ray_api.grpc.pb.cc) || !exists($$PWD/libs/gen/v2ray_api.pb.h) || !exists($$PWD/libs/gen/v2ray_api.pb.cc) {
        Qv2rayQMakeError("gRPC and protobuf headers for v2ray API is missing.")
    }

    SOURCES += $$PWD/libs/gen/v2ray_api.pb.cc $$PWD/libs/gen/v2ray_api.grpc.pb.cc
    HEADERS += $$PWD/libs/gen/v2ray_api.pb.h $$PWD/libs/gen/v2ray_api.grpc.pb.h
} else {
    message("Qv2ray will use libqvb as API backend")
    !exists($$PWD/libs/libqvb/build/libqvb.h) {
        Qv2rayQMakeError("libs/libqvb/build/libqvb.h is missing.")
    }
    HEADERS += $$PWD/libs/libqvb/build/libqvb.h
}

# Misc steps to build Qv2ray.
include($$PWD/makespec/99-others.pri)

message(" ")
message("This Qv2ray build contains: ")
message("  --> $${size(SOURCES)} source files")
message("  --> $${size(HEADERS)} header files")
message("  --> $${size(FORMS)} ui files")
message("  --> $${size(TRANSLATIONS)} translation files")
message("  --> $${size(EXTRA_TRANSLATIONS)} extra translation files")
message(" ")
message("Finished configuring Qv2ray project. Build output will be at:" $$OUT_PWD)
message("Type 'make' or 'nmake' to start building Qv2ray")
