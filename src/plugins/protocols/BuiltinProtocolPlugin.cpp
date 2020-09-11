#include "BuiltinProtocolPlugin.hpp"

#include "core/OutboundHandler.hpp"
#include "ui/Interface.hpp"

bool InternalProtocolSupportPlugin::InitializePlugin(const QString &, const QJsonObject &settings)
{
    this->settings = settings;
    outboundHandler = std::make_shared<BuiltinSerializer>();
    guiInterface = new ProtocolGUIInterface();
    return true;
}
