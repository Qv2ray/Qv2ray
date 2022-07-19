//  Qv2rayBase, the modular feature-rich infrastructure library for Qv2ray.
//  Copyright (C) 2021 Moody and relavent Qv2ray contributors.
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

#include "Qv2rayBase/Common/ProfileHelpers.hpp"

#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Plugin/PluginAPIHost.hpp"
#include "Qv2rayBase/Profile/ProfileManager.hpp"
#include "Qv2rayBase/Qv2rayBaseLibrary.hpp"

namespace Qv2rayBase::Utils
{
    int GetConnectionLatency(const ConnectionId &id)
    {
        const auto connection = Qv2rayBaseLibrary::ProfileManager()->GetConnectionObject(id);
        return std::max(connection.latency, 0);
    }

    std::pair<quint64, quint64> GetConnectionUsageAmount(const ConnectionId &id, StatisticsObject::StatisticsType type)
    {
        const auto stat = Qv2rayBaseLibrary::ProfileManager()->GetConnectionObject(id).statistics;
        switch (type)
        {
            case StatisticsObject::ALL: return { stat.directUp + stat.proxyUp, stat.directDown + stat.proxyDown };
            case StatisticsObject::DIRECT: return { stat.directUp, stat.directDown };
            case StatisticsObject::PROXY: return { stat.proxyUp, stat.proxyDown };
        }
        return { 0L, 0L };
    }

    quint64 GetConnectionTotalUsage(const ConnectionId &id, StatisticsObject::StatisticsType type)
    {
        const auto d = GetConnectionUsageAmount(id, type);
        return d.first + d.second;
    }

    InboundObject GetInbound(const ConnectionId &id, int index)
    {
        return Qv2rayBaseLibrary::ProfileManager()->GetConnection(id).inbounds.at(index);
    }

    OutboundObject GetOutbound(const ConnectionId &id, int index)
    {
        return Qv2rayBaseLibrary::ProfileManager()->GetConnection(id).outbounds.at(index);
    }

    QString GetDisplayName(const ConnectionId &id)
    {
        return Qv2rayBaseLibrary::ProfileManager()->GetConnectionObject(id).name;
    }

    QString GetDisplayName(const GroupId &id)
    {
        return Qv2rayBaseLibrary::ProfileManager()->GetGroupObject(id).name;
    }

    QMap<QString, IOBoundData> GetInboundInfo(const ConnectionId &id)
    {
        return GetInboundInfo(Qv2rayBaseLibrary::ProfileManager()->GetConnection(id));
    }

    QMap<QString, IOBoundData> GetInboundInfo(const ProfileContent &root)
    {
        QMap<QString, IOBoundData> infomap;
        for (const auto &in : root.inbounds)
            infomap[in.name] = GetInboundInfo(in);
        return infomap;
    }

    QMap<QString, IOBoundData> GetOutboundInfo(const ConnectionId &id)
    {
        const auto root = Qv2rayBaseLibrary::ProfileManager()->GetConnection(id);
        return GetOutboundInfo(root);
    }

    QMap<QString, IOBoundData> GetOutboundInfo(const ProfileContent &out)
    {
        QMap<QString, IOBoundData> result;
        for (const auto &item : out.outbounds)
            result[item.name] = GetOutboundInfo(item);
        return result;
    }

    QString GetConnectionProtocolDescription(const ConnectionId &id)
    {
        const auto root = Qv2rayBaseLibrary::ProfileManager()->GetConnection(id);
        if (root.outbounds.isEmpty())
            return u""_qs;

        const auto outbound = root.outbounds.first();
        QStringList result;
        result << outbound.outboundSettings.protocol;

        const auto streamSettings = outbound.outboundSettings.streamSettings;

        if (streamSettings.contains(u"network"_qs))
            result << streamSettings[u"network"_qs].toString();

        const auto security = streamSettings[u"security"_qs].toString();
        if (!security.isEmpty() && security != u"none"_qs)
            result << streamSettings[u"security"_qs].toString();

        return result.join(u"+"_qs);
    }

    std::optional<std::pair<QString, ProfileContent>> ConvertConfigFromString(const QString &link)
    {
        const auto optConf = Qv2rayBaseLibrary::PluginAPIHost()->Outbound_Deserialize(link);
        if (!optConf)
            return std::nullopt;

        const auto &[name, outbound] = *optConf;
        return std::pair{ name, ProfileContent{ outbound } };
    }

    std::optional<QString> ConvertConfigToString(const ConnectionId &id)
    {
        auto alias = GetDisplayName(id);
        auto server = Qv2rayBaseLibrary::ProfileManager()->GetConnection(id);
        return ConvertConfigToString(alias, server);
    }

    std::optional<QString> ConvertConfigToString(const QString &alias, const ProfileContent &root)
    {
        if (root.outbounds.isEmpty())
            return u""_qs;
        const auto outbound = root.outbounds.first();
        return Qv2rayBaseLibrary::PluginAPIHost()->Outbound_Serialize(alias, outbound.outboundSettings);
    }

    bool IsComplexConfig(const ConnectionId &id)
    {
        const auto root = Qv2rayBaseLibrary::ProfileManager()->GetConnection(id);
        bool hasRouting = !root.routing.rules.isEmpty();
        bool hasInbound = !root.inbounds.isEmpty();
        bool hasAtLeastOneOutbounds = root.outbounds.size() > 1;
        return hasRouting || hasInbound || hasAtLeastOneOutbounds;
    }

    bool ExpandProfileChains(ProfileContent &root)
    {
        // Proxy Chain Expansion
        const auto outbounds = root.outbounds;
        const auto inbounds = root.inbounds;
        const auto rules = root.routing.rules;

        QList<OutboundObject> finalOutbounds;
        QList<InboundObject> chainingInbounds;
        QList<RuleObject> chainingRules;

        // First pass - Resolve Indexes (tags), build cache
        QMap<QString, OutboundObject> outboundCache;
        for (const auto &outbound : outbounds)
        {
            if (outbound.objectType != OutboundObject::CHAIN)
                outboundCache[outbound.name] = outbound;
        }

        // Second pass - Build Chains
        for (const auto &outbound : outbounds)
        {
            if (outbound.objectType != OutboundObject::CHAIN)
            {
                finalOutbounds << outbound;
                continue;
            }

            if (outbound.chainSettings.chains.isEmpty())
            {
                qInfo() << "Trying to expand an empty chain.";
                continue;
            }

            int nextInboundPort = outbound.chainSettings.chaining_port;
            const auto firstOutboundTag = outbound.name;
            const auto lastOutboundTag = outbound.chainSettings.chains.first();

            IOBoundData lastOutbound;
            QString lastOutboundSNI;

            const auto outbountTagCount = outbound.chainSettings.chains.count();

            for (auto i = outbountTagCount - 1; i >= 0; i--)
            {
                const auto chainOutboundTag = outbound.chainSettings.chains[i];

                const auto isFirstOutbound = i == outbountTagCount - 1;
                const auto isLastOutbound = i == 0;
                const auto newOutboundTag = [&]()
                {
                    if (isFirstOutbound)
                        return firstOutboundTag;
                    else if (isLastOutbound)
                        return lastOutboundTag;
                    else
                        return (firstOutboundTag + "_" + chainOutboundTag + "_" + QString::number(nextInboundPort));
                }();

                if (!outboundCache.contains(chainOutboundTag))
                {
                    qInfo() << "Cannot build outbound chain: Missing tag:" << firstOutboundTag;
                    return false;
                }

                auto newOutbound = outboundCache[chainOutboundTag];

                // Create Inbound
                if (!isFirstOutbound)
                {
                    const auto inboundTag = firstOutboundTag + ":" + QString::number(nextInboundPort) + "->" + newOutboundTag;

                    IOProtocolSettings inboundSettings;
                    inboundSettings[u"address"_qs] = std::get<1>(lastOutbound);
                    inboundSettings[u"port"_qs] = std::get<2>(lastOutbound).from;
                    inboundSettings[u"network"_qs] = "tcp,udp";

                    InboundObject newInbound;
                    newInbound.name = inboundTag;
                    newInbound.inboundSettings.protocol = u"dokodemo-door"_qs;
                    newInbound.inboundSettings.address = u"127.0.0.1"_qs;
                    newInbound.inboundSettings.port = nextInboundPort;
                    newInbound.inboundSettings.protocolSettings = inboundSettings;

                    nextInboundPort++;
                    chainingInbounds << newInbound;
                    //
                    RuleObject ruleObject;
                    ruleObject.inboundTags.append(inboundTag);
                    ruleObject.outboundTag = newOutboundTag;
                    chainingRules.prepend(ruleObject);
                }

                if (!isLastOutbound)
                {
                    // Get Outbound Info for next Inbound
                    auto outboundSettings = newOutbound.outboundSettings;
                    const auto info = Qv2rayBaseLibrary::PluginAPIHost()->Outbound_GetData(outboundSettings);
                    if (!info)
                    {
                        qInfo() << "Cannot find SNI";
                        return false;
                    }

                    lastOutboundSNI = (*info).value(IOBOUND_DATA_TYPE::IO_SNI).toString();
                    lastOutbound = GetOutboundInfo(outboundSettings);

                    // Update allocated port as outbound server/port
                    outboundSettings.address = "127.0.0.1";
                    outboundSettings.port = nextInboundPort;

                    // For those kernels deducing SNI from the server name.
                    if (!lastOutboundSNI.isEmpty())
                        Qv2rayBaseLibrary::PluginAPIHost()->Outbound_SetData(outboundSettings, { { IOBOUND_DATA_TYPE::IO_SNI, std::get<1>(lastOutbound) } });

                    newOutbound.outboundSettings = outboundSettings;

                    // Create new outbound
                    newOutbound.name = newOutboundTag;
                }
                finalOutbounds << newOutbound;
            }
        }

        //
        // Finalize
        {
            root.inbounds.clear();
            root.inbounds << inbounds << chainingInbounds;

            root.routing.rules.clear();
            root.routing.rules << chainingRules << rules;

            root.outbounds = finalOutbounds;
        }
        return true;
    }

    QList<OutboundObject> ExpandProfileExternalOutbounds(const QList<OutboundObject> &outbounds)
    {
        QList<OutboundObject> result;
        result.reserve(outbounds.size());
        for (auto out : outbounds)
        {
            if (out.objectType == OutboundObject::EXTERNAL)
            {
                const auto _outs = Qv2rayBaseLibrary::ProfileManager()->GetConnection(out.externalId).outbounds;
                if (!_outs.isEmpty())
                {
                    auto newOut = _outs.first();
                    newOut.name = out.name;
                    out = newOut;
                }
            }
            result << out;
        }
        return result;
    }
} // namespace Qv2rayBase::Utils
