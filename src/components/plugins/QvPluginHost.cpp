#include "QvPluginHost.hpp"

#include "base/Qv2rayBase.hpp"
#include "base/Qv2rayLog.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "utils/QvHelpers.hpp"

#include <QPluginLoader>

#define QV_MODULE_NAME "PluginHost"
namespace Qv2ray::components::plugins
{
    QvPluginHost::QvPluginHost(QObject *parent) : QObject(parent)
    {
        if (!QvCoreApplication->StartupArguments.noPlugins)
        {
            if (auto dir = QDir(QV2RAY_PLUGIN_SETTINGS_DIR); !dir.exists())
            {
                dir.mkpath(QV2RAY_PLUGIN_SETTINGS_DIR);
            }
            initializePluginHost();
        }
        else
        {
            LOG("PluginHost initilization skipped by command line option.");
        }
    }

    int QvPluginHost::refreshPluginList()
    {
        clearPlugins();
        LOG("Reloading plugin list");
        for (const auto &pluginDirPath : QvCoreApplication->GetAssetsPaths("plugins"))
        {
            const QStringList entries = QDir(pluginDirPath).entryList(QDir::Files);
            for (const auto &fileName : entries)
            {
                if (!fileName.endsWith(QV2RAY_LIBRARY_SUFFIX))
                {
                    DEBUG("Skipping: " + fileName + " in: " + pluginDirPath);
                    continue;
                }
                DEBUG("Loading plugin: " + fileName + " from: " + pluginDirPath);
                //
                QvPluginInfo info;
                auto pluginFullPath = QDir(pluginDirPath).absoluteFilePath(fileName);
                info.libraryPath = pluginFullPath;
                info.pluginLoader = new QPluginLoader(pluginFullPath, this);
                // auto meta = pluginLoader.metaData();
                // You should not call "delete" on this object, it's handled by the QPluginLoader
                QObject *plugin = info.pluginLoader->instance();
                if (plugin == nullptr)
                {
                    const auto errMessage = info.pluginLoader->errorString();
                    LOG(errMessage);
                    QvMessageBoxWarn(nullptr, tr("Failed to load plugin"), errMessage);
                    continue;
                }
                info.pluginInterface = qobject_cast<Qv2rayInterface *>(plugin);
                if (info.pluginInterface == nullptr)
                {
                    LOG("Failed to cast from QObject to Qv2rayPluginInterface");
                    info.pluginLoader->unload();
                    continue;
                }

                if (info.pluginInterface->QvPluginInterfaceVersion != QV2RAY_PLUGIN_INTERFACE_VERSION)
                {
                    // The plugin was built for a not-compactable version of Qv2ray. Don't load the plugin by default.
                    LOG(info.libraryPath + " is built with an older Interface, ignoring");
                    QvMessageBoxWarn(nullptr, tr("Cannot load plugin"),
                                     tr("The plugin cannot be loaded: ") + NEWLINE + info.libraryPath + NEWLINE NEWLINE +
                                         tr("This plugin was built against a different version of the Plugin Interface.") + NEWLINE +
                                         tr("Please contact the plugin provider or report the issue to Qv2ray Workgroup."));
                    info.pluginLoader->unload();
                    continue;
                }
                info.metadata = info.pluginInterface->GetMetadata();
                if (plugins.contains(info.metadata.InternalName))
                {
                    LOG("Found another plugin with the same internal name: " + info.metadata.InternalName + ". Skipped");
                    continue;
                }
                connect(plugin, SIGNAL(PluginLog(const QString &)), this, SLOT(QvPluginLog(const QString &)));
                connect(plugin, SIGNAL(PluginErrorMessageBox(const QString &, const QString &)), this,
                        SLOT(QvPluginMessageBox(const QString &, const QString &)));
                LOG("Loaded plugin: \"" + info.metadata.Name + "\" made by: \"" + info.metadata.Author + "\"");
                plugins.insert(info.metadata.InternalName, info);
            }
        }
        return plugins.count();
    }

    void QvPluginHost::QvPluginLog(const QString &log)
    {
        auto _sender = sender();
        if (auto _interface = qobject_cast<Qv2rayInterface *>(_sender); _interface)
        {
            LOG(_interface->GetMetadata().InternalName, log);
        }
        else
        {
            LOG("UNKNOWN CLIENT: " + log);
        }
    }

    void QvPluginHost::QvPluginMessageBox(const QString &title, const QString &message)
    {
        const auto _sender = sender();
        const auto _interface = qobject_cast<Qv2rayInterface *>(_sender);
        if (_interface)
            QvMessageBoxWarn(nullptr, _interface->GetMetadata().Name + " - " + title, message);
        else
            QvMessageBoxWarn(nullptr, "Unknown Plugin - " + title, message);
    }

    bool QvPluginHost::GetPluginEnabled(const QString &internalName) const
    {
        return GlobalConfig.pluginConfig.pluginStates[internalName];
    }

    void QvPluginHost::SetPluginEnabled(const QString &internalName, bool isEnabled)
    {
        LOG("Set plugin: \"" + internalName + "\" enable state: " + (isEnabled ? "true" : "false"));
        GlobalConfig.pluginConfig.pluginStates[internalName] = isEnabled;
        if (isEnabled && !plugins[internalName].isLoaded)
        {
            // Load plugin if it haven't been loaded.
            initializePlugin(internalName);
            QvMessageBoxInfo(nullptr, tr("Enabling a plugin"), tr("The plugin will become fully functional after restarting Qv2ray."));
        }
    }

    void QvPluginHost::initializePluginHost()
    {
        refreshPluginList();
        for (auto &plugin : plugins.keys())
        {
            initializePlugin(plugin);
        }
    }

    void QvPluginHost::clearPlugins()
    {
        for (auto &&plugin : plugins)
        {
            DEBUG("Unloading: \"" + plugin.metadata.Name + "\"");
            plugin.pluginLoader->unload();
            plugin.pluginLoader->deleteLater();
        }
        plugins.clear();
    }
    bool QvPluginHost::initializePlugin(const QString &internalName)
    {
        const auto &plugin = plugins[internalName];
        if (plugin.isLoaded)
        {
            LOG("The plugin: \"" + internalName + "\" has already been loaded.");
            return true;
        }
        if (!GlobalConfig.pluginConfig.pluginStates.contains(internalName))
        {
            // If not contained, default to enabled.
            GlobalConfig.pluginConfig.pluginStates[internalName] = true;
        }
        // If the plugin is disabled
        if (!GlobalConfig.pluginConfig.pluginStates[internalName])
        {
            LOG("Cannot load a plugin that's been disabled.");
            return false;
        }

        auto conf = JsonFromString(StringFromFile(QV2RAY_PLUGIN_SETTINGS_DIR + internalName + ".conf"));
        plugins[internalName].pluginInterface->InitializePlugin(QV2RAY_PLUGIN_SETTINGS_DIR + internalName + "/", conf);
        plugins[internalName].isLoaded = true;
        return true;
    }

    void QvPluginHost::SavePluginSettings() const
    {
        for (const auto &name : plugins.keys())
        {
            if (plugins[name].isLoaded)
            {
                LOG("Saving plugin settings for: \"" + name + "\"");
                auto &conf = plugins[name].pluginInterface->GetSettngs();
                StringToFile(JsonToString(conf), QV2RAY_PLUGIN_SETTINGS_DIR + name + ".conf");
            }
        }
    }

    QvPluginHost::~QvPluginHost()
    {
        SavePluginSettings();
        clearPlugins();
    }

    // ================== BEGIN SEND EVENTS ==================
    void QvPluginHost::SendEvent(const Events::ConnectionStats::EventObject &object)
    {
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.Components.contains(COMPONENT_EVENT_HANDLER))
                plugin.pluginInterface->GetEventHandler()->ProcessEvent_ConnectionStats(object);
        }
    }
    void QvPluginHost::SendEvent(const Events::Connectivity::EventObject &object)
    {
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.Components.contains(COMPONENT_EVENT_HANDLER))
                plugin.pluginInterface->GetEventHandler()->ProcessEvent_Connectivity(object);
        }
    }
    void QvPluginHost::SendEvent(const Events::ConnectionEntry::EventObject &object)
    {
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.Components.contains(COMPONENT_EVENT_HANDLER))
                plugin.pluginInterface->GetEventHandler()->ProcessEvent_ConnectionEntry(object);
        }
    }
    void QvPluginHost::SendEvent(const Events::SystemProxy::EventObject &object)
    {
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.Components.contains(COMPONENT_EVENT_HANDLER))
                plugin.pluginInterface->GetEventHandler()->ProcessEvent_SystemProxy(object);
        }
    }

    const QList<std::tuple<QString, QString, QJsonObject>> QvPluginHost::TryDeserializeShareLink(const QString &sharelink, //
                                                                                                 QString *aliasPrefix,     //
                                                                                                 QString *errMessage,      //
                                                                                                 QString *newGroupName,    //
                                                                                                 bool &ok) const
    {
        Q_UNUSED(newGroupName)
        QList<std::tuple<QString, QString, QJsonObject>> data;
        ok = false;
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.Components.contains(COMPONENT_OUTBOUND_HANDLER))
            {
                auto serializer = plugin.pluginInterface->GetOutboundHandler();
                bool thisPluginCanHandle = false;
                for (const auto &prefix : serializer->SupportedLinkPrefixes())
                {
                    thisPluginCanHandle = thisPluginCanHandle || sharelink.startsWith(prefix);
                }
                if (thisPluginCanHandle)
                {
                    // Populate Plugin Options
                    {
                        auto opt = plugin.pluginLoader->instance()->property(QV2RAY_PLUGIN_INTERNAL_PROPERTY_KEY).value<Qv2rayPluginOption>();
                        opt[OPTION_SET_TLS_DISABLE_SYSTEM_CERTS] = GlobalConfig.advancedConfig.disableSystemRoot;
                        plugin.pluginLoader->instance()->setProperty(QV2RAY_PLUGIN_INTERNAL_PROPERTY_KEY, QVariant::fromValue(opt));
                    }
                    const auto &[protocol, outboundSettings] = serializer->DeserializeOutbound(sharelink, aliasPrefix, errMessage);
                    if (errMessage->isEmpty())
                    {
                        data << std::tuple{ *aliasPrefix, protocol, outboundSettings };
                        ok = true;
                    }
                    break;
                }
            }
        }
        return data;
    }

    const OutboundInfoObject QvPluginHost::GetOutboundInfo(const QString &protocol, const QJsonObject &o, bool &status) const
    {
        status = false;
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.Components.contains(COMPONENT_OUTBOUND_HANDLER))
            {
                auto serializer = plugin.pluginInterface->GetOutboundHandler();
                if (serializer && serializer->SupportedProtocols().contains(protocol))
                {
                    auto info = serializer->GetOutboundInfo(protocol, o);
                    status = true;
                    return info;
                }
            }
        }
        return {};
    }

    void QvPluginHost::SetOutboundInfo(const QString &protocol, const OutboundInfoObject &info, QJsonObject &o) const
    {
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.Components.contains(COMPONENT_OUTBOUND_HANDLER))
            {
                auto serializer = plugin.pluginInterface->GetOutboundHandler();
                if (serializer && serializer->SupportedProtocols().contains(protocol))
                {
                    serializer->SetOutboundInfo(protocol, info, o);
                }
            }
        }
    }

    const QString QvPluginHost::SerializeOutbound(const QString &protocol,           //
                                                  const QJsonObject &out,            //
                                                  const QJsonObject &streamSettings, //
                                                  const QString &name,               //
                                                  const QString &group,              //
                                                  bool *ok) const
    {
        *ok = false;
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.Components.contains(COMPONENT_OUTBOUND_HANDLER))
            {
                auto serializer = plugin.pluginInterface->GetOutboundHandler();
                if (serializer && serializer->SupportedProtocols().contains(protocol))
                {
                    auto link = serializer->SerializeOutbound(protocol, name, group, out, streamSettings);
                    *ok = true;
                    return link;
                }
            }
        }
        return "";
    }

    const QStringList GetPluginComponentsString(const QList<PluginGuiComponentType> &types)
    {
        QStringList typesList;
        if (types.isEmpty())
            typesList << QObject::tr("None");
        for (auto type : types)
        {
            switch (type)
            {
                case GUI_COMPONENT_SETTINGS: typesList << QObject::tr("Settings Widget"); break;
                case GUI_COMPONENT_INBOUND_EDITOR: typesList << QObject::tr("Inbound Editor"); break;
                case GUI_COMPONENT_OUTBOUND_EDITOR: typesList << QObject::tr("Outbound Editor"); break;
                case GUI_COMPONENT_MAINWINDOW_WIDGET: typesList << QObject::tr("MainWindow Widget"); break;
                default: typesList << QObject::tr("Unknown type."); break;
            }
        }
        return typesList;
    }

    const QStringList GetPluginComponentsString(const QList<PluginComponentType> &types)
    {
        QStringList typesList;
        if (types.isEmpty())
            typesList << QObject::tr("None");
        for (auto type : types)
        {
            switch (type)
            {
                case COMPONENT_KERNEL: typesList << QObject::tr("Kernel"); break;
                case COMPONENT_OUTBOUND_HANDLER: typesList << QObject::tr("Outbound Handler/Parser"); break;
                case COMPONENT_SUBSCRIPTION_ADAPTER: typesList << QObject::tr("Subscription Adapter"); break;
                case COMPONENT_EVENT_HANDLER: typesList << QObject::tr("Event Handler"); break;
                case COMPONENT_GUI: typesList << QObject::tr("GUI Components"); break;
                default: typesList << QObject::tr("Unknown type."); break;
            }
        }
        return typesList;
    }
} // namespace Qv2ray::components::plugins
