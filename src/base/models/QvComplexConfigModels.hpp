#pragma once

#include "CoreObjectModels.hpp"
#include "QvConfigIdentifier.hpp"
#include "QvSafeType.hpp"

namespace Qv2ray::base::objects::complex
{
    constexpr auto META_OUTBOUND_KEY_NAME = "QV2RAY_OUTBOUND_METADATA";
    /*****************************************************************
     *   ROOT
     *      | Original Structures
     *      | ======================
     *      | - Inbounds
     *      | - Routing Rules
     *      | - DNS /
     *      |
     *      | Qv2ray-only structures
     *      | ======================
     *      | - Outbounds
     *      |           | - Single Orginal Outbound Unmodified
     *      |           | ==========================================
     *      |           | - QV2RAY_OUTBOUND_METADATA -> OutboundObjectMeta
     *      |           |
     *      |           |
     *      |           |
     *      |           |
     *      |
     *      |
     *      |
     *      |
     *      |
     *      |
     *      |
     *
     *******************************************************************/
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

    DECL_IDTYPE(ChainId);
    DECL_IDTYPE(BalancerId);
    typedef QList<OutboundObject> ChainObject;
    typedef BalancerObject BalancerObject;

    struct OutboundObjectMeta
    {
        complex::MetaOutboundObjectType metaType;
        complex::ChainId chainId;
        complex::OutboundObject object;
        safetype::OUTBOUND realOutbound;
        JSONSTRUCT_REGISTER(OutboundObjectMeta, F(metaType, chainId, object))
    };

} // namespace Qv2ray::base::objects::complex

using namespace Qv2ray::base::objects::complex;
