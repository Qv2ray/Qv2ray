#pragma once
#include "src/plugin-interface/QvPluginInterface.hpp"

#include <QHash>
#include <QMap>
#include <QObject>
#include <memory>

class QPluginLoader;

using namespace Qv2rayPlugin;
namespace Qv2ray::components::plugins
{
    struct QvPluginInfo
    {
      public:
        bool isLoaded = false;
        QString libraryPath;
        QvPluginMetadata metadata;
        QPluginLoader *pluginLoader;
        Qv2rayInterface *pluginInterface;
        bool hasComponent(PluginComponentType t)
        {
            return metadata.Components.contains(t);
        }
    };

    class QvPluginHost : public QObject
    {
        Q_OBJECT
      public:
        explicit QvPluginHost(QObject *parent = nullptr);
        ~QvPluginHost();
        //
        bool GetPluginEnabled(const QString &internalName) const;
        void SetPluginEnabled(const QString &internalName, bool isEnabled);
        //
        void SavePluginSettings() const;
        //
        QvPluginInfo *GetPlugin(const QString &internalName)
        {
            return plugins.contains(internalName) ? &plugins[internalName] : nullptr;
        }
        const inline QStringList AllPlugins() const
        {
            return plugins.keys();
        }
        const inline QStringList UsablePlugins() const
        {
            QStringList result;
            for (const auto &pluginName : plugins.keys())
                if (shouldUsePlugin(pluginName))
                    result << pluginName;
            return result;
        }

        const QList<std::tuple<QString, QString, QJsonObject>> TryDeserializeShareLink(const QString &sharelink, //
                                                                                       QString *aliasPrefix,     //
                                                                                       QString *errMessage,      //
                                                                                       QString *newGroupName,    //
                                                                                       bool *status) const;
        const QString SerializeOutbound(const QString &protocol, const QJsonObject &out, const QString &name, const QString &group, bool *ok) const;
        const QMap<OutboundInfoFlags, QVariant> TryGetOutboundInfo(const QString &protocol, const QJsonObject &o, bool *status) const;
        //
        void Send_ConnectionStatsEvent(const Events::ConnectionStats::EventObject &object);
        void Send_ConnectivityEvent(const Events::Connectivity::EventObject &object);
        void Send_ConnectionEvent(const Events::ConnectionEntry::EventObject &object);
        void Send_SystemProxyEvent(const Events::SystemProxy::EventObject &object);
        //
      private slots:
        void QvPluginLog(const QString &log);
        void QvPluginMessageBox(const QString &title, const QString &message);

      private:
        bool shouldUsePlugin(const QString &internalName) const
        {
            return GetPluginEnabled(internalName) && plugins[internalName].isLoaded;
        }
        void initializePluginHost();
        int refreshPluginList();
        bool initializePlugin(const QString &internalName);
        void clearPlugins();
        // Internal name, plugin info
        QHash<QString, QvPluginInfo> plugins;
    };

    const QStringList GetPluginComponentsString(const QList<PluginGuiComponentType> &types);
    const QStringList GetPluginComponentsString(const QList<PluginComponentType> &types);
    inline ::Qv2ray::components::plugins::QvPluginHost *PluginHost = nullptr;

} // namespace Qv2ray::components::plugins
using namespace Qv2ray::components::plugins;
