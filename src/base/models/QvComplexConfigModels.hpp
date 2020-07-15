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
        QString tag;
        OutboundObjectMode mode;
        ConnectionId connectionId;
        QJsonObject json;
    };

    class __ChainID;
    class __BalancerID;
    typedef IDType<__ChainID> ChainID;
    typedef IDType<__BalancerID> BalancerID;
    typedef QList<OutboundObject> ChainObject;
    typedef BalancerObject BalancerObject;

} // namespace Qv2ray::base::objects::complex
