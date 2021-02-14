set(SUBSCRIPTION_PLUGIN_TARGET QvPlugin-BuiltinSubscriptionSupport)

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

target_include_directories(${SUBSCRIPTION_PLUGIN_TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(${SUBSCRIPTION_PLUGIN_TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common)

qv2ray_configure_plugin(${SUBSCRIPTION_PLUGIN_TARGET})

target_link_libraries(${SUBSCRIPTION_PLUGIN_TARGET}
    Qt::Core
    Qt::Gui
    Qt::Widgets)
