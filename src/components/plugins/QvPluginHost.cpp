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
        if (!StartupOption.noPlugins)
        {
            if (auto dir = QDir(QV2RAY_PLUGIN_SETTINGS_DIR); !dir.exists())
            {
                dir.mkpath(QV2RAY_PLUGIN_SETTINGS_DIR);
            }
            InitializePluginHost();
        }
        else
        {
            LOG(MODULE_PLUGINHOST, "PluginHost initilization skipped by command line option.")
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
                    LOG(MODULE_PLUGINHOST, info.metadata.InternalName + " is built with an older Interface, ignoring")
                    QvMessageBoxWarn(nullptr, tr("Cannot load plugin"),
                                     info.metadata.Name + " " + tr("cannot be loaded.") + NEWLINE NEWLINE +
                                         tr("This plugin was built against an older/newer version of the Plugin Interface.") + NEWLINE +
                                         tr("Please contact the plugin provider or report the issue to Qv2ray Workgroup."));
                    continue;
                }
                connect(plugin, SIGNAL(PluginLog(const QString &)), this, SLOT(QvPluginLog(const QString &)));
                connect(plugin, SIGNAL(PluginErrorMessageBox(const QString &)), this, SLOT(QvPluginMessageBox(const QString &)));
                LOG(MODULE_PLUGINHOST, "Loaded plugin: \"" + info.metadata.Name + "\" made by: \"" + info.metadata.Author + "\"")
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
            QvMessageBoxInfo(nullptr, tr("Enabling a plugin"), tr("The plugin will become fully functional after restarting Qv2ray."));
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
        const auto &plugin = plugins[internalName];
        if (plugin.isLoaded)
        {
            LOG(MODULE_PLUGINHOST, "The plugin: \"" + internalName + "\" has already been loaded.")
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
        for (const auto &name : plugins.keys())
        {
            if (plugins[name].isLoaded)
            {
                LOG(MODULE_PLUGINHOST, "Saving plugin settings for: \"" + name + "\"")
                auto &conf = plugins[name].pluginInterface->GetSettngs();
                StringToFile(JsonToString(conf), QV2RAY_PLUGIN_SETTINGS_DIR + name + ".conf");
            }
        }
        ClearPlugins();
    }

    // ================== BEGIN SEND EVENTS ==================
    void QvPluginHost::Send_ConnectionStatsEvent(const Events::ConnectionStats::EventObject &object)
    {
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.Capabilities.contains(CAPABILITY_STATS))
            {
                plugin.pluginInterface->GetEventHandler()->ProcessEvent_ConnectionStats(object);
            }
        }
    }
    void QvPluginHost::Send_ConnectivityEvent(const Events::Connectivity::EventObject &object)
    {
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.Capabilities.contains(CAPABILITY_CONNECTIVITY))
            {
                plugin.pluginInterface->GetEventHandler()->ProcessEvent_Connectivity(object);
            }
        }
    }
    void QvPluginHost::Send_ConnectionEvent(const Events::ConnectionEntry::EventObject &object)
    {
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.Capabilities.contains(CAPABILITY_CONNECTION_ENTRY))
            {
                plugin.pluginInterface->GetEventHandler()->ProcessEvent_ConnectionEntry(object);
            }
        }
    }
    void QvPluginHost::Send_SystemProxyEvent(const Events::SystemProxy::EventObject &object)
    {
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.Capabilities.contains(CAPABILITY_SYSTEM_PROXY))
            {
                plugin.pluginInterface->GetEventHandler()->ProcessEvent_SystemProxy(object);
            }
        }
    }

    const QList<QvPluginEditor *> QvPluginHost::GetOutboundEditorWidgets() const
    {
        QList<QvPluginEditor *> data;
        for (const auto &plugin : plugins)
        {
            if (!plugin.isLoaded)
                continue;
            auto editor = plugin.pluginInterface->GetEditorWidget(UI_TYPE::UI_TYPE_OUTBOUND_EDITOR);
            if (editor)
            {
                data.append(editor.release());
            }
        }
        return data;
    }

    const QMultiHash<QString, QPair<QString, QJsonObject>> QvPluginHost::TryDeserializeShareLink(const QString &sharelink, //
                                                                                                 QString *prefix,          //
                                                                                                 QString *errMessage,      //
                                                                                                 QString *newGroupName,    //
                                                                                                 bool *status) const
    {
        Q_UNUSED(newGroupName)
        QMultiHash<QString, QPair<QString, QJsonObject>> data;
        *status = true;
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.SpecialPluginType.contains(SPECIAL_TYPE_SERIALIZOR))
            {
                auto serializer = plugin.pluginInterface->GetSerializer();
                bool thisPluginCanHandle = false;
                for (const auto &prefix : serializer->ShareLinkPrefixes())
                {
                    thisPluginCanHandle = thisPluginCanHandle || sharelink.startsWith(prefix);
                }
                if (thisPluginCanHandle)
                {
                    auto [protocol, outboundSettings] = serializer->DeserializeOutbound(sharelink, prefix, errMessage);
                    *status = *status && errMessage->isEmpty();
                    data.insert(*prefix, { protocol, outboundSettings });
                }
            }
        }
        return data;
    }
    const QvPluginOutboundInfoObject QvPluginHost::TryGetOutboundInfo(const QString &protocol, const QJsonObject &o, bool *status) const
    {
        *status = false;
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.SpecialPluginType.contains(SPECIAL_TYPE_SERIALIZOR))
            {
                auto serializer = plugin.pluginInterface->GetSerializer();
                if (serializer->OutboundProtocols().contains(protocol))
                {
                    auto info = serializer->GetOutboundInfo(protocol, o);
                    *status = true;
                    return info;
                }
            }
        }
        return {};
    }
    const QString QvPluginHost::TrySerializeShareLink(const QString &protocol,             //
                                                      const QJsonObject &outboundSettings, //
                                                      const QString &alias,                //
                                                      const QString &groupName,            //
                                                      bool *status) const
    {
        *status = false;
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.SpecialPluginType.contains(SPECIAL_TYPE_SERIALIZOR))
            {
                auto serializer = plugin.pluginInterface->GetSerializer();
                if (serializer->OutboundProtocols().contains(protocol))
                {
                    auto link = serializer->SerializeOutbound(protocol, alias, groupName, outboundSettings);
                    *status = true;
                    return link;
                }
            }
        }
        return "";
    }

    const QMap<QString, std::shared_ptr<QvPluginKernel>> QvPluginHost::GetPluginKernels() const
    {
        QMap<QString, std::shared_ptr<QvPluginKernel>> kernels;
        for (const auto &plugin : plugins)
        {
            if (plugin.isLoaded && plugin.metadata.SpecialPluginType.contains(SPECIAL_TYPE_KERNEL))
            {
                auto kern = plugin.pluginInterface->GetKernel();
                for (const auto &cap : kern->KernelOutboundCapabilities())
                {
                    kernels.insert(cap.protocol, kern);
                }
            }
        }
        return kernels;
    }

    const QString GetPluginTypeString(const SPECIAL_TYPE_FLAGS &types)
    {
        QStringList typesList;
        if (types.isEmpty())
        {
            typesList << QObject::tr("Normal Plugin");
        }
        for (auto type : types)
        {
            switch (type)
            {
                case SPECIAL_TYPE_KERNEL: typesList << QObject::tr("Kernel"); break;
                case SPECIAL_TYPE_SERIALIZOR: typesList << QObject::tr("Share Link Parser"); break;
                default: typesList << QObject::tr("Unknown type."); break;
            }
        }
        return typesList.join(NEWLINE);
    }

    const QString GetPluginCapabilityString(const CAPABILITY_FLAGS &caps)
    {
        QStringList capsString;
        if (caps.isEmpty())
        {
            capsString << QObject::tr("No Capability");
        }
        for (auto cap : caps)
        {
            switch (cap)
            {
                case CAPABILITY_CONNECTIVITY: capsString << QObject::tr("Connection State Change"); break;
                case CAPABILITY_CONNECTION_ENTRY: capsString << QObject::tr("Connection Change"); break;
                case CAPABILITY_STATS: capsString << QObject::tr("Statistics Event"); break;
                case CAPABILITY_SYSTEM_PROXY: capsString << QObject::tr("System Proxy Event"); break;
                default: capsString << QObject::tr("Unknown"); break;
            }
        }
        return capsString.join(NEWLINE);
    }
} // namespace Qv2ray::components::plugins
