#pragma once
#include "src/plugin-interface/QvPluginInterface.hpp"

#include <QMap>
#include <QObject>
#include <memory>

class QPluginLoader;

using namespace Qv2rayPlugin;
namespace Qv2ray::components::plugins
{
    struct QvPluginInfo
    {
        bool isLoaded = false;
        QString libraryPath;
        QvPluginMetadata metadata;
        QPluginLoader *pluginLoader;
        Qv2rayInterface *pluginInterface;
    };

    class QvPluginHost : public QObject
    {
        Q_OBJECT
      public:
        explicit QvPluginHost(QObject *parent = nullptr);
        ~QvPluginHost();
        //
        bool GetPluginEnableState(const QString &internalName) const;
        void SetPluginEnableState(const QString &internalName, bool isEnabled);
        void SavePluginSettings() const;
        //
        bool inline GetPluginLoadState(const QString &internalName) const
        {
            return plugins.value(internalName).isLoaded;
        }
        const inline QString GetPluginLibraryPath(const QString &internalName) const
        {
            return plugins.value(internalName).libraryPath;
        }
        const inline QStringList AvailablePlugins() const
        {
            return plugins.keys();
        }
        inline std::unique_ptr<QWidget> GetPluginSettingsWidget(const QString &internalName) const
        {
            return plugins.value(internalName).pluginInterface->GetSettingsWidget();
        }
        const inline QJsonObject GetPluginSettings(const QString &internalName) const
        {
            return plugins.value(internalName).pluginInterface->GetSettngs();
        }
        bool inline SetPluginSettings(const QString &internalName, const QJsonObject &settings) const
        {
            return plugins.value(internalName).pluginInterface->UpdateSettings(settings);
        }
        const inline QvPluginMetadata GetPluginMetadata(const QString &internalName) const
        {
            return plugins.value(internalName).metadata;
        }
        const QMap<QString, QList<QString>> GetPluginKernels() const;
        const std::unique_ptr<QvPluginKernel> CreatePluginKernel(const QString &pluginInternalName) const;
        //
        const QList<std::tuple<QString, QString, QJsonObject>> TryDeserializeShareLink(const QString &sharelink, //
                                                                                       QString *aliasPrefix,     //
                                                                                       QString *errMessage,      //
                                                                                       QString *newGroupName,    //
                                                                                       bool *status) const;
        //
        const QString TrySerializeShareLink(const QString &protocol,             //
                                            const QJsonObject &outboundSettings, //
                                            const QString &alias,                //
                                            const QString &groupName,            //
                                            bool *status) const;
        const QvPluginOutboundInfoObject TryGetOutboundInfo(const QString &protocol, const QJsonObject &o, bool *status) const;
        const QList<QvPluginEditor *> GetOutboundEditorWidgets() const;
        //
        void Send_ConnectionStatsEvent(const Events::ConnectionStats::EventObject &object);
        void Send_ConnectivityEvent(const Events::Connectivity::EventObject &object);
        void Send_ConnectionEvent(const Events::ConnectionEntry::EventObject &object);
        void Send_SystemProxyEvent(const Events::SystemProxy::EventObject &object);
        //
      private slots:
        void QvPluginLog(const QString &log);
        void QvPluginMessageBox(const QString &message);

      private:
        void InitializePluginHost();
        int RefreshPluginList();
        bool InitializePlugin(const QString &internalName);
        void ClearPlugins();
        // Internal name, plugin info
        QHash<QString, QvPluginInfo> plugins;
    };

    const QString GetPluginTypeString(const SPECIAL_TYPE_FLAGS &types);
    const QString GetPluginCapabilityString(const CAPABILITY_FLAGS &caps);
    inline ::Qv2ray::components::plugins::QvPluginHost *PluginHost = nullptr;
} // namespace Qv2ray::components::plugins

using namespace Qv2ray::components::plugins;
