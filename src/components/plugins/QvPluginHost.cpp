#include "QvPluginHost.hpp"

#include "base/Qv2rayBase.hpp"
#include "base/Qv2rayLog.hpp"
#include "common/QvHelpers.hpp"
#include "core/settings/SettingsBackend.hpp"

#include <QPluginLoader>

namespace Qv2ray::components::plugins
{
    QvPluginHost::QvPluginHost(QObject *parent) : QObject(parent)
    {
        if (auto dir = QDir(QV2RAY_PLUGIN_SETTINGS_DIR); !dir.exists())
        {
            dir.mkpath(QV2RAY_PLUGIN_SETTINGS_DIR);
        }
    }

    int QvPluginHost::RefreshPluginList()
    {
        ClearPlugins();
        LOG(MODULE_PLUGINHOST, "Reloading plugin list")
        for (const auto &pluginDirPath : Qv2rayAssetsPaths("plugins"))
        {
            const QStringList entries = QDir(pluginDirPath).entryList(QDir::Files);
            for (const auto &fileName : entries)
            {
                DEBUG(MODULE_PLUGINHOST, "Loading plugin: " + fileName + " from: " + pluginDirPath)
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
                    LOG(MODULE_PLUGINHOST, info.pluginLoader->errorString());
                    continue;
                }
                info.pluginInterface = qobject_cast<Qv2rayInterface *>(plugin);
                if (info.pluginInterface == nullptr)
                {
                    // info.errorMessage = tr("Failed to cast from QObject to Qv2rayPluginInterface");
                    LOG(MODULE_PLUGINHOST, "Failed to cast from QObject to Qv2rayPluginInterface")
                    info.pluginLoader->unload();
                    continue;
                }
                info.metadata = info.pluginInterface->GetMetadata();
                if (plugins.contains(info.metadata.InternalName))
                {
                    LOG(MODULE_PLUGINHOST, "Found another plugin with the same internal name: " + info.metadata.InternalName + ". Skipped")
                    continue;
                }

                if (info.pluginInterface->QvPluginInterfaceVersion != QV2RAY_PLUGIN_INTERFACE_VERSION)
                {
                    // The plugin was built for a not-compactable version of Qv2ray. Don't load the plugin by default.
                    LOG(MODULE_PLUGINHOST, "The plugin " + info.metadata.InternalName +
                                               " is not loaded since it was built against a different version of interface")
                    info.errorMessage = tr("This plugin was built against an incompactable version of Qv2ray Plugin Interface.") + NEWLINE +
                                        QObject::tr("Please contact the plugin provider or report the issue to Qv2ray Workgroup.");
                }
                connect(plugin, SIGNAL(PluginLog(const QString &)), this, SLOT(QvPluginLog(const QString &)));
                connect(plugin, SIGNAL(PluginErrorMessageBox(const QString &)), this, SLOT(QvPluginMessageBox(const QString &)));
                LOG(MODULE_PLUGINHOST, "Loaded plugin: \"" + info.metadata.Name + "\" made by: \"" + info.metadata.Author + "\"")
                plugins.insert(info.metadata.InternalName, info);
            }
        }
        return plugins.count();
    }

    const QString QvPluginHost::GetPluginTypeString(const QString &internalName) const
    {
        QStringList types;
        for (auto type : plugins.value(internalName).metadata.SpecialPluginType)
        {
            switch (type)
            {
                case SPECIAL_TYPE_NONE: types << tr("No Special Type"); break;
                case SPECIAL_TYPE_KERNEL: types << tr("Connection Kernel"); break;
                case SPECIAL_TYPE_SERIALIZOR: types << tr("Connection String Serializer/Deserializer"); break;
                default: types << tr("Unknown/unsupported plugin type."); break;
            }
        }
        return types.join("; ");
    }

    const QString QvPluginHost::GetPluginHookTypeString(const QString &internalName) const
    {
        QStringList hooks;
        for (auto hook : plugins.value(internalName).metadata.Capabilities)
        {
            switch (hook)
            {
                case CAPABILITY_NONE: hooks << tr("No hook"); break;
                case CAPABILITY_CONNECTIVITY: hooks << tr("Connection State Change"); break;
                case CAPABILITY_CONNECTION_ENTRY: hooks << tr("Connection Change"); break;
                case CAPABILITY_STATS: hooks << tr("Statistics Event"); break;
                case CAPABILITY_SYSTEM_PROXY: hooks << tr("System Proxy"); break;
                default: hooks << tr("Unknown/unsupported hook type."); break;
            }
        }
        return hooks.join("; ");
    }

    void QvPluginHost::QvPluginLog(const QString &log)
    {
        auto _sender = sender();
        if (auto _interface = qobject_cast<Qv2rayInterface *>(_sender); _interface)
        {
            LOG(MODULE_PLUGINCLIENT + "-" + _interface->GetMetadata().InternalName, log)
        }
        else
        {
            LOG(MODULE_PLUGINHOST, "UNKNOWN CLIENT: " + log)
        }
    }

    void QvPluginHost::QvPluginMessageBox(const QString &msg)
    {
        auto _sender = sender();
        if (auto _interface = qobject_cast<Qv2rayInterface *>(_sender); _interface)
        {
            QvMessageBoxWarn(nullptr, _interface->GetMetadata().Name, msg);
        }
        else
        {
            QvMessageBoxWarn(nullptr, "Unknown Plugin", msg);
        }
    }

    bool QvPluginHost::GetPluginEnableState(const QString &internalName) const
    {
        return GlobalConfig.pluginConfig.pluginStates[internalName];
    }

    void QvPluginHost::SetPluginEnableState(const QString &internalName, bool isEnabled)
    {
        LOG(MODULE_PLUGINHOST, "Set plugin: \"" + internalName + "\" enable state: " + (isEnabled ? "true" : "false"))
        GlobalConfig.pluginConfig.pluginStates[internalName] = isEnabled;
        if (isEnabled && !plugins[internalName].isLoaded)
        {
            // Load plugin if it haven't been loaded.
            InitializePlugin(internalName);
        }
    }

    void QvPluginHost::InitializePluginHost()
    {
        RefreshPluginList();
        for (auto &plugin : plugins.keys())
        {
            InitializePlugin(plugin);
        }
    }

    void QvPluginHost::ClearPlugins()
    {
        for (auto &&plugin : plugins)
        {
            DEBUG(MODULE_PLUGINHOST, "Unloading: \"" + plugin.metadata.Name + "\"")
            plugin.pluginLoader->unload();
            plugin.pluginLoader->deleteLater();
        }
        plugins.clear();
    }
    bool QvPluginHost::InitializePlugin(const QString &internalName)
    {
        auto &plugin = plugins[internalName];
        if (plugin.isLoaded)
        {
            LOG(MODULE_PLUGINHOST, "The plugin: \"" + internalName + "\" has already been loaded.")
            return true;
        }
        if (!GlobalConfig.pluginConfig.pluginStates.contains(internalName))
        {
            // If not contained, default to disable.
            GlobalConfig.pluginConfig.pluginStates[internalName] = false;
        }
        // If the plugin is disabled
        if (!GlobalConfig.pluginConfig.pluginStates[internalName])
        {
            LOG(MODULE_PLUGINHOST, "Cannot load a plugin that's been disabled.")
            return false;
        }

        auto conf = JsonFromString(StringFromFile(QV2RAY_PLUGIN_SETTINGS_DIR + internalName + ".conf"));
        plugins[internalName].pluginInterface->Initialize(QV2RAY_PLUGIN_SETTINGS_DIR + internalName + "/", conf);
        plugins[internalName].isLoaded = true;
        return true;
    }

    QvPluginHost::~QvPluginHost()
    {
        for (auto name : plugins.keys())
        {
            LOG(MODULE_PLUGINHOST, "Saving plugin settings for: \"" + name + "\"")
            auto &conf = plugins[name].pluginInterface->GetSettngs();
            StringToFile(JsonToString(conf), QV2RAY_PLUGIN_SETTINGS_DIR + name + ".conf");
        }
        ClearPlugins();
    }

    // ================== BEGIN SEND EVENTS ==================
    void QvPluginHost::Send_ConnectionStatsEvent(const QvConnectionStatsEventObject &object)
    {
        for (auto &plugin : plugins)
        {
            if (plugin.metadata.Capabilities.contains(CAPABILITY_STATS))
            {
                plugin.pluginInterface->GetEventHandler()->ProcessEvent_ConnectionStats(object);
            }
        }
    }
    void QvPluginHost::Send_ConnectivityEvent(const QvConnectivityEventObject &object)
    {
        for (auto &plugin : plugins)
        {
            if (plugin.metadata.Capabilities.contains(CAPABILITY_CONNECTIVITY))
            {
                plugin.pluginInterface->GetEventHandler()->ProcessEvent_Connectivity(object);
            }
        }
    }
    void QvPluginHost::Send_ConnectionEvent(const QvConnectionEntryEventObject &object)
    {
        for (auto &plugin : plugins)
        {
            if (plugin.metadata.Capabilities.contains(CAPABILITY_CONNECTION_ENTRY))
            {
                plugin.pluginInterface->GetEventHandler()->ProcessEvent_ConnectionEntry(object);
            }
        }
    }
    void QvPluginHost::Send_SystemProxyEvent(const QvSystemProxyEventObject &object)
    {
        for (auto &plugin : plugins)
        {
            if (plugin.metadata.Capabilities.contains(CAPABILITY_SYSTEM_PROXY))
            {
                plugin.pluginInterface->GetEventHandler()->ProcessEvent_SystemProxy(object);
            }
        }
    }
} // namespace Qv2ray::components::plugins
