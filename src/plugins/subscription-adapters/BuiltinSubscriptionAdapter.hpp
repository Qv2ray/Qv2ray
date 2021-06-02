#pragma once

#include "QvPluginInterface.hpp"

#include <QObject>
#include <QtPlugin>

using namespace Qv2rayPlugin;

class InternalSubscriptionSupportPlugin
    : public QObject
    , public Qv2rayInterface
{
    Q_INTERFACES(Qv2rayPlugin::Qv2rayInterface)
    Q_PLUGIN_METADATA(IID Qv2rayInterface_IID)
    Q_OBJECT
  public:
    InternalSubscriptionSupportPlugin() = default;
    // Basic metainfo of this plugin
    const QvPluginMetadata GetMetadata() const override
    {
        return { "Builtin Subscription Support",          //
                 "Qv2ray Core Workgroup",                 //
                 "qv2ray_builtin_subscription",           //
                 "Basic subscription support for Qv2ray", //
                 "Qv2ray Repository",                     //
                 { COMPONENT_SUBSCRIPTION_ADAPTER } };
    }

    bool InitializePlugin() override;
    void SettingsUpdated() override{};

  signals:
    void PluginLog(QString) const override;
    void PluginErrorMessageBox(QString, QString) const override;
};
