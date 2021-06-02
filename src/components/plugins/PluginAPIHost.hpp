#pragma once
#include "PluginManagerCore.hpp"
#include "src/plugin-interface/QvPluginInterface.hpp"

#include <QHash>
#include <QMap>
#include <QObject>
#include <memory>

using namespace Qv2rayPlugin;

namespace Qv2ray::ui::common
{
    class GuiPluginAPIHost;
}

namespace Qv2ray::components::plugins
{
    class PluginAPIHost
    {
        friend class Qv2ray::ui::common::GuiPluginAPIHost;

      public:
        explicit PluginAPIHost();
        ~PluginAPIHost();
        void LoadPlugins();
        void SetPluginSettings(const QString &pid, const QJsonObject &settings);

        QList<const QvPluginInfo *> AllPlugins() const;

      public:
        // Kernel API
        std::unique_ptr<PluginKernel> Kernel_Create(const QUuid &kid) const;
        QString Kernel_GetName(const QUuid &kid) const;
        QUuid Kernel_QueryProtocol(const QString &protocol) const;

        // Event API
        template<typename E>
        void Event_Send(const typename E::EventObject &object) const
        {
            for (const auto &plugin : core->GetPlugins(COMPONENT_EVENT_HANDLER))
                plugin->pinterface->EventHandler()->ProcessEvent(object);
        }

        // Outbound API
        std::optional<PluginOutboundDescriptor> Outbound_Deserialize(const QString &link) const;
        std::optional<QString> Outbound_Serialize(const PluginOutboundDescriptor &outbound) const;
        std::optional<PluginIOBoundData> Outbound_GetData(const QString &protocol, const QJsonObject &o) const;
        bool Outbound_SetData(const QString &protocol, QJsonObject &o, const PluginIOBoundData &info) const;

        // Subscription Adapter API
        std::optional<std::shared_ptr<SubscriptionDecoder>> Subscription_QueryType(const QString &type) const;
        QList<std::pair<const QvPluginInfo *, SubscriptionInfoObject>> Subscription_GetAllAdapters() const;

      private:
        PluginManagerCore *core;
        QHash<QUuid, KernelInfo> kernels;
    };

    const QStringList GetPluginComponentsString(const QList<QV2RAY_PLUGIN_GUI_COMPONENT_TYPE> &types);
    const QStringList GetPluginComponentsString(const QList<QV2RAY_PLUGIN_COMPONENT_TYPE> &types);
    inline PluginAPIHost *PluginHost = nullptr;

} // namespace Qv2ray::components::plugins
using namespace Qv2ray::components::plugins;
