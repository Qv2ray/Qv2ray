#include "BuiltinProtocolPlugin.hpp"

#include "core/OutboundHandler.hpp"
#include "ui/Interface.hpp"

bool InternalProtocolSupportPlugin::InitializePlugin()
{
    m_OutboundHandler = std::make_shared<BuiltinSerializer>();
    m_GUIInterface = new ProtocolGUIInterface();
    return true;
}
