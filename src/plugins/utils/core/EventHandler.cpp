#include "EventHandler.hpp"

UtilEventHandler::UtilEventHandler()
{
}

void UtilEventHandler::ProcessEvent(const ConnectionStats::EventObject &c)
{
    Q_UNUSED(c)
}

void UtilEventHandler::ProcessEvent(const Connectivity::EventObject &c)
{
    Q_UNUSED(c)
}

void UtilEventHandler::ProcessEvent(const SystemProxy::EventObject &c)
{
    Q_UNUSED(c)
}

void UtilEventHandler::ProcessEvent(const ConnectionEntry::EventObject &c)
{
    Q_UNUSED(c)
}
