set(SUBSCRIPTION_PLUGIN_TARGET QvPlugin-BuiltinSubscriptionSupport)

set(QVPLUGIN_INTERFACE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/plugin-interface)

include(${CMAKE_SOURCE_DIR}/src/plugin-interface/QvPluginInterface.cmake)
include(${CMAKE_SOURCE_DIR}/src/plugin-interface/QvGUIPluginInterface.cmake)

set(BUILTIN_SUBSCRIPTION_PLUGIN_SOURCES
    ${QVPLUGIN_INTERFACE_HEADERS}
    ${QVGUIPLUGIN_INTERFACE_HEADERS}
    ${CMAKE_CURRENT_LIST_DIR}/BuiltinSubscriptionAdapter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/BuiltinSubscriptionAdapter.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/SubscriptionAdapter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/SubscriptionAdapter.hpp
    )
list(APPEND PLUGIN_TRANSLATION_SOURCES ${BUILTIN_PROTOCOL_PLUGIN_SOURCES})

add_library(${SUBSCRIPTION_PLUGIN_TARGET} MODULE
    ${CMAKE_CURRENT_LIST_DIR}/resx.qrc
    ${BUILTIN_SUBSCRIPTION_PLUGIN_SOURCES}
    )

target_include_directories(${SUBSCRIPTION_PLUGIN_TARGET} PRIVATE ${QVPLUGIN_INTERFACE_INCLUDE_DIR})
target_include_directories(${SUBSCRIPTION_PLUGIN_TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(${SUBSCRIPTION_PLUGIN_TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common)

if(UNIX AND NOT APPLE AND NOT WIN32 AND NOT ANDROID)
    install(TARGETS ${SUBSCRIPTION_PLUGIN_TARGET} LIBRARY DESTINATION share/qv2ray/plugins)
elseif(WIN32)
    install(TARGETS ${SUBSCRIPTION_PLUGIN_TARGET} LIBRARY DESTINATION plugins)
elseif(APPLE)
    add_custom_command(TARGET ${SUBSCRIPTION_PLUGIN_TARGET}
        POST_BUILD
        COMMAND ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "@executable_path/../Frameworks/" $<TARGET_FILE:${SUBSCRIPTION_PLUGIN_TARGET}>)
    install(TARGETS ${SUBSCRIPTION_PLUGIN_TARGET} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/qv2ray.app/Contents/Resources/plugins)
elseif(ANDROID)
    set(deployment_tool "${QT_HOST_PATH}/${QT6_HOST_INFO_BINDIR}/androiddeployqt")
    set(apk_dir "$<TARGET_PROPERTY:${SUBSCRIPTION_PLUGIN_TARGET},BINARY_DIR>/android-build")
    add_custom_command(TARGET ${SUBSCRIPTION_PLUGIN_TARGET} POST_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${SUBSCRIPTION_PLUGIN_TARGET}>
        "${apk_dir}/libs/${CMAKE_ANDROID_ARCH_ABI}/$<TARGET_FILE_NAME:${SUBSCRIPTION_PLUGIN_TARGET}>"
        )
else()
    message(FATAL_ERROR "?")
endif()

target_link_libraries(${SUBSCRIPTION_PLUGIN_TARGET}
    ${QV_QT_LIBNAME}::Core
    ${QV_QT_LIBNAME}::Gui
    ${QV_QT_LIBNAME}::Widgets)
