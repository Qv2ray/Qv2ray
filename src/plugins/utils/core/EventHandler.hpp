#pragma once

#include "QvPluginInterface.hpp"

#include <QtCore/qglobal.h>

class EventHandler : public Qv2rayPlugin::PluginEventHandler
{
  public:
    EventHandler();

    QvPlugin_EventHandler_Decl(ConnectionStats);
    QvPlugin_EventHandler_Decl(SystemProxy);
    QvPlugin_EventHandler_Decl(Connectivity);
    QvPlugin_EventHandler_Decl(ConnectionEntry);
};
