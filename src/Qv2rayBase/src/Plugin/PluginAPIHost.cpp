//  Qv2rayBase, the modular feature-rich infrastructure library for Qv2ray.
//  Copyright (C) 2021 Moody and relavent Qv2ray contributors.
//  Copyright (C) 2022 Moody.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Qv2rayBase/Plugin/PluginAPIHost.hpp"

#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Plugin/PluginManagerCore.hpp"
#include "Qv2rayBase/Qv2rayBaseLibrary.hpp"
#include "Qv2rayBase/private/Plugin/PluginAPIHost_p.hpp"
#include "Qv2rayBase/private/Plugin/PluginManagerCore_p.hpp"

using namespace Qv2rayPlugin;

namespace Qv2rayBase::Plugin
{
    using namespace Qv2rayPlugin::Event;
    using namespace Qv2rayPlugin::Kernel;
    using namespace Qv2rayPlugin::Outbound;
    using namespace Qv2rayPlugin::Subscription;

    PluginAPIHost::PluginAPIHost() : d_ptr(new PluginAPIHostPrivate)
    {
    }

    PluginAPIHost::~PluginAPIHost()
    {
    }

    void PluginAPIHost::InitializePlugins()
    {
        Q_D(PluginAPIHost);
        for (const auto &plugin : Qv2rayBaseLibrary::PluginManagerCore()->GetPlugins(COMPONENT_KERNEL))
            for (const auto &kinterface : plugin->pinterface->KernelInterface()->PluginKernels())
                d->kernels.insert(kinterface.Id, kinterface);

        for (const auto &plugin : Qv2rayBaseLibrary::PluginManagerCore()->GetPlugins(COMPONENT_LATENCY_TEST_ENGINE))
            for (const auto &linterface : plugin->pinterface->LatencyTestHandler()->PluginLatencyTestEngines())
                d->latencyTesters.insert(linterface.Id, linterface);
    }

    QList<LatencyTestEngineInfo> PluginAPIHost::Latency_GetAllEngines() const
    {
        Q_D(const PluginAPIHost);
        return d->latencyTesters.values();
    }

    LatencyTestEngineInfo PluginAPIHost::Latency_GetEngine(const LatencyTestEngineId &id) const
    {
        Q_D(const PluginAPIHost);
        return d->latencyTesters[id];
    }

    QList<KernelFactory> PluginAPIHost::Kernel_GetAllKernels() const
    {
        Q_D(const PluginAPIHost);
        return d->kernels.values();
    }

    KernelFactory PluginAPIHost::Kernel_GetInfo(const KernelId &kid) const
    {
        Q_D(const PluginAPIHost);
        return d->kernels[kid];
    }

    KernelId PluginAPIHost::Kernel_GetDefaultKernel() const
    {
        Q_D(const PluginAPIHost);
        qsizetype supportedProtocolsCount = 0;
        KernelId result;
        for (auto it = d->kernels.constKeyValueBegin(); it != d->kernels.constKeyValueEnd(); it++)
        {
            if (it->second.Capabilities.testFlag(KERNELCAP_ROUTER))
                if (it->second.SupportedProtocols.size() > supportedProtocolsCount)
                    result = it->first, supportedProtocolsCount = it->second.SupportedProtocols.size();
        }
        return result;
    }

    KernelId PluginAPIHost::Kernel_QueryProtocol(const QSet<QString> &protocols) const
    {
        Q_D(const PluginAPIHost);
        const KernelFactory *bestMatch = nullptr;
        qsizetype maxIntersections = 0;
        for (const auto &k : d->kernels)
        {
            const auto intersection = (k.SupportedProtocols & protocols).size();
            if (maxIntersections < intersection)
            {
                maxIntersections = intersection;
                bestMatch = &k;
            }
        }
        return bestMatch ? bestMatch->Id : NullKernelId;
    }

    std::optional<PluginIOBoundData> PluginAPIHost::Outbound_GetData(const IOConnectionSettings &o) const
    {
        for (const auto &plugin : Qv2rayBaseLibrary::PluginManagerCore()->GetPlugins(COMPONENT_OUTBOUND_HANDLER))
        {
            auto serializer = plugin->pinterface->OutboundHandler();
            if (serializer && serializer->SupportedProtocols().contains(o.protocol))
            {
                auto info = serializer->GetOutboundInfo(o);
                if (info)
                    return info;
            }
        }
        return std::nullopt;
    }

    bool PluginAPIHost::Outbound_SetData(IOConnectionSettings &o, const PluginIOBoundData &info) const
    {
        for (const auto &plugin : Qv2rayBaseLibrary::PluginManagerCore()->GetPlugins(COMPONENT_OUTBOUND_HANDLER))
        {
            auto serializer = plugin->pinterface->OutboundHandler();
            if (serializer && serializer->SupportedProtocols().contains(o.protocol))
            {
                bool result = serializer->SetOutboundInfo(o, info);
                if (result)
                    return result;
            }
        }
        return false;
    }

    std::optional<std::shared_ptr<SubscriptionProvider>> PluginAPIHost::Subscription_CreateProvider(const SubscriptionProviderId &id) const
    {
        for (const auto &plugin : Qv2rayBaseLibrary::PluginManagerCore()->GetPlugins(COMPONENT_SUBSCRIPTION_ADAPTER))
        {
            auto adapterInterface = plugin->pinterface->SubscriptionAdapter();
            if (adapterInterface)
                for (const auto &subscriptionInfo : adapterInterface->GetInfo())
                    if (subscriptionInfo.id == id)
                        return subscriptionInfo.Creator();
        }
        return std::nullopt;
    }

    QList<std::pair<const PluginInfo *, SubscriptionProviderInfo>> PluginAPIHost::Subscription_GetProviderInfoList() const
    {
        QList<std::pair<const PluginInfo *, SubscriptionProviderInfo>> list;
        for (const auto &plugin : Qv2rayBaseLibrary::PluginManagerCore()->GetPlugins(COMPONENT_SUBSCRIPTION_ADAPTER))
        {
            auto adapterInterface = plugin->pinterface->SubscriptionAdapter();
            if (adapterInterface)
                for (const auto &subscriptionInfo : adapterInterface->GetInfo())
                    list << std::make_pair(plugin, subscriptionInfo);
        }
        return list;
    }

    std::pair<const PluginInfo *, SubscriptionProviderInfo> PluginAPIHost::Subscription_GetProviderInfo(const SubscriptionProviderId &id) const
    {
        for (const auto &plugin : Qv2rayBaseLibrary::PluginManagerCore()->GetPlugins(COMPONENT_SUBSCRIPTION_ADAPTER))
        {
            auto adapterInterface = plugin->pinterface->SubscriptionAdapter();
            if (adapterInterface)
                for (const auto &subscriptionInfo : adapterInterface->GetInfo())
                    if (subscriptionInfo.id == id)
                        return std::make_pair(plugin, subscriptionInfo);
        }
        return {};
    }

    std::optional<QString> PluginAPIHost::Outbound_Serialize(const QString &name, const IOConnectionSettings &outbound) const
    {
        for (const auto &plugin : Qv2rayBaseLibrary::PluginManagerCore()->GetPlugins(COMPONENT_OUTBOUND_HANDLER))
        {
            auto serializer = plugin->pinterface->OutboundHandler();
            if (serializer && serializer->SupportedProtocols().contains(outbound.protocol))
            {
                const auto result = serializer->Serialize(name, outbound);
                if (result)
                    return result;
            }
        }
        return std::nullopt;
    }

    std::optional<std::pair<QString, IOConnectionSettings>> PluginAPIHost::Outbound_Deserialize(const QString &link) const
    {
        for (const auto &plugin : Qv2rayBaseLibrary::PluginManagerCore()->GetPlugins(COMPONENT_OUTBOUND_HANDLER))
        {
            auto serializer = plugin->pinterface->OutboundHandler();
            for (const auto &prefix : serializer->SupportedLinkPrefixes())
            {
                if (!link.startsWith(prefix))
                    continue;
                const auto outboundObject = serializer->Deserialize(link);
                if (outboundObject)
                    return outboundObject;
            }
        }
        return std::nullopt;
    }

    ProfileContent PluginAPIHost::PreprocessProfile(const ProfileContent &c) const
    {
        auto profile = c;
        for (const auto &plugin : Qv2rayBaseLibrary::PluginManagerCore()->GetPlugins(Qv2rayPlugin::COMPONENT_PROFILE_PREPROCESSOR))
            profile = plugin->pinterface->ProfilePreprocessor()->PreprocessProfile(profile);
        return profile;
    }

    void PluginAPIHost::SendEventInternal(const ConnectionStats::EventObject &object) const
    {
        for (const auto &plugin : Qv2rayBaseLibrary::PluginManagerCore()->GetPlugins(Qv2rayPlugin::COMPONENT_EVENT_HANDLER))
            plugin->pinterface->EventHandler()->ProcessEvent(object);
    }

    void PluginAPIHost::SendEventInternal(const Connectivity::EventObject &object) const
    {
        for (const auto &plugin : Qv2rayBaseLibrary::PluginManagerCore()->GetPlugins(Qv2rayPlugin::COMPONENT_EVENT_HANDLER))
            plugin->pinterface->EventHandler()->ProcessEvent(object);
    }

    void PluginAPIHost::SendEventInternal(const ConnectionEntry::EventObject &object) const
    {
        for (const auto &plugin : Qv2rayBaseLibrary::PluginManagerCore()->GetPlugins(Qv2rayPlugin::COMPONENT_EVENT_HANDLER))
            plugin->pinterface->EventHandler()->ProcessEvent(object);
    }
} // namespace Qv2rayBase::Plugin
