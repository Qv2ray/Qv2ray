#include "EventHandler.hpp"

EventHandler::EventHandler()
{
}

void EventHandler::ProcessEvent(const ConnectionStats::EventObject &c)
{
    Q_UNUSED(c)
}

void EventHandler::ProcessEvent(const Connectivity::EventObject &c)
{
    Q_UNUSED(c)
}

void EventHandler::ProcessEvent(const SystemProxy::EventObject &c)
{
    Q_UNUSED(c)
}

void EventHandler::ProcessEvent(const ConnectionEntry::EventObject &c)
{
    Q_UNUSED(c)
}
