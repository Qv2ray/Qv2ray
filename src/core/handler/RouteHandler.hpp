#pragma once

#include "base/Qv2rayBase.hpp"

namespace Qv2ray::core::handler
{
    class RouteHandler : public QObject
    {
        Q_OBJECT
      public:
        explicit RouteHandler(QObject *parent = nullptr);
        ~RouteHandler();
        void SaveRoutes() const;
        //
        std::tuple<bool, DNSConfig, FakeDNSConfig> GetDNSSettings(const GroupRoutingId &id) const
        {
            return { configs[id].overrideDNS, configs[id].dnsConfig, configs[id].fakeDNSConfig };
        }
        std::pair<bool, RouteConfig> GetAdvancedRoutingSettings(const GroupRoutingId &id) const
        {
            return { configs[id].overrideRoute, configs[id].routeConfig };
        }
        //
        bool SetDNSSettings(const GroupRoutingId &id, bool overrideGlobal, const DNSConfig &dns, const FakeDNSConfig &fakeDNS);
        bool SetAdvancedRouteSettings(const GroupRoutingId &id, bool overrideGlobal, const RouteConfig &dns);
        //
        OUTBOUNDS ExpandExternalConnection(const OUTBOUNDS &outbounds) const;
        //
        // Final Config Generation
        CONFIGROOT GenerateFinalConfig(const ConnectionGroupPair &pair, bool hasAPI = true) const;
        CONFIGROOT GenerateFinalConfig(CONFIGROOT root, const GroupRoutingId &routingId, bool hasAPI = true) const;
        //
        bool ExpandChainedOutbounds(CONFIGROOT &) const;

      private:
        QHash<GroupRoutingId, GroupRoutingConfig> configs;
    };
    inline ::Qv2ray::core::handler::RouteHandler *RouteManager = nullptr;
} // namespace Qv2ray::core::handler
