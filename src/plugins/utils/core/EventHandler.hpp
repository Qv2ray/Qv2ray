#pragma once

#include "handlers/EventHandler.hpp"

#include <QtCore/qglobal.h>

using namespace Qv2rayPlugin::handlers::event;
class EventHandler : public Qv2rayPlugin::handlers::event::PluginEventHandler
{
  public:
    EventHandler();

    void ProcessEvent(const ConnectionStats::EventObject &c) override;
    void ProcessEvent(const Connectivity::EventObject &c) override;
    void ProcessEvent(const SystemProxy::EventObject &c) override;
    void ProcessEvent(const ConnectionEntry::EventObject &c) override;
};
