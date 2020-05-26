#pragma once

#include "base/Qv2rayBase.hpp"

#include <QObject>
namespace Qv2ray::core::handler
{
    class RouteHandler : public QObject
    {
        Q_OBJECT
      public:
        explicit RouteHandler(QObject *parent = nullptr);
        void SaveRoutes() const;
        //
        // Final Config Generation
        CONFIGROOT GenerateFinalConfig(const ConnectionGroupPair &pair) const;
        CONFIGROOT GenerateFinalConfig(CONFIGROOT root, const GroupRoutingId &routingId) const;
        // Route Table Generation
        ROUTING GenerateRoutes(bool enableProxy, bool bypassCN, const QString &outboundTag, const QvConfig_Route &routeConfig) const;

      private:
        QHash<GroupRoutingId, GroupRoutingConfig> configs;
    };
    inline ::Qv2ray::core::handler::RouteHandler *RouteManager = nullptr;
} // namespace Qv2ray::core::handler
