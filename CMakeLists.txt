cmake_minimum_required(VERSION 3.5.1)

file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/VERSION" QV2RAY_VERSION)
file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/BUILDVERSION" QV2RAY_BUILD_VERSION)
set(PACKAGE_VERSION "${QV2RAY_VERSION}.${QV2RAY_BUILD_VERSION}")

project(Qv2ray)
add_definitions(-DQV2RAY_VERSION_STRING="\\\"v${PACKAGE_VERSION}\\\"")
add_definitions(-DXTOSTRUCT_QT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

cmake_policy(SET CMP0071 NEW)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

include(cmake/translations.cmake)
include(cmake/qnodeeditor.cmake)
include(cmake/singleapplication.cmake)
include(cmake/qzxing.cmake)
include(cmake/protobuf.cmake)
include(cmake/cpp-httplib.cmake)
include(cmake/backend.cmake)

if(DEFINED ENV{_QV2RAY_BUILD_INFO_})
    set(_QV2RAY_BUILD_INFO_STR_ "$ENV{_QV2RAY_BUILD_INFO_}")
else()
    set(_QV2RAY_BUILD_INFO_STR_ "Qv2ray from manual build")
endif()

if(DEFINED ENV{_QV2RAY_BUILD_EXTRA_INFO_})
    set(_QV2RAY_BUILD_EXTRA_INFO_STR_ "$ENV{_QV2RAY_BUILD_EXTRA_INFO_}")
else()
    set(_QV2RAY_BUILD_EXTRA_INFO_STR_ "Qv2ray v${PACKAGE_VERSION}")
endif()

add_definitions(-D_QV2RAY_BUILD_INFO_STR_="\\\"${_QV2RAY_BUILD_INFO_STR_}\\\"")
add_definitions(-D_QV2RAY_BUILD_EXTRA_INFO_STR_="\\\"${_QV2RAY_BUILD_EXTRA_INFO_STR_}\\\"")
message("Qv2ray build info: ${_QV2RAY_BUILD_INFO_STR_}")
message("Qv2ray build info ex: ${_QV2RAY_BUILD_EXTRA_INFO_STR_}")

set(QV2RAY_SOURCES
  src/base/Qv2rayLog.cpp
  src/common/CommandArgs.cpp
  src/common/HTTPRequestHelper.cpp
  src/common/LogHighlighter.cpp
  src/common/QJsonModel.cpp
  src/common/QvHelpers.cpp
  src/components/autolaunch/QvAutoLaunch.cpp
  src/components/geosite/QvGeositeReader.cpp
  src/components/icmping/win/ICMPPinger.cpp
  src/components/pac/QvGFWPACConverter.cpp
  src/components/pac/QvPACHandler.cpp
  src/components/plugins/toolbar/QvToolbar.cpp
  src/components/plugins/toolbar/QvToolbar_linux.cpp
  src/components/plugins/toolbar/QvToolbar_win.cpp
  src/components/proxy/QvProxyConfigurator.cpp
  src/components/speedchart/speedplotview.cpp
  src/components/speedchart/speedwidget.cpp
  src/core/connection/ConnectionIO.cpp
  src/core/connection/Generation.cpp
  src/core/connection/Serialization.cpp
  src/core/connection/Serialization_ss.cpp
  src/core/connection/Serialization_ssd.cpp
  src/core/connection/Serialization_vmess.cpp
  src/core/CoreUtils.cpp
  src/core/handler/ConfigHandler.cpp
  src/core/handler/V2rayInstanceHandler.cpp
  src/core/kernel/APIBackend.cpp
  src/core/kernel/KernelInteractions.cpp
  src/core/settings/SettingsBackend.cpp
  src/core/settings/SettingsUpgrade.cpp
  src/core/tcping/QvTCPing.cpp
  src/main.cpp
  src/ui/editors/w_InboundEditor.cpp
  src/ui/editors/w_JsonEditor.cpp
  src/ui/editors/w_OutboundEditor.cpp
  src/ui/editors/w_RoutesEditor.cpp
  src/ui/editors/w_RoutesEditor_extra.cpp
  src/ui/messaging/QvMessageBus.cpp
  src/ui/models/InboundNodeModel.cpp
  src/ui/models/OutboundNodeModel.cpp
  src/ui/models/RuleNodeModel.cpp
  src/ui/widgets/ConnectionInfoWidget.cpp
  src/ui/widgets/ConnectionItemWidget.cpp
  src/ui/widgets/QvAutoCompleteTextEdit.cpp
  src/ui/widgets/StreamSettingsWidget.cpp
  src/ui/w_ExportConfig.cpp
  src/ui/w_ImportConfig.cpp
  src/ui/w_MainWindow.cpp
  src/ui/w_MainWindow_extra.cpp
  src/ui/w_PreferencesWindow.cpp
  src/ui/w_ScreenShot_Core.cpp
  src/ui/w_SubscriptionManager.cpp
)

set(QRC_RESOURCES ${CMAKE_SOURCE_DIR}/resources.qrc)

set(QT_LIBRARY
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
)

if(APPLE)
  set(MACOSX_FRAMEWORK 
    "-framework Carbon"
    "-framework Cocoa"
    "-framework Security"
  )
endif()

add_executable(qv2ray
  ${QV2RAY_SOURCES}
  ${QNODEEDITOR_SOURCES}
  ${QNODEEDITOR_QRC_RESOURCES}
  ${SINGLEAPPLICATION_SOURCES}
  ${QZXING_SOURCES}
  ${PROTO_SRCS}
  ${PROTO_HDRS}
  ${API_GRPC_SRCS}
  ${API_GRPC_HDRS}
  ${QRC_RESOURCES}
)

target_link_libraries(qv2ray
  ${QT_LIBRARY}
  ${MACOSX_FRAMEWORK}
  ${QV2RAY_PROTOBUF_LIBRARY}
  ${QV2RAY_BACKEND_LIBRARIES}
)


target_include_directories(qv2ray PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}
  ${QHTTPSERVER_DIR}
  ${QNODEEDITOR_INCLUDE_PATH}
  ${QZXING_INCLUDE_PATH}
  ${SINGLEAPPLICATION_DIR}
  ${Protobuf_INCLUDE_DIRS}
)

install(TARGETS qv2ray
        RUNTIME
        DESTINATION bin
)

if(UNIX AND NOT APPLE)
  install(FILES assets/qv2ray.metainfo.xml
          DESTINATION share/metainfo
  )

  install(FILES assets/qv2ray.desktop
          DESTINATION share/applications
  )

  install(FILES assets/icons/qv2ray.png
          DESTINATION share/icons/hicolor/256x256/apps/
  )
endif()

if(UNIX AND NOT WIN32)
  install(FILES ${QM_FILES}
          DESTINATION share/qv2ray/translations
  )
endif()
