#pragma once

#include "QvPlugin/PluginInterface.hpp"

#include <QObject>
#include <QtPlugin>

using namespace Qv2rayPlugin;

class InternalSubscriptionSupportPlugin
    : public QObject
    , public Qv2rayInterface<InternalSubscriptionSupportPlugin>
{
    Q_OBJECT
    QV2RAY_PLUGIN(InternalSubscriptionSupportPlugin)

  public:
    // Basic metainfo of this plugin
    const QvPluginMetadata GetMetadata() const override
    {
        return QvPluginMetadata{
            "Builtin Subscription Support",
            "Qv2ray Core Workgroup",
            PluginId{ "builtin_subscription" },
            "Basic subscription support for Qv2ray",
            QUrl{},
            {
                COMPONENT_SUBSCRIPTION_ADAPTER,
            },
        };
    }

    bool InitializePlugin() override;
    void SettingsUpdated() override{};
};
