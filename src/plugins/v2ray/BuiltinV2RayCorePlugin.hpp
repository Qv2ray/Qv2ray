#pragma once

#include "QvPlugin/PluginInterface.hpp"
#include "common/SettingsModels.hpp"

#include <QObject>
#include <QtPlugin>

using namespace Qv2rayPlugin;

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
