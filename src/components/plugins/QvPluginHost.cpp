#include "QvPluginHost.hpp"

#include "base/Qv2rayBase.hpp"
#include "base/Qv2rayLog.hpp"
#include "core/settings/SettingsBackend.hpp"

#include <QPluginLoader>
namespace Qv2ray::components::plugins
{
    QvPluginHost::QvPluginHost(QObject *parent) : QObject(parent)
    {
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
                if (plugins.contains(info.pluginInterface->InternalName()))
                {
                    LOG(MODULE_PLUGINHOST,
                        "Found another plugin with the same internal name: " + info.pluginInterface->InternalName() + ". Skipped")
                    continue;
                }

                if (info.pluginInterface->QvPluginInterfaceVersion != QV2RAY_PLUGIN_INTERFACE_VERSION)
                {
                    // The plugin was built for a not-compactable version of Qv2ray. Don't load the plugin by default.
                    LOG(MODULE_PLUGINHOST, "The plugin " + info.pluginInterface->InternalName() +
                                               " is not loaded since it was built against a different version of interface")
                    info.errorMessage = tr("This plugin was built against an incompactable version of Qv2ray Plugin Interface.") + NEWLINE +
                                        QObject::tr("Please contact the plugin provider or report the issue to Qv2ray Workgroup.");
                }

                connect(info.pluginInterface->GetQObject(), SIGNAL(PluginLog(const QString &)), this, SLOT(QvPluginLog(const QString &)));
                LOG(MODULE_PLUGINHOST,
                    "Loaded plugin: \"" + info.pluginInterface->Name() + "\" made by: \"" + info.pluginInterface->Author() + "\"")
                plugins.insert(info.pluginInterface->InternalName(), info);
            }
        }
        return plugins.count();
    }

    const QString QvPluginHost::GetPluginTypeString(const QString &internalName) const
    {
        switch (plugins.value(internalName).pluginInterface->SpecialPluginType())
        {
            case SPECIAL_TYPE_NONE: return tr("No Special Type");
            case SPECIAL_TYPE_KERNEL: return tr("Connection Kernel");
            case SPECIAL_TYPE_GENERATION: return tr("Final Configuration Parser");
            case SPECIAL_TYPE_SERIALIZATION: return tr("Connection String Serializer/Deserializer");
            default: return tr("Unknown/unsupported plugin type.");
        }
    }

    const QString QvPluginHost::GetPluginHookTypeString(const QString &internalName) const
    {
        switch (plugins.value(internalName).pluginInterface->PluginHooks())
        {
            //
        }
        return "";
    }

    void QvPluginHost::QvPluginLog(const QString &log)
    {
        auto _sender = sender();
        if (auto _interface = qobject_cast<Qv2rayInterface *>(_sender); _interface)
        {
            LOG(MODULE_PLUGINCLIENT + "-" + _interface->InternalName(), log)
        }
        else
        {
            LOG(MODULE_PLUGINHOST, "UNKNOWN CLIENT: " + log)
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
            DEBUG(MODULE_PLUGINHOST, "Unloading: \"" + plugin.pluginInterface->Name() + "\"")
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
            // If not contained, default to enable.
            GlobalConfig.pluginConfig.pluginStates[internalName] = true;
        }
        // If the plugin is disabled
        if (!GlobalConfig.pluginConfig.pluginStates[internalName])
        {
            LOG(MODULE_PLUGINHOST, "Cannot load a plugin that's been disabled.")
            return false;
        }

        // TODO: Load plugin settings.
        plugins[internalName].pluginInterface->InitializePlugin({});
        plugins[internalName].isLoaded = true;
        return true;
    }

    QvPluginHost::~QvPluginHost()
    {
        ClearPlugins();
    }

} // namespace Qv2ray::components::plugins
