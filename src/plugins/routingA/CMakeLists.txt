set(CMAKE_INCLUDE_CURRENT_DIR ON)

qv2ray_add_plugin(QvPlugin-RoutingAEditor Widgets
    INSTALL_PREFIX_MACOS "${CMAKE_INSTALL_PREFIX}/qv2ray.app/Contents/Resources/plugins"
    CLASS_NAME "RoutingAPlugin"
    DEV_INTERFACE)

target_sources(QvPlugin-RoutingAEditor PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/RoutingAPlugin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/RoutingAPlugin.hpp
    ${CMAKE_CURRENT_LIST_DIR}/RoutingA_GuiInterface.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/libRoutingA-ActionTable.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/libRoutingA.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/libRoutingA.hpp
    ${CMAKE_CURRENT_LIST_DIR}/common/RoutingAConverter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/common/RoutingAConverter.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/RoutingAEditor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/RoutingAEditor.ui
    ${CMAKE_CURRENT_LIST_DIR}/ui/RoutingAEditor.hpp
    )

target_include_directories(QvPlugin-RoutingAEditor PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../PluginsCommon)
