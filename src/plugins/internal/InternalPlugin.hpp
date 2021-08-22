#pragma once
#include "QvPlugin/PluginInterface.hpp"

#ifndef QT_STATICPLUGIN
#define QT_STATICPLUGIN
#endif

#include <QtPlugin>

class Qv2rayInternalPlugin
    : public QObject
    , public Qv2rayPlugin::Qv2rayInterface<Qv2rayInternalPlugin>
{
    Q_OBJECT
    QV2RAY_PLUGIN(Qv2rayInternalPlugin)

  public:
    virtual const Qv2rayPlugin::QvPluginMetadata GetMetadata() const override;
    virtual bool InitializePlugin() override;
    virtual void SettingsUpdated() override;
};
