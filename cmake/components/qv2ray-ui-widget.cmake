set(QV2RAY_QWIDGETS_UI_BASEDIR ${CMAKE_SOURCE_DIR}/src/ui/widgets)
add_definitions(-DQAPPLICATION_CLASS=QApplication)

set(_QV2RAY_UI_FORMS
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_OutboundEditor.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_InboundEditor.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_JsonEditor.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_RoutesEditor.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_ChainSha256Editor.ui
    #
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/StreamSettingsWidget.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/ConnectionInfoWidget.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/ConnectionItemWidget.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/RouteSettingsMatrix.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/InboundSettingsWidget.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/ConnectionSettingsWidget.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/DnsSettingsWidget.ui
    #
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_GroupManager.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_ImportConfig.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_MainWindow.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_PreferencesWindow.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_PluginManager.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_ScreenShot_Core.ui
    #
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/complex/ChainEditorWidget.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/complex/RoutingEditorWidget.ui
    )

set(_QV2RAY_UI_NODEEDITOR_SOURCES
    # NodeEditor Models
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/NodeBase.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/NodeBase.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/NodeDispatcher.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/NodeDispatcher.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/models/InboundNodeModel.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/models/InboundNodeModel.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/models/OutboundNodeModel.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/models/OutboundNodeModel.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/models/RuleNodeModel.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/models/RuleNodeModel.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/models/ChainOutboundNodeModel.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/models/ChainOutboundNodeModel.hpp
    # NodeEditor Widgets
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/RuleWidget.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/RuleWidget.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/RuleWidget.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/InboundOutboundWidget.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/InboundOutboundWidget.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/InboundOutboundWidget.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/ChainWidget.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/ChainWidget.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/ChainWidget.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/BalancerWidget.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/BalancerWidget.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/BalancerWidget.ui
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/ChainOutboundWidget.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/ChainOutboundWidget.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/node/widgets/ChainOutboundWidget.ui
    )

set(_QV2RAY_UI_SOURCES
    # Style Manager
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/Qv2rayWidgetApplication.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/Qv2rayWidgetApplication.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/common/WidgetUIBase.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/common/QJsonModel.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/common/QJsonModel.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/styles/StyleManager.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/styles/StyleManager.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/styles/StyleManager.hpp
    # Models
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/models/ConnectionModelHelper.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/models/ConnectionModelHelper.hpp
    # UI Widgets
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/ConnectionInfoWidget.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/ConnectionInfoWidget.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/QvAutoCompleteTextEdit.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/QvAutoCompleteTextEdit.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/RouteSettingsMatrix.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/RouteSettingsMatrix.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/ConnectionSettingsWidget.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/ConnectionSettingsWidget.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/ConnectionItemWidget.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/ConnectionItemWidget.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/StreamSettingsWidget.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/StreamSettingsWidget.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/InboundSettingsWidget.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/InboundSettingsWidget.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/DnsSettingsWidget.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/DnsSettingsWidget.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/CertificateItemWidget.ui
    # Complex Widgets
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/complex/ChainEditorWidget.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/complex/ChainEditorWidget.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/complex/RoutingEditorWidget.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/widgets/complex/RoutingEditorWidget.hpp
    # Editors
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_InboundEditor.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_InboundEditor.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_JsonEditor.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_JsonEditor.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_OutboundEditor.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_OutboundEditor.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_RoutesEditor.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_RoutesEditor.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_ChainSha256Editor.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/editors/w_ChainSha256Editor.cpp
    # Windows
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_ImportConfig.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_ImportConfig.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_MainWindow.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_MainWindow.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_MainWindow_extra.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_PreferencesWindow.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_PreferencesWindow.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_PluginManager.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_PluginManager.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_ScreenShot_Core.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_ScreenShot_Core.cpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_GroupManager.hpp
    ${QV2RAY_QWIDGETS_UI_BASEDIR}/windows/w_GroupManager.cpp
    )

set(QV2RAY_UI_WIDGET_SOURCES ${_QV2RAY_UI_FORMS} ${_QV2RAY_UI_SOURCES} ${_QV2RAY_UI_NODEEDITOR_SOURCES})
