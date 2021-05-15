#pragma once

#include "handlers/EventHandler.hpp"

#include <QtCore/qglobal.h>

using namespace Qv2rayPlugin::handlers::event;
class UtilEventHandler : public Qv2rayPlugin::handlers::event::PluginEventHandler
{
  public:
    UtilEventHandler();

    void ProcessEvent(const ConnectionStats::EventObject &c) override;
    void ProcessEvent(const Connectivity::EventObject &c) override;
    void ProcessEvent(const SystemProxy::EventObject &c) override;
    void ProcessEvent(const ConnectionEntry::EventObject &c) override;
};
