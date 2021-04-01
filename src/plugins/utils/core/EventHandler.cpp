#include "EventHandler.hpp"

EventHandler::EventHandler()
{
}

QvPlugin_EventHandler(EventHandler, ConnectionStats)
{
    Q_UNUSED(pluginEvent);
}

QvPlugin_EventHandler(EventHandler, SystemProxy)
{
    Q_UNUSED(pluginEvent);
}

QvPlugin_EventHandler(EventHandler, Connectivity)
{
    Q_UNUSED(pluginEvent);
}

QvPlugin_EventHandler(EventHandler, ConnectionEntry)
{
    Q_UNUSED(pluginEvent);
}
