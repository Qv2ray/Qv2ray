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
     *      |
     *      | - Outbounds
     *      |   |
     *      |   | - OUTBOUND
     *      |   |   - Original Outbound Object
     *      |   |   ==========================================
     *      |   |   - "QV2RAY_OUTBOUND_METADATA" -> OutboundObjectMeta
     *      |   |     - realOutbound                -> OUTBOUND A.K.A ref<OUTBOUND>
     *      |   |     - chainId                     -> ChainID
     *      |   |     - object                      -> OutboundObject
     *      |   |     - metaType                    -> MetaOutboundObjectType
     *      |   |       - ORIGINAL                      -> Enables realOutbound
     *      |   |       - CHAINED
     *      |   |       - BALANCER
     *      |   |
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
        MetaOutboundObjectType metaType;
        ChainId chainId;
        OutboundObject object;
        safetype::OUTBOUND realOutbound;
        JSONSTRUCT_REGISTER(OutboundObjectMeta, F(metaType, chainId, object))
    };

} // namespace Qv2ray::base::objects::complex

using namespace Qv2ray::base::objects::complex;
