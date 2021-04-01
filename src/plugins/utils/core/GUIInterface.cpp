#include "GUIInterface.hpp"

#include "MainWindowWidget.hpp"

GUIInterface::GUIInterface()
{
}

QList<GUIInterface::typed_plugin_editor> GUIInterface::createInboundEditors() const
{
    return {};
}

QList<GUIInterface::typed_plugin_editor> GUIInterface::createOutboundEditors() const
{
    return {};
}

std::unique_ptr<Qv2rayPlugin::QvPluginSettingsWidget> GUIInterface::createSettingsWidgets() const
{
    return nullptr;
}

std::unique_ptr<Qv2rayPlugin::QvPluginMainWindowWidget> GUIInterface::createMainWindowWidget() const
{
    return std::make_unique<MainWindowWidget>();
}
