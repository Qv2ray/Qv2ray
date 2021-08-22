#include "RoutingAPlugin.hpp"

#include "RoutingA_GuiInterface.hpp"

bool RoutingAPlugin::InitializePlugin()
{
    m_GUIInterface = new RoutingAGuiInterface;
    return true;
}

void RoutingAPlugin::SettingsUpdated()
{
}
