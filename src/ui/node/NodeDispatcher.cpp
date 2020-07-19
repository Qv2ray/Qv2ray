#include "NodeDispatcher.hpp"

#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"

NodeDispatcher::NodeDispatcher(QObject *parent) : QObject(parent)
{
}

NodeDispatcher::~NodeDispatcher()
{
}

QString NodeDispatcher::CreateInbound(INBOUND in)
{
    auto tag = getTag(in);
    if (inbounds.contains(tag))
    {
        tag += "_" + GenerateRandomString(5);
    }
    in["tag"] = tag;
    inbounds[tag] = in;
    emit OnInboundCreated(std::make_shared<INBOUND>(inbounds.last()));
    return tag;
}

QString NodeDispatcher::CreateOutbound(OutboundObjectMeta out)
{
    QString tag;
    switch (out.metaType)
    {
        case complex::METAOUTBOUND_CHAINED:
        {
            break;
        }
        case complex::METAOUTBOUND_ORIGINAL:
        {
            break;
        }
        case complex::METAOUTBOUND_BALANCER:
        {
            break;
        }
    }
}

QString NodeDispatcher::CreateRule(RuleObject rule)
{
}
