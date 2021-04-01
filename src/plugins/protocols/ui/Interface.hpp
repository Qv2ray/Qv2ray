#include "PluginSettingsWidget.hpp"
#include "QvGUIPluginInterface.hpp"
//
#include "inbound/dokodemo-door.hpp"
#include "inbound/httpin.hpp"
#include "inbound/socksin.hpp"
//
#include "outbound/blackhole.hpp"
#include "outbound/dns.hpp"
#include "outbound/freedom.hpp"
#include "outbound/httpout.hpp"
#include "outbound/loopback.hpp"
#include "outbound/shadowsocks.hpp"
#include "outbound/socksout.hpp"
#include "outbound/vless.hpp"
#include "outbound/vmess.hpp"

using namespace Qv2rayPlugin;

class ProtocolGUIInterface : public PluginGUIInterface
{
  public:
    explicit ProtocolGUIInterface(){};
    ~ProtocolGUIInterface(){};
    QList<PluginGuiComponentType> GetComponents() const override
    {
        return {
            GUI_COMPONENT_INBOUND_EDITOR, //
            GUI_COMPONENT_OUTBOUND_EDITOR //
        };
    }
    std::unique_ptr<QvPluginSettingsWidget> createSettingsWidgets() const override
    {
        return std::make_unique<SimplePluginSettingsWidget>();
    }
    QList<typed_plugin_editor> createInboundEditors() const override
    {
        return {
            MakeEditorInfoPair<HTTPInboundEditor>("http", "HTTP"),
            MakeEditorInfoPair<SocksInboundEditor>("socks", "SOCKS"),
            MakeEditorInfoPair<DokodemoDoorInboundEditor>("dokodemo-door", "Dokodemo-Door"),
        };
    }
    QList<typed_plugin_editor> createOutboundEditors() const override
    {
        return {
            MakeEditorInfoPair<VmessOutboundEditor>("vmess", "VMess"),                   //
            MakeEditorInfoPair<VlessOutboundEditor>("vless", "VLESS"),                   //
            MakeEditorInfoPair<ShadowsocksOutboundEditor>("shadowsocks", "Shadowsocks"), //
            MakeEditorInfoPair<HttpOutboundEditor>("http", "HTTP"),                      //
            MakeEditorInfoPair<SocksOutboundEditor>("socks", "SOCKS"),                   //
            MakeEditorInfoPair<FreedomOutboundEditor>("freedom", "Freedom"),             //
            MakeEditorInfoPair<BlackholeOutboundEditor>("blackhole", "Blackhole"),       //
            MakeEditorInfoPair<DnsOutboundEditor>("dns", "DNS"),                         //
            MakeEditorInfoPair<LoopbackSettingsEditor>("loopback", "Loopback"),          //
        };
    }
    std::unique_ptr<QvPluginMainWindowWidget> createMainWindowWidget() const override
    {
        return nullptr;
    }
    QIcon Icon() const override
    {
        return QIcon(":/assets/qv2ray.png");
    }
};
