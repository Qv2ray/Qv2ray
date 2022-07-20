#pragma once

#include "QvPlugin/PluginInterface.hpp"

#include <QtPlugin>

using namespace Qv2rayPlugin;

class RoutingAPlugin
    : public QObject
    , public Qv2rayInterface<RoutingAPlugin>
{
    Q_OBJECT
    QV2RAY_PLUGIN(RoutingAPlugin)

  public:
    const QvPluginMetadata GetMetadata() const override
    {
        return {
            "RoutingA Editor Plugin",
            "Moody",
            PluginId("RoutingA_Plugin"),
            "Allowing edit complex configurations in RoutingA syntax.",
            QUrl{},
            {
                COMPONENT_GUI,
            },
        };
    }

    bool InitializePlugin() override;
    void SettingsUpdated() override;
};
