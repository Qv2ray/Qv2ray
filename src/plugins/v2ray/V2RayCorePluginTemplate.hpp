#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "QvPlugin/PluginInterface.hpp"
#include "common/SettingsModels.hpp"

#include <QtPlugin>

using namespace Qv2rayPlugin;

#define CORETYPE_V2Ray 0x01
#define CORETYPE_V2RaySN 0x02
#define CORETYPE_V2Ray5 0x03
#define CORETYPE_V2RayRust 0x04

#if !defined(V2RayCorePluginClass) || !defined(V2RayCoreType)
#error either V2RayCorePluginClass or V2RayCoreType is not defined.
#endif

class V2RayCorePluginClass
    : public QObject
    , public Qv2rayInterface<V2RayCorePluginClass>
{
    Q_OBJECT
    QV2RAY_PLUGIN(V2RayCorePluginClass)

  public:
    V2RayCorePluginSettings settings;

    const QvPluginMetadata GetMetadata() const override;
    bool InitializePlugin() override;
    void SettingsUpdated() override;
};

namespace V2RayPluginNamespace
{
    class V2RayKernelInterface : public Qv2rayPlugin::Kernel::IKernelHandler
    {
      public:
        V2RayKernelInterface() = default;

      public:
        virtual QList<Qv2rayPlugin::Kernel::KernelFactory> PluginKernels() const override;
        Q_DISABLE_COPY(V2RayKernelInterface)
    };

    class GuiInterface : public Qv2rayPlugin::Gui::Qv2rayGUIInterface
    {
      public:
        GuiInterface() = default;

      public:
        virtual QIcon Icon() const override;
        virtual QList<PLUGIN_GUI_COMPONENT_TYPE> GetComponents() const override;

      protected:
        virtual std::unique_ptr<Gui::PluginSettingsWidget> GetSettingsWidget() const override;
        virtual PluginEditorDescriptor GetInboundEditors() const override;
        virtual PluginEditorDescriptor GetOutboundEditors() const override;
        virtual std::unique_ptr<Gui::PluginMainWindowWidget> GetMainWindowWidget() const override;

      private:
        Q_DISABLE_COPY(GuiInterface)
    };
} // namespace V2RayPluginNamespace
