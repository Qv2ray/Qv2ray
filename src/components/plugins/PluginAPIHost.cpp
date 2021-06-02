#include "PluginAPIHost.hpp"

#include "base/Qv2rayBase.hpp"
#include "base/Qv2rayLog.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "utils/QvHelpers.hpp"

#include <QPluginLoader>

#define QV_MODULE_NAME "PluginAPIHost"

PluginAPIHost::PluginAPIHost()
{
    core = new PluginManagerCore;
}

void PluginAPIHost::LoadPlugins()
{
    core->LoadPlugins();
    for (const auto &p : core->GetPlugins(COMPONENT_KERNEL))
        for (const auto &kinfo : p->pinterface->KernelInterface()->GetKernels())
            kernels.insert(kinfo.Id, kinfo);
}

std::unique_ptr<PluginKernel> PluginAPIHost::Kernel_Create(const QUuid &kid) const
{
    return kernels[kid].Create();
}

QString PluginAPIHost::Kernel_GetName(const QUuid &kid) const
{
    return kernels[kid].Name;
}

QUuid PluginAPIHost::Kernel_QueryProtocol(const QString &protocol) const
{
    for (const auto &k : kernels)
        if (k.SupportedProtocols.contains(protocol))
            return k.Id;
    return {};
}

void PluginAPIHost::SetPluginSettings(const QString &pid, const QJsonObject &settings)
{
    const auto plugin = core->GetPlugin(pid);
    plugin->pinterface->m_Settings = settings;
    if (plugin->isEnabled())
        plugin->pinterface->SettingsUpdated();
}

QList<const QvPluginInfo *> PluginAPIHost::AllPlugins() const
{
    return core->AllPlugins();
}

PluginAPIHost::~PluginAPIHost()
{
    delete core;
}

std::optional<PluginIOBoundData> PluginAPIHost::Outbound_GetData(const QString &protocol, const QJsonObject &o) const
{
    for (const auto &plugin : core->GetPlugins(COMPONENT_OUTBOUND_HANDLER))
    {
        auto serializer = plugin->pinterface->OutboundHandler();
        if (serializer && serializer->SupportedProtocols().contains(protocol))
        {
            auto info = serializer->GetOutboundInfo(protocol, o);
            if (info)
                return info;
        }
    }
    return std::nullopt;
}

bool PluginAPIHost::Outbound_SetData(const QString &protocol, QJsonObject &o, const PluginIOBoundData &info) const
{
    for (const auto &plugin : core->GetPlugins(COMPONENT_OUTBOUND_HANDLER))
    {
        auto serializer = plugin->pinterface->OutboundHandler();
        if (serializer && serializer->SupportedProtocols().contains(protocol))
        {
            bool result = serializer->SetOutboundInfo(protocol, o, info);
            if (result)
                return result;
        }
    }
    return false;
}

std::optional<std::shared_ptr<SubscriptionDecoder>> PluginAPIHost::Subscription_QueryType(const QString &type) const
{
    for (const auto &plugin : core->GetPlugins(COMPONENT_SUBSCRIPTION_ADAPTER))
    {
        auto adapterInterface = plugin->pinterface->SubscriptionAdapter();
        if (adapterInterface)
            for (const auto &subscriptionInfo : adapterInterface->SupportedSubscriptionTypes())
                if (subscriptionInfo.type == type)
                    return adapterInterface->GetSubscriptionDecoder(type);
    }
    return std::nullopt;
}

QList<std::pair<const QvPluginInfo *, SubscriptionInfoObject>> PluginAPIHost::Subscription_GetAllAdapters() const
{
    QList<std::pair<const QvPluginInfo *, SubscriptionInfoObject>> list;
    for (const auto &plugin : core->GetPlugins(COMPONENT_SUBSCRIPTION_ADAPTER))
    {
        auto adapterInterface = plugin->pinterface->SubscriptionAdapter();
        if (adapterInterface)
            for (const auto &subscriptionInfo : adapterInterface->SupportedSubscriptionTypes())
                list << std::make_pair(plugin, subscriptionInfo);
    }
    return list;
}

std::optional<QString> PluginAPIHost::Outbound_Serialize(const PluginOutboundDescriptor &info) const
{
    for (const auto &plugin : core->GetPlugins(COMPONENT_OUTBOUND_HANDLER))
    {
        auto serializer = plugin->pinterface->OutboundHandler();
        if (serializer && serializer->SupportedProtocols().contains(info.Protocol))
        {
            const auto result = serializer->Serialize(info);
            if (result)
                return result;
        }
    }
    return std::nullopt;
}

std::optional<PluginOutboundDescriptor> PluginAPIHost::Outbound_Deserialize(const QString &link) const
{
    for (const auto &plugin : core->GetPlugins(COMPONENT_OUTBOUND_HANDLER))
    {
        auto serializer = plugin->pinterface->OutboundHandler();
        for (const auto &prefix : serializer->SupportedLinkPrefixes())
        {
            if (link.startsWith(prefix))
            {
                const auto outboundObject = serializer->Deserialize(link);
                if (outboundObject)
                    return outboundObject;
            }
        }
    }
    return std::nullopt;
}

namespace Qv2ray::components::plugins
{
    const QStringList GetPluginComponentsString(const QList<QV2RAY_PLUGIN_GUI_COMPONENT_TYPE> &types)
    {
        QStringList typesList;
        if (types.isEmpty())
            typesList << QObject::tr("None");
        for (const auto &type : types)
        {
            switch (type)
            {
                case GUI_COMPONENT_SETTINGS: typesList << QObject::tr("Settings Widget"); break;
                case GUI_COMPONENT_INBOUND_EDITOR: typesList << QObject::tr("Inbound Editor"); break;
                case GUI_COMPONENT_OUTBOUND_EDITOR: typesList << QObject::tr("Outbound Editor"); break;
                case GUI_COMPONENT_MAIN_WINDOW_ACTIONS: typesList << QObject::tr("MainWindow Actions"); break;
                case GUI_COMPONENT_TRAY_MENUS: typesList << QObject::tr("Tray Menu"); break;
            }
        }
        return typesList;
    }

    const QStringList GetPluginComponentsString(const QList<QV2RAY_PLUGIN_COMPONENT_TYPE> &types)
    {
        QStringList typesList;
        if (types.isEmpty())
            typesList << QObject::tr("None");
        for (const auto &type : types)
        {
            switch (type)
            {
                case COMPONENT_KERNEL: typesList << QObject::tr("Kernel"); break;
                case COMPONENT_OUTBOUND_HANDLER: typesList << QObject::tr("Outbound Handler/Parser"); break;
                case COMPONENT_SUBSCRIPTION_ADAPTER: typesList << QObject::tr("Subscription Adapter"); break;
                case COMPONENT_EVENT_HANDLER: typesList << QObject::tr("Event Handler"); break;
                case COMPONENT_GUI: typesList << QObject::tr("GUI Components"); break;
            }
        }
        return typesList;
    }
} // namespace Qv2ray::components::plugins
