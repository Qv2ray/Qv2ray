#include "GuiPluginHost.hpp"

GuiPluginAPIHost::GuiPluginAPIHost(PluginAPIHost *host)
{
    apiHost = host;
}

QList<std::pair<Qv2rayInterface *, Qv2rayGUIInterface *>> GuiPluginAPIHost::GUI_QueryByComponent(QV2RAY_PLUGIN_GUI_COMPONENT_TYPE c) const
{
    QList<std::pair<Qv2rayInterface *, Qv2rayGUIInterface *>> guiInterfaces;
    for (const auto &plugin : apiHost->core->GetPlugins(COMPONENT_GUI))
    {
        const auto guiInterface = plugin->pinterface->GetGUIInterface();
        if (guiInterface->GetComponents().contains(c))
            guiInterfaces << std::make_pair(plugin->pinterface, guiInterface);
    }
    return guiInterfaces;
}
