#include "BuiltinV2RayCorePlugin.hpp"

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "core/V2RayGoKernel.hpp"
#include "core/V2RayKernel.hpp"
#include "ui/w_V2RayKernelSettings.hpp"

class V2RayKernelInterface : public Qv2rayPlugin::Kernel::IKernelHandler
{
  public:
    V2RayKernelInterface() : Qv2rayPlugin::Kernel::IKernelHandler(){};
    virtual QList<Qv2rayPlugin::Kernel::KernelFactory> PluginKernels() const override
    {
        QList<Qv2rayPlugin::Kernel::KernelFactory> factories;
        {
            Qv2rayPlugin::Kernel::KernelFactory v2ray;
            v2ray.Capabilities.setFlag(Qv2rayPlugin::Kernel::KERNELCAP_ROUTER);
            v2ray.Id = v2ray_kernel_id;
            v2ray.Name = u"V2Ray"_qs;
            v2ray.Create = std::function{ []() { return std::make_unique<V2RayKernel>(); } };
            v2ray.SupportedProtocols << u"blackhole"_qs   //
                                     << u"dns"_qs         //
                                     << u"freedom"_qs     //
                                     << u"http"_qs        //
                                     << u"loopback"_qs    //
                                     << u"shadowsocks"_qs //
                                     << u"socks"_qs       //
                                     << u"trojan"_qs      //
                                     << u"vless"_qs       //
                                     << u"vmess"_qs;
            factories << v2ray;
        }
        {
            Qv2rayPlugin::Kernel::KernelFactory v2raygo;
            v2raygo.Capabilities.setFlag(Qv2rayPlugin::Kernel::KERNELCAP_ROUTER);
            v2raygo.Id = v2ray_go_kernel_id;
            v2raygo.Name = u"V2Ray Go"_qs;
            v2raygo.Create = std::function{ []() { return std::make_unique<V2RayGoKernel>(); } };
            v2raygo.SupportedProtocols << u"blackhole"_qs   //
                                       << u"dns"_qs         //
                                       << u"freedom"_qs     //
                                       << u"http"_qs        //
                                       << u"loopback"_qs    //
                                       << u"shadowsocks"_qs //
                                       << u"socks"_qs       //
                                       << u"trojan"_qs      //
                                       << u"vless"_qs       //
                                       << u"vmess"_qs;
            factories << v2raygo;
        }
        return factories;
    }
};

class GuiInterface : public Qv2rayPlugin::Gui::Qv2rayGUIInterface
{
  public:
    GuiInterface() = default;

    // PluginGUIInterface interface
  public:
    virtual QIcon Icon() const override
    {
        return QIcon(u":/qv2ray.png"_qs);
    }
    virtual QList<PLUGIN_GUI_COMPONENT_TYPE> GetComponents() const override
    {
        return { GUI_COMPONENT_SETTINGS };
    }

  protected:
    virtual std::unique_ptr<Gui::PluginSettingsWidget> GetSettingsWidget() const override
    {
        return std::make_unique<V2RayKernelSettings>();
    }
    virtual PluginEditorDescriptor GetInboundEditors() const override
    {
        return {};
    }
    virtual PluginEditorDescriptor GetOutboundEditors() const override
    {
        return {};
    }
    virtual std::unique_ptr<Gui::PluginMainWindowWidget> GetMainWindowWidget() const override
    {
        return nullptr;
    }

  private:
    Q_DISABLE_COPY(GuiInterface)
};

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
