set(UTILS_PLUGIN_TARGET QvPlugin-BuiltinUtils)

set(BUILTIN_UTILS_PLUGIN_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/BuiltinUtils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/BuiltinUtils.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/EventHandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/EventHandler.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/GUIInterface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/GUIInterface.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/MainWindowWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/MainWindowWidget.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/MainWindowWidget.ui
    )
list(APPEND PLUGIN_TRANSLATION_SOURCES ${BUILTIN_UTILS_PLUGIN_SOURCES})

add_library(${UTILS_PLUGIN_TARGET} MODULE
    ${CMAKE_CURRENT_LIST_DIR}/resx.qrc
    ${BUILTIN_UTILS_PLUGIN_SOURCES}
    )

qv2ray_configure_plugin(${UTILS_PLUGIN_TARGET} Widgets)
