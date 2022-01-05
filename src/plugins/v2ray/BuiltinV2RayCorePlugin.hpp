#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "QvPlugin/PluginInterface.hpp"
#include "common/SettingsModels.hpp"

#include <QtPlugin>

using namespace Qv2rayPlugin;

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

class BuiltinV2RayCorePlugin
    : public QObject
    , public Qv2rayInterface<BuiltinV2RayCorePlugin>
{
    Q_OBJECT
    QV2RAY_PLUGIN(BuiltinV2RayCorePlugin)

  public:
    V2RayCorePluginSettings settings;

    const QvPluginMetadata GetMetadata() const override;
    bool InitializePlugin() override;
    void SettingsUpdated() override;
};
