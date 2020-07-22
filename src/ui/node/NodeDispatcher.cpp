#include "NodeDispatcher.hpp"

#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"
#include "models/InboundNodeModel.hpp"
#include "models/OutboundNodeModel.hpp"
#include "models/RuleNodeModel.hpp"

#include <nodes/Node>

NodeDispatcher::NodeDispatcher(QtNodes::FlowScene *_scene, QObject *parent) : QObject(parent), scene(_scene)
{
}

NodeDispatcher::~NodeDispatcher()
{
}

void NodeDispatcher::LoadFullConfig(const CONFIGROOT &root)
{
    // Show connections in the node graph
    for (const auto &in : root["inbounds"].toArray())
    {
        auto _ = CreateInbound(INBOUND(in.toObject()));
    }

    for (const auto &out : root["outbounds"].toArray())
    {
        OutboundObjectMeta meta;
        meta.loadJson(out.toObject()[META_OUTBOUND_KEY_NAME].toObject());
        meta.realOutbound = OUTBOUND(out.toObject());
        auto _ = CreateOutbound(meta);
    }

    for (const auto &item : root["routing"].toObject()["rules"].toArray())
    {
        auto _ = CreateRule(RuleObject::fromJson(item.toObject()));
    }
    RestoreConnections();
}

void NodeDispatcher::RestoreConnections()
{
    isConstructing = true;
    for (const auto &rule : rules)
    {
        if (!ruleNodes.contains(rule.QV2RAY_RULE_TAG))
        {
            LOG(MODULE_NODE, "Could not find rule: " + rule.QV2RAY_RULE_TAG)
            continue;
        }
        const auto ruleNodeId = ruleNodes[rule.QV2RAY_RULE_TAG];
        // Process inbounds.
        for (const auto &inboundTag : rule.inboundTag)
        {
            if (inboundNodes.contains(inboundTag))
            {
                const auto inboundNodeId = inboundNodes.value(inboundTag);
                scene->createConnection(*scene->node(ruleNodeId).get(), 0, *scene->node(inboundNodeId).get(), 0);
            }
            else
            {
                LOG(MODULE_NODE, "Could not find inbound: " + inboundTag)
            }
        }

        const auto outboundTag = rule.QV2RAY_RULE_USE_BALANCER ? rule.balancerTag : rule.outboundTag;
        if (outboundNodes.contains(outboundTag))
        {
            const auto outboundNodeId = outboundNodes[outboundTag];
            scene->createConnection(*scene->node(outboundNodeId).get(), 0, *scene->node(ruleNodeId).get(), 0);
        }
        else
        {
            LOG(MODULE_NODE, "Could not find outbound: " + outboundTag)
        }
    }
    isConstructing = false;
}

QString NodeDispatcher::CreateInbound(INBOUND in)
{
    auto tag = getTag(in);
    while (inbounds.contains(tag))
    {
        tag += "_" + GenerateRandomString(5);
    }
    in["tag"] = tag;
    inbounds.insert(tag, in);
    // Create node and emit signals.
    {
        auto dataPtr = std::make_shared<INBOUND>(inbounds[tag]);
        auto nodeData = std::make_unique<InboundNodeModel>(shared_from_this(), dataPtr);
        auto &node = scene->createNode(std::move(nodeData));
        inboundNodes.insert(tag, node.id());
        emit OnInboundCreated(dataPtr, node);
    }
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
    outbounds.insert(tag, out);
    // Create node and emit signals.
    {
        auto dataPtr = std::make_shared<OutboundObjectMeta>(outbounds[tag]);
        auto nodeData = std::make_unique<OutboundNodeModel>(shared_from_this(), dataPtr);
        auto &node = scene->createNode(std::move(nodeData));
        outboundNodes.insert(tag, node.id());
        emit OnOutboundCreated(dataPtr, node);
    }
    return tag;
}

QString NodeDispatcher::CreateRule(RuleObject rule)
{
    auto &tag = rule.QV2RAY_RULE_TAG;
    while (rules.contains(tag))
    {
        tag += "_" + GenerateRandomString(5);
    }
    rules.insert(tag, rule);
    // Create node and emit signals.
    {
        auto dataPtr = std::make_shared<RuleObject>(rules[tag]);
        auto nodeData = std::make_unique<RuleNodeModel>(shared_from_this(), dataPtr);
        auto &node = scene->createNode(std::move(nodeData));
        ruleNodes.insert(tag, node.id());
        emit OnRuleCreated(dataPtr, node);
    }
    return tag;
}
