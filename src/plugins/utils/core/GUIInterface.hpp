#pragma once

#include "QvGUIPluginInterface.hpp"

class GUIInterface : public Qv2rayPlugin::PluginGUIInterface
{
  public:
    GUIInterface();
    QIcon Icon() const override
    {
        return QIcon(":/assets/qv2ray.png");
    }
    QList<Qv2rayPlugin::PluginGuiComponentType> GetComponents() const override
    {
        return { Qv2rayPlugin::GUI_COMPONENT_MAINWINDOW_WIDGET };
    }

  private:
    QList<typed_plugin_editor> createInboundEditors() const override;
    QList<typed_plugin_editor> createOutboundEditors() const override;
    std::unique_ptr<Qv2rayPlugin::QvPluginSettingsWidget> createSettingsWidgets() const override;
    std::unique_ptr<Qv2rayPlugin::QvPluginMainWindowWidget> createMainWindowWidget() const override;
};
