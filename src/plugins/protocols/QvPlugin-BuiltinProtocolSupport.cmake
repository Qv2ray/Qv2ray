set(PROTOCOL_PLUGIN_TARGET QvPlugin-BuiltinProtocolSupport)

set(QVPLUGIN_INTERFACE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/plugin-interface)

include(${CMAKE_SOURCE_DIR}/src/plugin-interface/QvPluginInterface.cmake)
include(${CMAKE_SOURCE_DIR}/src/plugin-interface/QvGUIPluginInterface.cmake)

macro(ADD_SOURCE mode name)
    list(APPEND PLUGIN_UI_SOURCE "${CMAKE_CURRENT_LIST_DIR}/ui/${mode}/${name}.ui")
    list(APPEND PLUGIN_UI_SOURCE "${CMAKE_CURRENT_LIST_DIR}/ui/${mode}/${name}.cpp")
    list(APPEND PLUGIN_UI_SOURCE "${CMAKE_CURRENT_LIST_DIR}/ui/${mode}/${name}.hpp")
endmacro()

ADD_SOURCE(outbound blackhole)
ADD_SOURCE(outbound dns)
ADD_SOURCE(outbound freedom)
ADD_SOURCE(outbound httpout)
ADD_SOURCE(outbound socksout)
ADD_SOURCE(outbound shadowsocks)
ADD_SOURCE(outbound vless)
ADD_SOURCE(outbound vmess)
ADD_SOURCE(outbound loopback)

ADD_SOURCE(inbound dokodemo-door)
ADD_SOURCE(inbound httpin)
ADD_SOURCE(inbound socksin)
#ADD_SOURCE(inbound shadowsocks)
#ADD_SOURCE(inbound vless)
#ADD_SOURCE(inbound vmess)

set(BUILTIN_PROTOCOL_PLUGIN_SOURCES
    ${PLUGIN_UI_SOURCE}
    ${CMAKE_CURRENT_LIST_DIR}/../common/CommonTypes.hpp
    ${CMAKE_CURRENT_LIST_DIR}/BuiltinProtocolPlugin.hpp
    ${CMAKE_CURRENT_LIST_DIR}/BuiltinProtocolPlugin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/OutboundHandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/OutboundHandler.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/Interface.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/PluginSettingsWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/PluginSettingsWidget.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/PluginSettingsWidget.ui
    ${QVPLUGIN_INTERFACE_HEADERS}
    ${QVGUIPLUGIN_INTERFACE_HEADERS}
    )

list(APPEND PLUGIN_TRANSLATION_SOURCES ${BUILTIN_PROTOCOL_PLUGIN_SOURCES})

add_library(${PROTOCOL_PLUGIN_TARGET} MODULE
    ${CMAKE_CURRENT_LIST_DIR}/resx.qrc
    ${BUILTIN_PROTOCOL_PLUGIN_SOURCES}
    )

target_include_directories(${PROTOCOL_PLUGIN_TARGET} PRIVATE ${QVPLUGIN_INTERFACE_INCLUDE_DIR})
target_include_directories(${PROTOCOL_PLUGIN_TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(${PROTOCOL_PLUGIN_TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common)

if(UNIX AND NOT APPLE AND NOT WIN32 AND NOT ANDROID)
    install(TARGETS ${PROTOCOL_PLUGIN_TARGET} LIBRARY DESTINATION share/qv2ray/plugins)
elseif(WIN32)
    install(TARGETS ${PROTOCOL_PLUGIN_TARGET} LIBRARY DESTINATION plugins)
elseif(APPLE)
    add_custom_command(TARGET ${PROTOCOL_PLUGIN_TARGET}
        POST_BUILD
        COMMAND ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "@executable_path/../Frameworks/" $<TARGET_FILE:${PROTOCOL_PLUGIN_TARGET}>)
    install(TARGETS ${PROTOCOL_PLUGIN_TARGET} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/qv2ray.app/Contents/Resources/plugins)
elseif(ANDROID)
    set(deployment_tool "${QT_HOST_PATH}/${QT6_HOST_INFO_BINDIR}/androiddeployqt")
    set(apk_dir "$<TARGET_PROPERTY:${PROTOCOL_PLUGIN_TARGET},BINARY_DIR>/android-build")
    add_custom_command(TARGET ${PROTOCOL_PLUGIN_TARGET} POST_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROTOCOL_PLUGIN_TARGET}>
        "${apk_dir}/libs/${CMAKE_ANDROID_ARCH_ABI}/$<TARGET_FILE_NAME:${PROTOCOL_PLUGIN_TARGET}>"
        )
else()
    message(FATAL_ERROR "?")
endif()

target_link_libraries(${PROTOCOL_PLUGIN_TARGET}
    ${QV_QT_LIBNAME}::Core
    ${QV_QT_LIBNAME}::Gui
    ${QV_QT_LIBNAME}::Widgets)
