#include "PluginManagerCore.hpp"

#include "base/Qv2rayBase.hpp"
#include "base/Qv2rayBaseApplication.hpp"
#include "utils/QvHelpers.hpp"

using namespace Qv2rayPlugin;

#define QV_MODULE_NAME "PluginManagerCore"

bool QvPluginInfo::isEnabled() const
{
    return GlobalConfig.pluginConfig->pluginStates->value(pinterface->GetMetadata().InternalName, true);
}

void QvPluginInfo::setEnabled(bool enabled) const
{
    GlobalConfig.pluginConfig->pluginStates->insert(metadata().InternalName, enabled);
}

PluginManagerCore::PluginManagerCore(QObject *parent) : QObject(parent)
{
}

PluginManagerCore::~PluginManagerCore()
{
    SavePluginSettings();
    for (auto &&plugin : plugins)
    {
        DEBUG("Unloading: \"" + plugin.metadata().Name + "\"");
        plugin.loader->unload();
        plugin.loader->deleteLater();
    }
    plugins.clear();
}

void PluginManagerCore::LoadPlugins()
{
    if (!QvCoreApplication->StartupArguments.noPlugins)
    {
        if (auto dir = QDir(QV2RAY_PLUGIN_SETTINGS_DIR); !dir.exists())
        {
            dir.mkpath(QV2RAY_PLUGIN_SETTINGS_DIR);
        }

        LOG("Reloading plugin list");
        for (const auto &pluginDirPath : QvCoreApplication->GetAssetsPaths("plugins"))
        {
            const auto entries = QDir(pluginDirPath).entryList(QDir::Files);
            for (const auto &fileName : entries)
            {
                tryLoadPlugin(QDir(pluginDirPath).absoluteFilePath(fileName));
            }
        }

        for (auto &plugin : plugins.keys())
        {
            auto conf = JsonFromString(StringFromFile(QV2RAY_PLUGIN_SETTINGS_DIR + plugin + ".conf"));
            plugins[plugin].pinterface->InitializePlugin(QV2RAY_PLUGIN_SETTINGS_DIR + plugin + "/", conf);
        }
    }
    else
    {
        LOG("PluginHost initilization skipped by command line option.");
    }
}

QList<const QvPluginInfo *> PluginManagerCore::GetPlugins(Qv2rayPlugin::QV2RAY_PLUGIN_COMPONENT_TYPE c) const
{
    QList<const QvPluginInfo *> list;
    for (const auto &p : qAsConst(plugins))
    {
        if (p.isEnabled() && p.hasComponent(c))
            list << &p;
    }
    return list;
}

const QList<const QvPluginInfo *> PluginManagerCore::AllPlugins() const
{
    QList<const QvPluginInfo *> list;
    for (const auto &plugin : plugins)
        list << &plugin;
    return list;
}

bool PluginManagerCore::tryLoadPlugin(const QString &pluginFullPath)
{
    QvPluginInfo info;
    info.libraryPath = pluginFullPath;
    info.loader = new QPluginLoader(pluginFullPath, this);

    QObject *plugin = info.loader->instance();
    if (!plugin)
    {
        const auto errMessage = info.loader->errorString();
        LOG(errMessage);
        QvMessageBoxWarn(nullptr, tr("Failed to load plugin"), errMessage);
        return false;
    }

    info.pinterface = qobject_cast<Qv2rayInterface *>(plugin);
    if (!info.pinterface)
    {
        LOG("Failed to cast from QObject to Qv2rayPluginInterface");
        info.loader->unload();
        return false;
    }

    if (info.pinterface->QvPluginInterfaceVersion != QV2RAY_PLUGIN_INTERFACE_VERSION)
    {
        // The plugin was built for a not-compactable version of Qv2ray. Don't load the plugin by default.
        LOG(info.libraryPath + " is built with an older Interface, ignoring");
        QvMessageBoxWarn(nullptr, tr("Cannot load plugin"),
                         tr("The plugin cannot be loaded: ") + NEWLINE + info.libraryPath + NEWLINE NEWLINE +
                             tr("This plugin was built against a different version of the Plugin Interface.") + NEWLINE +
                             tr("Please contact the plugin provider or report the issue to Qv2ray Workgroup."));
        info.loader->unload();
        return false;
    }

    if (plugins.contains(info.metadata().InternalName))
    {
        LOG("Found another plugin with the same internal name: " + info.metadata().InternalName + ". Skipped");
        return false;
    }

    // Normalized function signature should not contain a space char, which would be added by clang-format
    // clang-format off
    connect(plugin, SIGNAL(PluginLog(QString)), this, SLOT(QvPluginLog(QString)));
    connect(plugin, SIGNAL(PluginErrorMessageBox(QString,QString)), this, SLOT(QvPluginMessageBox(QString,QString)));
    // clang-format on

    LOG("Loaded plugin: \"" + info.metadata().Name + "\" made by: \"" + info.metadata().Author + "\"");
    plugins.insert(info.metadata().InternalName, info);
    return true;
}

void PluginManagerCore::QvPluginLog(QString log)
{
    if (auto _interface = qobject_cast<Qv2rayInterface *>(sender()); _interface)
    {
        LOG(_interface->GetMetadata().InternalName, log);
    }
    else
    {
        LOG("Unknown Plugin:", log);
    }
}

void PluginManagerCore::QvPluginMessageBox(QString title, QString message)
{
    const auto pInterface = qobject_cast<Qv2rayInterface *>(sender());
    if (pInterface)
        QvMessageBoxWarn(nullptr, pInterface->GetMetadata().Name + " - " + title, message);
    else
        QvMessageBoxWarn(nullptr, "Unknown Plugin - " + title, message);
}

bool PluginManagerCore::IsPluginEnabled(const QString &internalName) const
{
    return plugins[internalName].isEnabled();
}

void PluginManagerCore::SetPluginEnabled(const QString &internalName, bool isEnabled)
{
    GlobalConfig.pluginConfig->pluginStates->insert(internalName, isEnabled);
}

void PluginManagerCore::SavePluginSettings() const
{
    for (const auto &name : plugins.keys())
    {
        LOG("Saving plugin settings for: \"" + name + "\"");
        auto conf = plugins[name].pinterface->settings;
        StringToFile(JsonToString(conf), QV2RAY_PLUGIN_SETTINGS_DIR + name + ".conf");
    }
}
