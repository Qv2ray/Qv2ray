#pragma once

#include "CoreObjectModels.hpp"
#include "QvConfigIdentifier.hpp"

namespace Qv2ray::base::objects::complex
{
    enum OutboundObjectMode
    {
        MODE_CONNECTIONID,
        MODE_JSON
    };

    struct OutboundObject
    {
        QString externalTag;
        OutboundObjectMode mode;
        ConnectionId connectionId;
        JSONSTRUCT_REGISTER(OutboundObject, F(externalTag, mode, connectionId))
    };

    enum MetaOutboundObjectType
    {
        METAOUTBOUND_ORIGINAL,
        METAOUTBOUND_BALANCER,
        METAOUTBOUND_CHAINED
    };

    class __ChainID;
    class __BalancerID;
    typedef IDType<__ChainID> ChainID;
    typedef IDType<__BalancerID> BalancerID;
    typedef QList<OutboundObject> ChainObject;
    typedef BalancerObject BalancerObject;
} // namespace Qv2ray::base::objects::complex
