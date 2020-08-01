#pragma once

#include "CoreObjectModels.hpp"
#include "QvConfigIdentifier.hpp"
#include "QvSafeType.hpp"

namespace Qv2ray::base::objects::complex
{
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
     *      |   |       - EXTERNAL                      -> Enables connectionId
     *      |   |       - CHAINED                       -> Enables chainId
     *      |   |       - BALANCER                      -> ?
     *      |
     *
     *******************************************************************/

    enum ComplexTagNodeMode
    {
        NODE_INBOUND,
        NODE_OUTBOUND,
        NODE_RULE
    };

    enum MetaOutboundObjectType
    {
        METAOUTBOUND_ORIGINAL,
        METAOUTBOUND_EXTERNAL,
        METAOUTBOUND_BALANCER,
        METAOUTBOUND_CHAIN
    };

    DECL_IDTYPE(BalancerTag);

    constexpr auto META_OUTBOUND_KEY_NAME = "QV2RAY_OUTBOUND_METADATA";
    constexpr auto META_CHANS_KEY = "QV2RAY_CHAINS";

    typedef BalancerObject ComplexBalancerObject;

    struct OutboundObjectMeta
    {
        MetaOutboundObjectType metaType;
        QString displayName;
        //
        ConnectionId connectionId;
        BalancerTag balancerTag;
        QList<QString> chainedOutbounds;
        //
        safetype::OUTBOUND realOutbound;
        QString getDisplayName() const
        {
            if (metaType == METAOUTBOUND_ORIGINAL)
                return realOutbound["tag"].toString();
            else
                return displayName;
        }
        explicit OutboundObjectMeta() : metaType(METAOUTBOUND_ORIGINAL){};
        JSONSTRUCT_REGISTER(OutboundObjectMeta, F(metaType, displayName, connectionId, balancerTag, chainedOutbounds))
    };

    inline OutboundObjectMeta make_outbound(const QList<QString> &chain, const QString &tag)
    {
        OutboundObjectMeta meta;
        meta.metaType = METAOUTBOUND_CHAIN;
        meta.chainedOutbounds = chain;
        meta.displayName = tag;
        return meta;
    }

    inline OutboundObjectMeta make_outbound(const BalancerTag &id, const QString &tag)
    {
        OutboundObjectMeta meta;
        meta.metaType = METAOUTBOUND_BALANCER;
        meta.balancerTag = id;
        meta.displayName = tag;
        return meta;
    }

    inline OutboundObjectMeta make_outbound(const ConnectionId &id, const QString &tag)
    {
        OutboundObjectMeta meta;
        meta.metaType = METAOUTBOUND_EXTERNAL;
        meta.connectionId = id;
        meta.displayName = tag;
        return meta;
    }

    inline OutboundObjectMeta make_outbound(const safetype::OUTBOUND &outbound)
    {
        OutboundObjectMeta meta;
        meta.metaType = METAOUTBOUND_ORIGINAL;
        meta.realOutbound = outbound;
        meta.displayName = outbound["tag"].toString();
        return meta;
    }
} // namespace Qv2ray::base::objects::complex

using namespace Qv2ray::base::objects::complex;
