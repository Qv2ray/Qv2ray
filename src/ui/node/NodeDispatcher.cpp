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
    while (inbounds.contains(tag))
    {
        tag += "_" + GenerateRandomString(5);
    }
    in["tag"] = tag;
    inbounds[tag] = in;
    emit OnInboundCreated(std::make_shared<INBOUND>(inbounds[tag]));
    return tag;
}

QString NodeDispatcher::CreateOutbound(OutboundObjectMeta out)
{
    QString tag = out.getTag();
    // In case the tag is duplicated:
    while (outbounds.contains(tag))
    {
        tag += "_" + GenerateRandomString(5);
        // It's ok to set them directly without checking.
        out.object.displayName = tag;
        out.realOutbound["tag"] = tag;
    }
    outbounds[tag] = out;
    emit OnOutboundCreated(std::make_shared<OutboundObjectMeta>(outbounds[tag]));
    return tag;
}

QString NodeDispatcher::CreateRule(RuleObject rule)
{
    auto &tag = rule.QV2RAY_RULE_TAG;
    while (rules.contains(tag))
    {
        tag += "_" + GenerateRandomString(5);
    }
    rules[tag] = rule;
    emit OnRuleCreated(std::make_shared<RuleObject>(rules[tag]));
    return tag;
}
