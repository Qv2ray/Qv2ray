#include "GuiPluginHost.hpp"

#include "Qv2rayBase/Plugin/PluginManagerCore.hpp"
#include "Qv2rayBase/Qv2rayBaseLibrary.hpp"

using namespace Qv2rayPlugin;

namespace Qv2ray::components::GuiPluginHost
{
    GuiPluginAPIHost::GuiPluginAPIHost()
    {
    }

    QList<std::pair<QvPluginMetadata, Gui::Qv2rayGUIInterface *>> GuiPluginAPIHost::QueryByGuiComponent(PLUGIN_GUI_COMPONENT_TYPE c) const
    {
        QList<std::pair<QvPluginMetadata, Gui::Qv2rayGUIInterface *>> guiInterfaces;
        for (const auto &plugin : QvPluginManagerCore->GetPlugins(Qv2rayPlugin::COMPONENT_GUI))
        {
            const auto guiInterface = plugin->pinterface->GetGUIInterface();
            if (guiInterface->GetComponents().contains(c))
                guiInterfaces << std::make_pair(plugin->metadata(), guiInterface);
        }
        return guiInterfaces;
    }
} // namespace Qv2ray::components::GuiPluginHost
