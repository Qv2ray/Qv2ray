qv2ray_add_plugin(QvPlugin-BuiltinSubscriptionSupport Widgets
    INSTALL_PREFIX_MACOS "${CMAKE_INSTALL_PREFIX}/qv2ray.app/Contents/Resources/plugins"
    CLASS_NAME "InternalSubscriptionSupportPlugin")

target_sources(QvPlugin-BuiltinSubscriptionSupport PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/resx.qrc
    ${CMAKE_CURRENT_LIST_DIR}/BuiltinSubscriptionAdapter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/BuiltinSubscriptionAdapter.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/SubscriptionAdapter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/SubscriptionAdapter.hpp
    )

target_include_directories(QvPlugin-BuiltinSubscriptionSupport PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../PluginsCommon)
