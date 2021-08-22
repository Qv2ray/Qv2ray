set(QV2RAY_FULL_SOURCES "")

macro(qv2ray_add_class CLASS)
    list(APPEND QV2RAY_FULL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${CLASS}.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${CLASS}.cpp)
endmacro()


macro(qv2ray_add_component COMP)
    add_compile_definitions(QV2RAY_COMPONENT_${COMP}=1)
    qv2ray_add_class(components/${COMP}/${COMP})
endmacro()

macro(qv2ray_add_widget WIDGET)
    list(APPEND QV2RAY_FULL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/${WIDGET}.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/${WIDGET}.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/${WIDGET}.ui)
endmacro()

macro(qv2ray_add_window WINDOW)
    qv2ray_add_widget(windows/${WINDOW})
endmacro()

list(APPEND QV2RAY_FULL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models/SettingsModels.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/PluginsCommon/V2RayModels.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/WidgetUIBase.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/windows/w_MainWindow_extra.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components/UpdateChecker/semver.hpp
    )

qv2ray_add_class(Qv2rayApplication)
qv2ray_add_class(ui/TrayManager)
qv2ray_add_class(ui/widgets/AutoCompleteTextEdit)
qv2ray_add_class(ui/widgets/ConfigurableEditorWidget)
qv2ray_add_class(ui/widgets/TagLineEditorWidget)
qv2ray_add_class(components/GeositeReader/picoproto)
qv2ray_add_class(plugins/internal/InternalProfilePreprocessor)
qv2ray_add_class(plugins/internal/InternalPlugin)

qv2ray_add_component(AutoLaunchHelper)
qv2ray_add_component(ConnectionModelHelper)
qv2ray_add_component(FlowLayout)
qv2ray_add_component(GeositeReader)
qv2ray_add_component(GuiPluginHost)
qv2ray_add_component(LogHighlighter)
qv2ray_add_component(MessageBus)
qv2ray_add_component(QJsonModel)
qv2ray_add_component(QRCodeHelper)
qv2ray_add_component(QueryParser)
qv2ray_add_component(RouteSchemeIO)
qv2ray_add_component(SpeedWidget)
qv2ray_add_component(StyleManager)
qv2ray_add_component(UpdateChecker)

qv2ray_add_window(w_AboutWindow)
qv2ray_add_window(w_GroupManager)
qv2ray_add_window(w_ImportConfig)
qv2ray_add_window(w_MainWindow)
qv2ray_add_window(w_PluginManager)
qv2ray_add_window(w_PreferencesWindow)

qv2ray_add_window(editors/w_JsonEditor)
qv2ray_add_window(editors/w_OutboundEditor)

qv2ray_add_widget(widgets/ConnectionInfoWidget)
qv2ray_add_widget(widgets/ConnectionItemWidget)
#qv2ray_add_widget(widgets/editors/CertificateItemWidget)
qv2ray_add_widget(widgets/editors/DnsSettingsWidget)
qv2ray_add_widget(widgets/editors/RouteSettingsMatrix)
qv2ray_add_widget(widgets/editors/StreamSettingsWidget)
