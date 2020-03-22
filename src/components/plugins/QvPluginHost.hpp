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
        bool isLoaded;
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
        int RefreshPluginList();
        QStringList AvailablePlugins();
        void InitializePluginHost();
        const inline QvPluginInfo GetPluginInfo(const QString &internalName)
        {
            return plugins.value(internalName);
        }

      private slots:
        void QvPluginLog(const QString &log);

      private:
        // Internal name, plugin info
        QMap<QString, QvPluginInfo> plugins;
        void ClearPlugins();
    };

    inline ::Qv2ray::components::plugins::QvPluginHost *PluginHost = nullptr;
} // namespace Qv2ray::components::plugins

using namespace Qv2ray::components::plugins;
