#include "NodeDispatcher.hpp"

#include "core/CoreUtils.hpp"
#include "models/InboundNodeModel.hpp"
#include "models/OutboundNodeModel.hpp"
#include "models/RuleNodeModel.hpp"
#include "utils/QvHelpers.hpp"

#include <nodes/Node>

#define QV_MODULE_NAME "NodeDispatcher"

NodeDispatcher::NodeDispatcher(QObject *parent) : QObject(parent)
{
}

NodeDispatcher::~NodeDispatcher()
{
}

std::tuple<QMap<QString, INBOUND>, QMap<QString, RuleObject>, QMap<QString, OutboundObjectMeta>> NodeDispatcher::GetData() const
{
    QMap<QString, INBOUND> _inbounds;
    QMap<QString, RuleObject> _rules;
    QMap<QString, OutboundObjectMeta> _outbounds;
    for (const auto &[key, val] : inbounds.toStdMap())
        _inbounds[key] = *val;
    for (const auto &[key, val] : rules.toStdMap())
        _rules[key] = *val;
    for (const auto &[key, val] : outbounds.toStdMap())
        _outbounds[key] = *val;
    return std::make_tuple(_inbounds, _rules, _outbounds);
}

void NodeDispatcher::LoadFullConfig(const CONFIGROOT &root)
{
    isOperationLocked = true;
    // Show connections in the node graph
    for (const auto &in : root["inbounds"].toArray())
    {
        auto inObject = in.toObject();
        if (inObject["tag"].toString().isEmpty())
            inObject["tag"] = GenerateRandomString();
        auto _ = CreateInbound(INBOUND(in.toObject()));
    }

    for (const auto &out : root["outbounds"].toArray())
    {
        const auto meta = OutboundObjectMeta::loadFromOutbound(OUTBOUND(out.toObject()));
        if (meta.metaType == METAOUTBOUND_ORIGINAL && meta.realOutbound["tag"].toString().isEmpty())
            meta.realOutbound["tag"] = GenerateRandomString();
        auto _ = CreateOutbound(meta);
    }

    for (const auto &item : root["routing"].toObject()["rules"].toArray())
    {
        auto _ = CreateRule(RuleObject::fromJson(item.toObject()));
    }

    for (const auto &balancer : root["routing"].toObject()["balancers"].toArray())
    {
        const auto array = balancer.toObject()["selector"].toArray();
        QStringList selector;
        for (const auto &item : array)
        {
            selector << item.toString();
        }
        QString strategyType = balancer.toObject()["strategy"].toObject()["type"].toString("random");
        const auto meta = make_balancer_outbound(selector, strategyType, balancer.toObject()["tag"].toString());
        auto _ = CreateOutbound(meta);
    }

    for (const auto &rule : rules)
    {
        if (!ruleNodes.contains(rule->QV2RAY_RULE_TAG))
        {
            LOG("Could not find rule: ", rule->QV2RAY_RULE_TAG);
            continue;
        }
        const auto ruleNodeId = ruleNodes[rule->QV2RAY_RULE_TAG];
        // Process inbounds.
        for (const auto &inboundTag : rule->inboundTag)
        {
            if (!inboundNodes.contains(inboundTag))
            {
                LOG("Could not find inbound: ", inboundTag);
                continue;
            }
            const auto inboundNodeId = inboundNodes.value(inboundTag);
            ruleScene->createConnection(*ruleScene->node(ruleNodeId), 0, *ruleScene->node(inboundNodeId), 0);
        }

        const auto &outboundTag = rule->outboundTag.isEmpty() ? rule->balancerTag : rule->outboundTag;
        if (!outboundNodes.contains(outboundTag))
        {
            LOG("Could not find outbound: ", outboundTag);
            continue;
        }
        const auto &outboundNodeId = outboundNodes[outboundTag];
        ruleScene->createConnection(*ruleScene->node(outboundNodeId), 0, *ruleScene->node(ruleNodeId), 0);
    }
    isOperationLocked = false;
    emit OnFullConfigLoadCompleted();
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

#define CLEANUP(type)                                                                                                                                \
    if (!type##Nodes.values().contains(nodeId))                                                                                                      \
    {                                                                                                                                                \
        LOG("Could not find a " #type " with id: " + nodeId.toString());                                                                             \
        return;                                                                                                                                      \
    }                                                                                                                                                \
    const auto type##Tag = type##Nodes.key(nodeId);                                                                                                  \
    const auto type = type##s.value(type##Tag);                                                                                                      \
                                                                                                                                                     \
    type##Nodes.remove(type##Tag);                                                                                                                   \
    type##s.remove(type##Tag);

    if (isInbound)
    {
        CLEANUP(inbound);
    }
    else if (isOutbound)
    {
        CLEANUP(outbound);
        const auto object = *outbound;
        if (object.metaType == METAOUTBOUND_CHAIN)
        {
            emit OnChainedOutboundDeleted(object);
        }
        emit OnOutboundDeleted(object);
    }
    else if (isRule)
    {
        CLEANUP(rule);
        emit OnRuleDeleted(*rule);
    }
    else
    {
        Q_UNREACHABLE();
    }
#undef CLEANUP
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
        emit OnChainedOutboundCreated(dataPtr, node);
    }
    else if (dataPtr->metaType == METAOUTBOUND_CHAIN)
    {
        emit OnChainedCreated(dataPtr);
    }
    else
    {
        LOG("Ignored non-connection outbound for Chain Editor.");
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
