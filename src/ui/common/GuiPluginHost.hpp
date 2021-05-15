#pragma once

#include "src/components/plugins/PluginAPIHost.hpp"
#include "src/plugin-interface/gui/QvGUIPluginInterface.hpp"

namespace Qv2ray::ui::common
{
    class GuiPluginAPIHost
    {
      public:
        GuiPluginAPIHost(PluginAPIHost *host);
        QList<std::pair<Qv2rayInterface *, Qv2rayGUIInterface *>> GUI_QueryByComponent(QV2RAY_PLUGIN_GUI_COMPONENT_TYPE c) const;

      private:
        PluginAPIHost *apiHost;
    };

    inline GuiPluginAPIHost *GUIPluginHost;
} // namespace Qv2ray::ui::common

using namespace Qv2ray::ui::common;
