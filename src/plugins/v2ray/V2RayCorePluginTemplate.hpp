#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "QvPlugin/PluginInterface.hpp"
#include "common/SettingsModels.hpp"

#include <QtPlugin>

using namespace Qv2rayPlugin;

#define CORETYPE_V2Ray 0x01
#define CORETYPE_V2RayGo 0x02
#define CORETYPE_V2Ray5 0x03

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
