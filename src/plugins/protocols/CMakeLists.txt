project(QvPlugin-BuiltinProtocolSupport)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(QVPLUGIN_INTERFACE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/plugin-interface)
include(${CMAKE_SOURCE_DIR}/src/plugin-interface/QvPluginInterface.cmake)
list(APPEND BUILTIN_PROTOCOL_SUPPORT_SOURCES ${QVPLUGIN_INTERFACE_HEADERS})

include(${CMAKE_SOURCE_DIR}/src/plugin-interface/QvGUIPluginInterface.cmake)
list(APPEND BUILTIN_PROTOCOL_SUPPORT_SOURCES ${QVGUIPLUGIN_INTERFACE_HEADERS})

include_directories(${QVPLUGIN_INTERFACE_INCLUDE_DIR})

macro(ADD_INBOUND_OUTBOUND mode name)
    list(APPEND PLUGIN_UI_SOURCE "ui/${mode}/${name}.ui")
    list(APPEND PLUGIN_UI_SOURCE "ui/${mode}/${name}.cpp")
    list(APPEND PLUGIN_UI_SOURCE "ui/${mode}/${name}.hpp")
endmacro()

ADD_INBOUND_OUTBOUND(outbound blackhole)
ADD_INBOUND_OUTBOUND(outbound dns)
ADD_INBOUND_OUTBOUND(outbound freedom)
ADD_INBOUND_OUTBOUND(outbound httpout)
ADD_INBOUND_OUTBOUND(outbound socksout)
ADD_INBOUND_OUTBOUND(outbound shadowsocks)
ADD_INBOUND_OUTBOUND(outbound vless)
ADD_INBOUND_OUTBOUND(outbound vmess)

ADD_INBOUND_OUTBOUND(inbound dokodemo-door)
ADD_INBOUND_OUTBOUND(inbound httpin)
ADD_INBOUND_OUTBOUND(inbound socksin)
#ADD_INBOUND_OUTBOUND(inbound shadowsocks)
#ADD_INBOUND_OUTBOUND(inbound vless)
#ADD_INBOUND_OUTBOUND(inbound vmess)

add_library(${PROJECT_NAME} MODULE
    ${PLUGIN_UI_SOURCE}
    BuiltinProtocolPlugin.cpp
    BuiltinProtocolPlugin.cpp
    core/OutboundHandler.cpp
    core/OutboundHandler.hpp
    ui/Interface.hpp
    ui/PluginSettingsWidget.cpp
    ui/PluginSettingsWidget.hpp
    ui/PluginSettingsWidget.ui
    resx.qrc
    ${BUILTIN_PROTOCOL_SUPPORT_SOURCES})


if(APPLE)
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "@executable_path/../Frameworks/" $<TARGET_FILE:${PROJECT_NAME}>)
endif()


if(UNIX AND NOT APPLE AND NOT WIN32 AND NOT ANDROID)
    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION share/qv2ray/plugins)
elseif(WIN32 OR ANDROID)
    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION plugins)
elseif(APPLE)
    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/qv2ray.app/Contents/Resources/plugins)
else()
    message(FATAL_ERROR "?")
endif()

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)
