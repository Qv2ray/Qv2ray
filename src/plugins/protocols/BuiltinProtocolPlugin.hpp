#pragma once

#include "QvPlugin/PluginInterface.hpp"

#include <QObject>
#include <QtPlugin>

using namespace Qv2rayPlugin;

class InternalProtocolSupportPlugin
    : public QObject
    , public Qv2rayInterface<InternalProtocolSupportPlugin>
{
    Q_OBJECT
    QV2RAY_PLUGIN(InternalProtocolSupportPlugin)

  public:
    //
    // Basic metainfo of this plugin
    const QvPluginMetadata GetMetadata() const override
    {
        return {
            "Builtin Protocol Support",
            "Qv2ray Core Workgroup",
            PluginId("qv2ray_builtin_protocol"),
            "VMess, SOCKS, HTTP, Shadowsocks, DNS, Dokodemo-door editor support",
            QUrl{ "Qv2ray Repository" },
            {
                COMPONENT_OUTBOUND_HANDLER,
                COMPONENT_GUI,
            },
        };
    }

    bool InitializePlugin() override;
    void SettingsUpdated() override{};
};
