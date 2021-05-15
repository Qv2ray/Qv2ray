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

class ProtocolGUIInterface : public Qv2rayGUIInterface
{
  public:
    explicit ProtocolGUIInterface(){};
    ~ProtocolGUIInterface(){};
    QList<QV2RAY_PLUGIN_GUI_COMPONENT_TYPE> GetComponents() const override
    {
        return {
            GUI_COMPONENT_INBOUND_EDITOR, //
            GUI_COMPONENT_OUTBOUND_EDITOR //
        };
    }
    std::unique_ptr<PluginSettingsWidget> createSettingsWidgets() const override
    {
        return std::make_unique<SimplePluginSettingsWidget>();
    }
    QList<typed_plugin_editor> createInboundEditors() const override
    {
        return {
            make_editor_info<HTTPInboundEditor>("http", "HTTP"),
            make_editor_info<SocksInboundEditor>("socks", "SOCKS"),
            make_editor_info<DokodemoDoorInboundEditor>("dokodemo-door", "Dokodemo-Door"),
        };
    }
    QList<typed_plugin_editor> createOutboundEditors() const override
    {
        return {
            make_editor_info<VmessOutboundEditor>("vmess", "VMess"),                   //
            make_editor_info<VlessOutboundEditor>("vless", "VLESS"),                   //
            make_editor_info<ShadowsocksOutboundEditor>("shadowsocks", "Shadowsocks"), //
            make_editor_info<HttpOutboundEditor>("http", "HTTP"),                      //
            make_editor_info<SocksOutboundEditor>("socks", "SOCKS"),                   //
            make_editor_info<FreedomOutboundEditor>("freedom", "Freedom"),             //
            make_editor_info<BlackholeOutboundEditor>("blackhole", "Blackhole"),       //
            make_editor_info<DnsOutboundEditor>("dns", "DNS"),                         //
            make_editor_info<LoopbackSettingsEditor>("loopback", "Loopback"),          //
        };
    }
    std::unique_ptr<PluginMainWindowWidget> createMainWindowWidget() const override
    {
        return nullptr;
    }
    QIcon Icon() const override
    {
        return QIcon(":/assets/qv2ray.png");
    }
};
