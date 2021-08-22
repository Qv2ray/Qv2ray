#include "BuiltinV2RayCorePlugin.hpp"

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "core/V2RayKernel.hpp"
#include "ui/w_V2RayKernelSettings.hpp"

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
