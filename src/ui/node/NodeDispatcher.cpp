#include "NodeDispatcher.hpp"

#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"
#include "models/InboundNodeModel.hpp"
#include "models/OutboundNodeModel.hpp"
#include "models/RuleNodeModel.hpp"

#include <nodes/Node>

NodeDispatcher::NodeDispatcher(QObject *parent) : QObject(parent)
{
}

NodeDispatcher::~NodeDispatcher()
{
}

CONFIGROOT NodeDispatcher::GetFullConfig() const
{
    return CONFIGROOT{};
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
    isOperationLocked = true;
    for (const auto &rule : rules)
    {
        if (!ruleNodes.contains(rule->QV2RAY_RULE_TAG))
        {
            LOG(MODULE_NODE, "Could not find rule: " + rule->QV2RAY_RULE_TAG)
            continue;
        }
        const auto ruleNodeId = ruleNodes[rule->QV2RAY_RULE_TAG];
        // Process inbounds.
        for (const auto &inboundTag : rule->inboundTag)
        {
            if (inboundNodes.contains(inboundTag))
            {
                const auto inboundNodeId = inboundNodes.value(inboundTag);
                ruleScene->createConnection(*ruleScene->node(ruleNodeId), 0, *ruleScene->node(inboundNodeId), 0);
            }
            else
            {
                LOG(MODULE_NODE, "Could not find inbound: " + inboundTag)
            }
        }

        const auto outboundTag = rule->QV2RAY_RULE_USE_BALANCER ? rule->balancerTag : rule->outboundTag;
        if (outboundNodes.contains(outboundTag))
        {
            const auto outboundNodeId = outboundNodes[outboundTag];
            ruleScene->createConnection(*ruleScene->node(outboundNodeId), 0, *ruleScene->node(ruleNodeId), 0);
        }
        else
        {
            LOG(MODULE_NODE, "Could not find outbound: " + outboundTag)
        }
    }
    isOperationLocked = false;
}

void NodeDispatcher::OnNodeDeleted(const QtNodes::Node &node)
{
    if (isOperationLocked)
        return;
    const auto nodeId = node.id();
    bool isInbound = inboundNodes.values().contains(nodeId);
    bool isOutbound = outboundNodes.values().contains(nodeId);
    bool isRule = ruleNodes.values().contains(nodeId);
    // Lots of duplicated checks.
    {
        Q_ASSERT_X(isInbound || isOutbound || isRule, "Delete Node", "Node type error.");
        if (isInbound)
            Q_ASSERT_X(!isOutbound && !isRule, "Delete Node", "Node Type Error");
        if (isOutbound)
            Q_ASSERT_X(!isInbound && !isRule, "Delete Node", "Node Type Error");
        if (isRule)
            Q_ASSERT_X(!isInbound && !isOutbound, "Delete Node", "Node Type Error");
    }

#define CLEANUP(type)                                                                                                                           \
    if (!type##Nodes.values().contains(nodeId))                                                                                                 \
    {                                                                                                                                           \
        LOG(MODULE_NODE, "Could not find a " #type " with id: " + nodeId.toString())                                                            \
        return;                                                                                                                                 \
    }                                                                                                                                           \
    const auto type##Tag = type##Nodes.key(nodeId);                                                                                             \
    const auto type = type##s.value(type##Tag);                                                                                                 \
                                                                                                                                                \
    type##Nodes.remove(type##Tag);                                                                                                              \
    type##s.remove(type##Tag);

    if (isInbound)
    {
        CLEANUP(inbound);
    }
    else if (isOutbound)
    {
        CLEANUP(outbound);
        const auto object = *outbound.get();
        emit OnOutboundDeleted(object);
        if (chainedOutboundNodes.contains(object.getDisplayName()))
        {
            emit OnChainedOutboundDeleted(object);
        }
        chainedOutboundNodes.remove(outboundTag);
    }
    else if (isRule)
    {
        CLEANUP(rule);
    }
    else
    {
        Q_UNREACHABLE();
    }
#undef CLEANUP
}

void NodeDispatcher::RequestEditChain(const ChainId &id)
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
    auto dataPtr = std::make_shared<INBOUND>(in);
    inbounds.insert(tag, dataPtr);
    // Create node and emit signals.
    {
        auto nodeData = std::make_unique<InboundNodeModel>(shared_from_this(), dataPtr);
        auto &node = ruleScene->createNode(std::move(nodeData));
        inboundNodes.insert(tag, node.id());
        emit OnInboundCreated(dataPtr, node);
    }
    return tag;
}

QString NodeDispatcher::CreateOutbound(OutboundObjectMeta out)
{
    QString tag = out.getDisplayName();
    // In case the tag is duplicated:
    while (outbounds.contains(tag))
    {
        tag += "_" + GenerateRandomString(5);
        // It's ok to set them directly without checking.
        out.displayName = tag;
        out.realOutbound["tag"] = tag;
    }
    auto dataPtr = std::make_shared<OutboundObjectMeta>(out);
    outbounds.insert(tag, dataPtr);
    // Create node and emit signals.
    {
        auto nodeData = std::make_unique<OutboundNodeModel>(shared_from_this(), dataPtr);
        auto &node = ruleScene->createNode(std::move(nodeData));
        outboundNodes.insert(tag, node.id());
        emit OnOutboundCreated(dataPtr, node);
    }

    // Create node and emit signals.
    if (dataPtr->metaType == METAOUTBOUND_EXTERNAL || dataPtr->metaType == METAOUTBOUND_ORIGINAL)
    {
        auto nodeData = std::make_unique<ChainOutboundNodeModel>(shared_from_this(), dataPtr);
        auto &node = chainScene->createNode(std::move(nodeData));
        chainedOutboundNodes[tag] = node.id();
        emit OnChainedOutboundCreated(dataPtr, node);
    }
    else
    {
        LOG(MODULE_UI, "Ignored non-connection outbound for Chain Editor.")
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
    auto dataPtr = std::make_shared<RuleObject>(rule);
    rules.insert(tag, dataPtr);
    // Create node and emit signals.
    {
        auto nodeData = std::make_unique<RuleNodeModel>(shared_from_this(), dataPtr);
        auto &node = ruleScene->createNode(std::move(nodeData));
        ruleNodes.insert(tag, node.id());
        emit OnRuleCreated(dataPtr, node);
    }
    return tag;
}
