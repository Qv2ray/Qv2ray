#include "BuiltinV2RayCorePlugin.hpp"

#include "core/v2ray-go/V2RayGo.hpp"
#include "core/v2ray-v5/V2Ray5.hpp"
#include "core/v2ray/V2Ray.hpp"
#include "ui/w_V2RayKernelSettings.hpp"

const QvPluginMetadata BuiltinV2RayCorePlugin::GetMetadata() const
{
    return {
        u"V2Ray Core Plugin"_qs,                 //
        u"Moody"_qs,                             //
        PluginId{ u"builtin_v2ray_support"_qs }, //
        u"Basic V2Ray kernel support"_qs,        //
        u""_qs,                                  //
        { COMPONENT_KERNEL, COMPONENT_GUI },     //
    };
}

bool BuiltinV2RayCorePlugin::InitializePlugin()
{
    m_KernelInterface = std::make_shared<V2RayKernelInterface>();
    m_GUIInterface = new GuiInterface;
    return true;
}

void BuiltinV2RayCorePlugin::SettingsUpdated()
{
    settings.loadJson(m_Settings);
}

QList<KernelFactory> V2RayKernelInterface::PluginKernels() const
{
    QList<Qv2rayPlugin::Kernel::KernelFactory> factories;
    {
        Qv2rayPlugin::Kernel::KernelFactory v2ray;
        v2ray.Capabilities.setFlag(Qv2rayPlugin::Kernel::KERNELCAP_ROUTER);
        v2ray.Id = v2ray_kernel_id;
        v2ray.Name = u"V2Ray"_qs;
        v2ray.Create = std::function{ []() { return std::make_unique<V2RayKernel>(); } };
        v2ray.SupportedProtocols << u"blackhole"_qs << u"dns"_qs << u"freedom"_qs     //
                                 << u"http"_qs << u"loopback"_qs << u"shadowsocks"_qs //
                                 << u"socks"_qs << u"trojan"_qs << u"vless"_qs        //
                                 << u"vmess"_qs;
        factories << v2ray;
    }
    {
        Qv2rayPlugin::Kernel::KernelFactory v2raygo;
        v2raygo.Capabilities.setFlag(Qv2rayPlugin::Kernel::KERNELCAP_ROUTER);
        v2raygo.Id = v2ray_go_kernel_id;
        v2raygo.Name = u"V2Ray Go"_qs;
        v2raygo.Create = std::function{ []() { return std::make_unique<V2RayGoKernel>(); } };
        v2raygo.SupportedProtocols << u"blackhole"_qs << u"dns"_qs << u"freedom"_qs     //
                                   << u"http"_qs << u"loopback"_qs << u"shadowsocks"_qs //
                                   << u"socks"_qs << u"trojan"_qs << u"vless"_qs        //
                                   << u"vmess"_qs;
        factories << v2raygo;
    }
    {
        Qv2rayPlugin::Kernel::KernelFactory v2ray5;
        v2ray5.Capabilities.setFlag(Qv2rayPlugin::Kernel::KERNELCAP_ROUTER);
        v2ray5.Id = v2ray_5_kernel_id;
        v2ray5.Name = u"V2Ray v5"_qs;
        v2ray5.Create = std::function{ []() { return std::make_unique<V2Ray5Kernel>(); } };
        v2ray5.SupportedProtocols << u"blackhole"_qs << u"dns"_qs << u"freedom"_qs     //
                                  << u"http"_qs << u"loopback"_qs << u"shadowsocks"_qs //
                                  << u"socks"_qs << u"trojan"_qs << u"vless"_qs        //
                                  << u"vmess"_qs;
        factories << v2ray5;
    }
    return factories;
}

QIcon GuiInterface::Icon() const
{
    return QIcon(u":/qv2ray.png"_qs);
}

QList<PLUGIN_GUI_COMPONENT_TYPE> GuiInterface::GetComponents() const
{
    return { GUI_COMPONENT_SETTINGS };
}

std::unique_ptr<Gui::PluginSettingsWidget> GuiInterface::GetSettingsWidget() const
{
    return std::make_unique<V2RayKernelSettings>();
}

Gui::Qv2rayGUIInterface::PluginEditorDescriptor GuiInterface::GetInboundEditors() const
{
    return {};
}

Gui::Qv2rayGUIInterface::PluginEditorDescriptor GuiInterface::GetOutboundEditors() const
{
    return {};
}

std::unique_ptr<Gui::PluginMainWindowWidget> GuiInterface::GetMainWindowWidget() const
{
    return nullptr;
}
