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
        MODE_JSON,
        MODE_CONNECTIONID
    };

    enum TagNodeMode
    {
        NODE_INBOUND,
        NODE_OUTBOUND,
        NODE_RULE
    };

    struct OutboundObject
    {
        QString displayName;
        OutboundObjectMode mode;
        ConnectionId connectionId;
        explicit OutboundObject() : displayName(), mode(MODE_JSON), connectionId(NullConnectionId){};
        JSONSTRUCT_REGISTER(OutboundObject, F(displayName, mode, connectionId))
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
        BalancerId balancerId;
        OutboundObject object;
        safetype::OUTBOUND realOutbound;
        QString getTag() const
        {
            if (metaType == METAOUTBOUND_ORIGINAL && object.mode == MODE_JSON)
                return realOutbound["tag"].toString();
            else
                return object.displayName;
        }
        JSONSTRUCT_REGISTER(OutboundObjectMeta, F(metaType, chainId, balancerId, object))
    };

    inline OutboundObjectMeta make_outbound(const ChainId &id, const QString &tag)
    {
        OutboundObjectMeta meta;
        meta.metaType = METAOUTBOUND_CHAINED;
        meta.chainId = id;
        meta.object.displayName = tag;
        return meta;
    }

    inline OutboundObjectMeta make_outbound(const BalancerId &id, const QString &tag)
    {
        OutboundObjectMeta meta;
        meta.metaType = METAOUTBOUND_BALANCER;
        meta.balancerId = id;
        meta.object.displayName = tag;
        return meta;
    }

    inline OutboundObjectMeta make_outbound(const ConnectionId &id, const QString &tag)
    {
        OutboundObjectMeta meta;
        meta.metaType = METAOUTBOUND_ORIGINAL;
        meta.object.mode = MODE_CONNECTIONID;
        meta.object.connectionId = id;
        meta.object.displayName = tag;
        return meta;
    }

    inline OutboundObjectMeta make_outbound(const safetype::OUTBOUND &outbound)
    {
        OutboundObjectMeta meta;
        meta.metaType = METAOUTBOUND_ORIGINAL;
        meta.realOutbound = outbound;
        meta.object.displayName = outbound["tag"].toString();
        return meta;
    }
} // namespace Qv2ray::base::objects::complex

using namespace Qv2ray::base::objects::complex;
