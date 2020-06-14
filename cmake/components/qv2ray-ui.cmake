set(QV2RAY_UI_FORMS
    ${CMAKE_SOURCE_DIR}/src/ui/editors/w_OutboundEditor.ui
    ${CMAKE_SOURCE_DIR}/src/ui/editors/w_InboundEditor.ui
    ${CMAKE_SOURCE_DIR}/src/ui/editors/w_JsonEditor.ui
    ${CMAKE_SOURCE_DIR}/src/ui/editors/w_RoutesEditor.ui
    #
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/StreamSettingsWidget.ui
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/ConnectionInfoWidget.ui
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/ConnectionItemWidget.ui
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/RouteSettingsMatrix.ui
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/InboundSettingsWidget.ui
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/ConnectionSettingsWidget.ui
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/DnsSettingsWidget.ui
    #
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_GroupManager.ui
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_ImportConfig.ui
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_MainWindow.ui
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_PreferencesWindow.ui
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_PluginManager.ui
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_ScreenShot_Core.ui
    )

set(QV2RAY_UI_SOURCES
    # Qv2ray Application
    ${CMAKE_SOURCE_DIR}/src/Qv2rayApplication.cpp
    ${CMAKE_SOURCE_DIR}/src/Qv2rayApplication.hpp
    # Common Utils
    ${CMAKE_SOURCE_DIR}/src/ui/common/QvDialog.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/common/QRCodeHelper.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/common/QRCodeHelper.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/common/UIBase.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/common/JsonHighlighter.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/common/JsonHighlighter.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/common/LogHighlighter.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/common/LogHighlighter.cpp
    # Message bus
    ${CMAKE_SOURCE_DIR}/src/ui/messaging/QvMessageBus.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/messaging/QvMessageBus.cpp
    # NodeEditor Models
    ${CMAKE_SOURCE_DIR}/src/ui/models/NodeModelsBase.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/models/InboundNodeModel.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/models/InboundNodeModel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/models/OutboundNodeModel.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/models/OutboundNodeModel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/models/RuleNodeModel.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/models/RuleNodeModel.cpp
    # Style Manager
    ${CMAKE_SOURCE_DIR}/src/ui/styles/StyleManager.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/styles/StyleManager.hpp
    # UI Widgets
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/ConnectionInfoWidget.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/ConnectionInfoWidget.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/QvAutoCompleteTextEdit.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/QvAutoCompleteTextEdit.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/RouteSettingsMatrix.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/RouteSettingsMatrix.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/ConnectionSettingsWidget.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/ConnectionSettingsWidget.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/ConnectionItemWidget.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/ConnectionItemWidget.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/StreamSettingsWidget.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/StreamSettingsWidget.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/InboundSettingsWidget.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/InboundSettingsWidget.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/DnsSettingsWidget.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/widgets/DnsSettingsWidget.hpp
    # Editors
    ${CMAKE_SOURCE_DIR}/src/ui/editors/w_InboundEditor.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/editors/w_InboundEditor.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/editors/w_JsonEditor.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/editors/w_JsonEditor.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/editors/w_OutboundEditor.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/editors/w_OutboundEditor.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/editors/w_RoutesEditor.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/editors/w_RoutesEditor.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/editors/w_RoutesEditor_extra.cpp
    # Windows
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_ImportConfig.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_ImportConfig.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_MainWindow.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_MainWindow.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_MainWindow_extra.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_PreferencesWindow.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_PreferencesWindow.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_PluginManager.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_PluginManager.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_ScreenShot_Core.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_ScreenShot_Core.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_GroupManager.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/windows/w_GroupManager.cpp
    )
