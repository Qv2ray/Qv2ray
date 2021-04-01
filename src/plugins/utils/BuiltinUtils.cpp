#include "BuiltinUtils.hpp"

#include "core/EventHandler.hpp"
#include "core/GUIInterface.hpp"

bool InternalUtilsPlugin::InitializePlugin(const QString &, const QJsonObject &settings)
{
    this->settings = settings;
    this->eventHandler = std::make_shared<EventHandler>();
    this->guiInterface = new GUIInterface;
    return true;
}
