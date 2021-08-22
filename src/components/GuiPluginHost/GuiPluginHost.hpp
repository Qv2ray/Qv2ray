#pragma once

#include "Qv2rayBase/Plugin/PluginAPIHost.hpp"
#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"

namespace Qv2ray::components::GuiPluginHost
{
    class GuiPluginAPIHost
    {
      public:
        GuiPluginAPIHost();
        QList<std::pair<Qv2rayPlugin::QvPluginMetadata, Qv2rayPlugin::Gui::Qv2rayGUIInterface *>> QueryByGuiComponent(Qv2rayPlugin::PLUGIN_GUI_COMPONENT_TYPE c) const;
    };
} // namespace Qv2ray::components::GuiPluginHost

inline Qv2ray::components::GuiPluginHost::GuiPluginAPIHost *GUIPluginHost;
