#pragma once

#include "QvGUIPluginInterface.hpp"

class GUIInterface : public Qv2rayPlugin::Qv2rayGUIInterface
{
  public:
    GUIInterface();
    QIcon Icon() const override
    {
        return QIcon(":/assets/qv2ray.png");
    }

    QList<Qv2rayPlugin::QV2RAY_PLUGIN_GUI_COMPONENT_TYPE> GetComponents() const override
    {
        return { Qv2rayPlugin::GUI_COMPONENT_MAIN_WINDOW_WIDGET };
    }

  private:
    QList<typed_plugin_editor> createInboundEditors() const override;
    QList<typed_plugin_editor> createOutboundEditors() const override;
    std::unique_ptr<Qv2rayPlugin::PluginSettingsWidget> createSettingsWidgets() const override;
    std::unique_ptr<Qv2rayPlugin::PluginMainWindowWidget> createMainWindowWidget() const override;
};
