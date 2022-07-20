#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "ui/RoutingAEditor.hpp"

class RoutingAGuiInterface : public Qv2rayPlugin::Gui::Qv2rayGUIInterface
{
  public:
    virtual QIcon Icon() const override
    {
        return {};
    }
    virtual QList<Qv2rayPlugin::PLUGIN_GUI_COMPONENT_TYPE> GetComponents() const override
    {
        return { Qv2rayPlugin::GUI_COMPONENT_MAIN_WINDOW_ACTIONS };
    }
    virtual std::unique_ptr<Qv2rayPlugin::Gui::PluginSettingsWidget> GetSettingsWidget() const override
    {
        return nullptr;
    }
    virtual PluginEditorDescriptor GetInboundEditors() const override
    {
        return {};
    }
    virtual PluginEditorDescriptor GetOutboundEditors() const override
    {
        return {};
    }
    virtual std::unique_ptr<Qv2rayPlugin::Gui::PluginMainWindowWidget> GetMainWindowWidget() const override
    {
        return std::make_unique<RoutingAEditor>();
    }
    virtual QList<QMenu> GetTrayMenus() const override
    {
        return {};
    }
    virtual ProfileEditorDescriptor GetProfileEditors() const override
    {
        return {};
    }
};
