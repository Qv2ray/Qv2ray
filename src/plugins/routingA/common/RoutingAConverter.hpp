#pragma once

#include "QvPlugin/Common/CommonTypes.hpp"
#include "core/libRoutingA.hpp"

namespace RoutingA::Converter
{
    QPair<QList<InboundObject>, QList<OutboundObject>> FromRADefines(const QList<Defination> &);
    QList<RuleObject> FromRARouting(const QList<Routing> &);

    QString ToRoutingA(const QList<InboundObject> &, const QList<OutboundObject> &, const QList<RuleObject> &);
} // namespace RoutingA::Converter
