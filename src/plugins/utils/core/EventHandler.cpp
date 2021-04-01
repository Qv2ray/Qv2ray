#include "EventHandler.hpp"

EventHandler::EventHandler()
{
}

void ProcessEvent(const ConnectionStats::EventObject &c)
{
    Q_UNUSED(c)
}

void ProcessEvent(const Connectivity::EventObject &c)
{
    Q_UNUSED(c)
}

void ProcessEvent(const SystemProxy::EventObject &c)
{
    Q_UNUSED(c)
}

void ProcessEvent(const ConnectionEntry::EventObject &c)
{
    Q_UNUSED(c)
}
