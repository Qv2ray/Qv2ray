#pragma once

#include "QvGUIPluginInterface.hpp"
#include "QvPluginInterface.hpp"

#include <QObject>
#include <QtPlugin>

using namespace Qv2rayPlugin;

class InternalProtocolSupportPlugin
    : public QObject
    , Qv2rayInterface
{
    Q_INTERFACES(Qv2rayPlugin::Qv2rayInterface)
    Q_PLUGIN_METADATA(IID Qv2rayInterface_IID)
    Q_OBJECT
  public:
    //
    // Basic metainfo of this plugin
    const QvPluginMetadata GetMetadata() const override
    {
        return { "Built-in Protocol Support",                                                     //
                 "Qv2ray Core Workgroup",                                                         //
                 "qvplugin_builtin_protocol",                                                     //
                 "Enable built-in VMess, VLESS, SOCKS, HTTP, Shadowsocks, Dokodemo-door support", //
                 QV2RAY_VERSION_STRING,                                                           //
                 "Qv2ray/Qv2ray",                                                                 //
                 {
                     COMPONENT_OUTBOUND_HANDLER, //
                     COMPONENT_GUI               //
                 },
                 UPDATE_NONE };
    }

    bool InitializePlugin(const QString &, const QJsonObject &) override;
    //
  signals:
    void PluginLog(const QString &) const override;
    void PluginErrorMessageBox(const QString &, const QString &) const override;
};
