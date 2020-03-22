#include "QvPluginHost.hpp"

#include "base/Qv2rayBase.hpp"
#include "base/Qv2rayLog.hpp"

#include <QPluginLoader>
namespace Qv2ray::components::plugins
{
    QvPluginHost::QvPluginHost(QObject *parent) : QObject(parent)
    {
        RefreshPluginList();
    }

    int QvPluginHost::RefreshPluginList()
    {
        ClearPlugins();
        for (const auto &pluginDirPath : Qv2rayAssetsPaths("plugins"))
        {
            const QStringList entries = QDir(pluginDirPath).entryList(QDir::Files);
            for (const auto &fileName : entries)
            {
                DEBUG(MODULE_PLUGIN, "Loading plugin: " + fileName + " from: " + pluginDirPath)
                //
                QvPluginInfo info;
                auto pluginFullPath = QDir(pluginDirPath).absoluteFilePath(fileName);
                info.libraryPath = pluginFullPath;
                info.pluginLoader = new QPluginLoader(pluginFullPath, this);
                // auto meta = pluginLoader.metaData();
                // You should not call "delete" on this object, it's handled by the QPluginLoader
                //
                QObject *plugin = info.pluginLoader->instance();
                if (plugin == nullptr)
                {
                    LOG(MODULE_PLUGIN, info.pluginLoader->errorString());
                    continue;
                }
                //
                info.interface = qobject_cast<Qv2rayInterface *>(plugin);
                if (info.interface == nullptr)
                {
                    LOG(MODULE_PLUGIN, "Failed to cast from QObject to Qv2rayPluginInterface")
                    info.pluginLoader->unload();
                    continue;
                }
                //
                if (plugins.contains(info.interface->InternalName()))
                {
                    LOG(MODULE_PLUGIN, "Found another plugin with the same internal name: " + info.interface->InternalName() + ". Skipped")
                    continue;
                }
                //
                if (info.interface->QvPluginInterfaceVersion != QV2RAY_PLUGIN_INTERFACE_VERSION)
                {
                    // The plugin was built for a not-compactable version of Qv2ray. Don't load the plugin by default.
                    LOG(MODULE_PLUGIN, "The plugin " + info.interface->InternalName() +
                                           " is not loaded since it was built against a different version of interface")
                    info.errorMessage = tr("This plugin was built against an incompactable version of Qv2ray Plugin Interface.")       //
                                        + NEWLINE + tr("Please contact the plugin provider or report the issue to Qv2ray Workgroup."); //
                }
                //
                LOG(MODULE_PLUGIN, "Loaded plugin: \"" + info.interface->Name() + "\" made by: \"" + info.interface->Author() + "\"")
                plugins.insert(info.interface->InternalName(), info);
            }
        }
        return plugins.count();
    }

    QStringList QvPluginHost::AvailablePlugins()
    {
        return plugins.keys();
    }
    QvPluginHost::~QvPluginHost()
    {
    }
    void QvPluginHost::ClearPlugins()
    {
        for (auto &&plugin : plugins)
        {
            DEBUG(MODULE_PLUGIN, "Unloading: \"" + plugin.interface->Name() + "\"")
            plugin.pluginLoader->unload();
            plugin.pluginLoader->deleteLater();
        }
        plugins.clear();
    }

} // namespace Qv2ray::components::plugins
