#pragma once
#include "components/plugins/interface/QvPluginInterface.hpp"

#include <QMap>
#include <QObject>
#include <memory>

class QPluginLoader;

namespace Qv2ray::components::plugins
{
    struct QvPluginInfo
    {
        bool isLoaded = false;
        QString libraryPath;
        QString errorMessage;
        QPluginLoader *pluginLoader;
        Qv2rayInterface *pluginInterface;
    };

    class QvPluginHost : public QObject
    {
        Q_OBJECT
      public:
        explicit QvPluginHost(QObject *parent = nullptr);
        ~QvPluginHost();
        void InitializePluginHost();
        bool GetPluginEnableState(const QString &internalName) const;
        void SetPluginEnableState(const QString &internalName, bool isEnabled);
        const QString GetPluginTypeString(const QString &internalName) const;
        const QString GetPluginHookTypeString(const QString &internalName) const;
        const QStringList AvailablePlugins() const
        {
            return plugins.keys();
        }
        const inline QvPluginInfo GetPluginInfo(const QString &internalName) const
        {
            return plugins.value(internalName);
        }
        void SendHook(QV2RAY_PLUGIN_HOOK_TYPE type, QV2RAY_PLUGIN_HOOK_SUBTYPE subtype, QVariant &data);

      private slots:
        void QvPluginLog(const QString &log);
        void QvPluginMessageBox(const QString &message);

      private:
        int RefreshPluginList();
        bool InitializePlugin(const QString &internalName);
        void ClearPlugins();
        // Internal name, plugin info
        QMap<QString, QvPluginInfo> plugins;
    };

    inline ::Qv2ray::components::plugins::QvPluginHost *PluginHost = nullptr;
} // namespace Qv2ray::components::plugins

using namespace Qv2ray::components::plugins;
