#include "PluginSettingsWidget.hpp"
#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
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
#include "outbound/trojan.hpp"
#include "outbound/vmess.hpp"

using namespace Qv2rayPlugin;

class ProtocolGUIInterface : public Gui::Qv2rayGUIInterface
{
  public:
    explicit ProtocolGUIInterface(){};
    virtual ~ProtocolGUIInterface() override = default;
    QList<PLUGIN_GUI_COMPONENT_TYPE> GetComponents() const override
    {
        return {
            GUI_COMPONENT_INBOUND_EDITOR, //
            GUI_COMPONENT_OUTBOUND_EDITOR //
        };
    }
    std::unique_ptr<Gui::PluginSettingsWidget> GetSettingsWidget() const override
    {
        return std::make_unique<SimplePluginSettingsWidget>();
    }
    QList<QPair<Qv2rayPlugin::Gui::ProtocolInfoObject, Qv2rayPlugin::Gui::PluginProtocolEditor *>> GetInboundEditors() const override
    {
        return {
            make_editor_info<HTTPInboundEditor>("http", "HTTP"),
            make_editor_info<SocksInboundEditor>("socks", "SOCKS"),
            make_editor_info<DokodemoDoorInboundEditor>("dokodemo-door", "Dokodemo-Door"),
        };
    }
    QList<QPair<Qv2rayPlugin::Gui::ProtocolInfoObject, Qv2rayPlugin::Gui::PluginProtocolEditor *>> GetOutboundEditors() const override
    {
        return {
            make_editor_info<VmessOutboundEditor>("vmess", "VMess"),                   //
            make_editor_info<TrojanOutboundEditor>("trojan", "Trojan"),                //
            make_editor_info<ShadowsocksOutboundEditor>("shadowsocks", "Shadowsocks"), //
            make_editor_info<HttpOutboundEditor>("http", "HTTP"),                      //
            make_editor_info<SocksOutboundEditor>("socks", "SOCKS"),                   //
            make_editor_info<FreedomOutboundEditor>("freedom", "Freedom"),             //
            make_editor_info<BlackholeOutboundEditor>("blackhole", "Blackhole"),       //
            make_editor_info<DnsOutboundEditor>("dns", "DNS"),                         //
            make_editor_info<LoopbackSettingsEditor>("loopback", "Loopback"),          //
        };
    }
    std::unique_ptr<Qv2rayPlugin::Gui::PluginMainWindowWidget> GetMainWindowWidget() const override
    {
        return nullptr;
    }
    QIcon Icon() const override
    {
        return QIcon(":/assets/qv2ray.png");
    }
};
