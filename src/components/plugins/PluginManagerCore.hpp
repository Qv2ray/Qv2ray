#pragma once

#include "src/plugin-interface/QvPluginInterface.hpp"

#include <QObject>

class QPluginLoader;

namespace Qv2ray::components::plugins
{
    struct QvPluginInfo
    {
        QString libraryPath;
        QPluginLoader *loader;
        Qv2rayPlugin::Qv2rayInterface *pinterface;
        bool isEnabled() const;
        void setEnabled(bool) const;
        Qv2rayPlugin::QvPluginMetadata metadata() const
        {
            return pinterface->GetMetadata();
        }
        bool hasComponent(Qv2rayPlugin::QV2RAY_PLUGIN_COMPONENT_TYPE t) const
        {
            return metadata().Components.contains(t);
        }
    };

    class PluginManagerCore : public QObject
    {
        Q_OBJECT
      public:
        PluginManagerCore(QObject *parent = nullptr);
        virtual ~PluginManagerCore();
        void LoadPlugins();

        QList<const QvPluginInfo *> GetPlugins(Qv2rayPlugin::QV2RAY_PLUGIN_COMPONENT_TYPE c) const;
        const QList<const QvPluginInfo *> AllPlugins() const;
        const QvPluginInfo *GetPlugin(const QString &internalName)
        {
            return plugins.contains(internalName) ? &plugins[internalName] : nullptr;
        }

        bool IsPluginEnabled(const QString &internalName) const;
        void SetPluginEnabled(const QString &internalName, bool isEnabled);

      private:
        bool tryLoadPlugin(const QString &pluginFullPath);
        void SavePluginSettings() const;

      private slots:
        void QvPluginLog(QString log);
        void QvPluginMessageBox(QString title, QString message);

      private:
        // Internal name, plugin info
        QHash<QString, QvPluginInfo> plugins;
    };
} // namespace Qv2ray::components::plugins

using namespace Qv2ray::components::plugins;
