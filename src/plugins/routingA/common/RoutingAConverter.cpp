#include "RoutingAConverter.hpp"

namespace RoutingA::Converter
{
    QPair<QList<InboundObject>, QList<OutboundObject>> FromRADefines(const QList<Defination> &defines)
    {
        QList<InboundObject> inbounds;
        QList<OutboundObject> outbounds;

        QString defaultOutbound;

        for (const auto &def : defines)
        {
            if (def.type == u"inbound"_qs)
            {
                const auto indef = def.content;
                InboundObject in;
                in.name = indef.name;
                in.inboundSettings.protocol = indef.function.name;

                // Only the first value is supported.
                for (const auto &[key, val] : indef.function.namedParams.toStdMap())
                    if (!val.isEmpty())
                        in.inboundSettings.protocolSettings.insert(key, val.first());
            }
            else if (def.type == u"outbound"_qs)
            {
                const auto outdef = def.content;
            }
            else if (def.type == u"default"_qs)
                defaultOutbound = def.value;
            else
                throw ParsingErrorException(u"Unexpected defination: "_qs + def.type, 0, {});
        }

        return { inbounds, outbounds };
    }

    QList<RuleObject> FromRARouting(const QList<Routing> &)
    {
        QList<RuleObject> rules;

        return rules;
    }

    QString ToRoutingA(const QList<InboundObject> &, const QList<OutboundObject> &, const QList<RuleObject> &)
    {
        return {};
    }

} // namespace RoutingA::Converter
