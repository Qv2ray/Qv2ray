#include "core/CoreUtils.hpp"
#include "ui/common/UIBase.hpp"
#include "ui/node/models/InboundNodeModel.hpp"
#include "ui/node/models/OutboundNodeModel.hpp"
#include "ui/node/models/RuleNodeModel.hpp"
#include "w_RoutesEditor.hpp"

#include <nodes/internal/FlowScene.hpp>
// Supplementary source file for Routes editor, basically providing
// routes-related operations.

void RouteEditor::AddInbound(const INBOUND &in)
{
    // QString tag = getTag(in);
    //
    // if (inboundNodes.contains(tag))
    //    tag.append("_" + GenerateRandomString(5));
    //
    // in["tag"] = tag;
    // inbounds << in;
    // auto _nodeData = std::make_unique<QvInboundNodeModel>(std::make_shared<InboundNodeData>(std::make_shared<INBOUND>(inbounds.last())));
    // auto &node = nodeScene->createNode(std::move(_nodeData));
    // QPointF pos;
    // pos.setX(0 + GRAPH_GLOBAL_OFFSET_X);
    // pos.setY(inboundNodes.count() * 130 + GRAPH_GLOBAL_OFFSET_Y);
    // nodeScene->setNodePosition(node, pos);
    // inboundNodes.insert(tag, node.id());
}

void RouteEditor::AddOutbound(OutboundObjectMeta &metaOutboud)
{
    QString tag;
    switch (metaOutboud.metaType)
    {
        case complex::METAOUTBOUND_ORIGINAL:
        {
            tag = getTag(metaOutboud.realOutbound);
            break;
        }
        case complex::METAOUTBOUND_CHAINED:
        case complex::METAOUTBOUND_BALANCER:
        {
            tag = metaOutboud.object.externalTag;
            break;
        }
    }
    outbounds << metaOutboud;
    auto _nodeData = std::make_unique<OutboundNodeModel>(std::make_shared<OutboundObjectMeta>(outbounds.last()));
    auto pos = nodeGraphWidget->pos();
    pos.setX(pos.x() + 850 + GRAPH_GLOBAL_OFFSET_X);
    pos.setY(pos.y() + outbounds.count() * 120 + GRAPH_GLOBAL_OFFSET_Y);
    auto &node = nodeScene->createNode(std::move(_nodeData));
    nodeScene->setNodePosition(node, pos);
    defaultOutboundCombo->addItem(tag);
}

QString RouteEditor::AddNewRule()
{
    // Add Route
    RuleObject rule;
    //
    rule.QV2RAY_RULE_ENABLED = true;
    rule.QV2RAY_RULE_USE_BALANCER = false;
    // Default balancer tag, it's a random string.
    auto bTag = GenerateRandomString();
    rule.QV2RAY_RULE_TAG = rules.isEmpty() ? tr("Default rule") : (tr("rule") + "-" + GenerateRandomString(6));
    rule.balancerTag = bTag;
    // balancers[bTag] = QStringList();
    AddRule(rule);
    return rule.QV2RAY_RULE_TAG;
}

void RouteEditor::AddRule(const RuleObject &rule)
{
    rules << rule;
    auto pos = nodeGraphWidget->pos();
    pos.setX(pos.x() + 350 + GRAPH_GLOBAL_OFFSET_X);
    pos.setY(pos.y() + rules.count() * 120 + GRAPH_GLOBAL_OFFSET_Y);
    auto _nodeData = std::make_unique<RuleNodeModel>(std::make_shared<RuleObject>(rules.last()));
    auto &node = nodeScene->createNode(std::move(_nodeData));
    nodeScene->setNodePosition(node, pos);

    ruleListWidget->addItem(rule.QV2RAY_RULE_TAG);
}

// Do not use reference here, we need deep copy of EVERY QString.
void RouteEditor::RenameItemTag(ROUTE_EDIT_MODE mode, const QString originalTag, QString *newTag)
{
    /*
    switch (mode)
    {
        case RENAME_RULE:
            if (rules.contains(originalTag) && ruleNodes.contains(originalTag))
            {
                if (rules.contains(*newTag) && ruleNodes.contains(*newTag))
                {
                    QvMessageBoxWarn(this, tr("Rename tags"), tr("The new tag has been used, we appended a postfix."));
                    *newTag += "_" + GenerateRandomString(5);
                }

                const auto &nodeDataModel = (QvRuleNodeModel *) nodeScene->node(ruleNodes.value(originalTag))->nodeDataModel();

                if (nodeDataModel == nullptr)
                {
                    LOG(MODULE_GRAPH, "EMPTY NODE WARN")
                }

                nodeDataModel->setData(*newTag);
                //
                auto rule = rules.take(originalTag);
                rule.QV2RAY_RULE_TAG = *newTag;
                rules.insert(*newTag, rule);
                ruleNodes.insert(*newTag, ruleNodes.take(originalTag));
                //
                // No other operation needed, but need to rename the one in the
                // ruleOrder list widget.
                auto items = ruleListWidget->findItems(originalTag, Qt::MatchExactly);

                if (items.isEmpty())
                {
                    LOG(MODULE_UI, "Cannot find a node: " + originalTag)
                }
                else
                {
                    items.first()->setText(*newTag);
                }
                abort();
                //                if (currentRuleTag == originalTag)
                //                {
                //                    currentRuleTag = *newTag;
                //                }
            }
            else
            {
                LOG(MODULE_UI, "There's nothing match " + originalTag + " in the containers.")
            }

            break;

        case RENAME_OUTBOUND:
            if (outbounds.contains(originalTag) && outboundNodes.contains(originalTag))
            {
                if (outbounds.contains(*newTag) && outboundNodes.contains(*newTag))
                {
                    QvMessageBoxWarn(this, tr("Rename tags"), tr("The new tag has been used, we appended a random string to the tag."));
                    *newTag += "_" + GenerateRandomString(5);
                }

                auto out = outbounds.take(originalTag);
                out["tag"] = *newTag;
                outbounds.insert(*newTag, out);
                outboundNodes.insert(*newTag, outboundNodes.take(originalTag));
                const auto &node = (QvOutboundNodeModel *) nodeScene->node(outboundNodes.value(*newTag))->nodeDataModel();

                if (node == nullptr)
                {
                    LOG(MODULE_GRAPH, "EMPTY NODE WARN")
                }

                node->setData(*newTag);

                // Change outbound tag in rules accordingly.
                for (const auto &k : rules.keys())
                {
                    auto v = rules.value(k);

                    if (v.outboundTag == originalTag)
                    {
                        v.outboundTag = *newTag;
                        // Put this inside the if block since no need an extra
                        // operation if the condition is false.
                        rules.insert(k, v);
                    }
                }

                // Resolve default outbound.
                ResolveDefaultOutboundTag(originalTag, *newTag);
            }
            else
            {
                LOG(MODULE_UI, "Failed to rename an outbound --> Item not found.")
            }

            break;

        case RENAME_INBOUND:
            if (inbounds.contains(originalTag) && inboundNodes.contains(originalTag))
            {
                if (inbounds.contains(*newTag) && inboundNodes.contains(*newTag))
                {
                    QvMessageBoxWarn(this, tr("Rename tags"), tr("The new tag has been used, we appended a postfix."));
                    *newTag += "_" + GenerateRandomString(5);
                }
                auto in = inbounds.take(originalTag);
                in["tag"] = *newTag;
                inbounds.insert(*newTag, in);
                inboundNodes.insert(*newTag, inboundNodes.take(originalTag));
                const auto &node = (QvInboundNodeModel *) nodeScene->node(inboundNodes.value(*newTag))->nodeDataModel();

                if (node == nullptr)
                {
                    LOG(MODULE_GRAPH, "EMPTY NODE WARN")
                }

                node->setData(*newTag);

                // Change inbound tag in rules accordingly.
                // k -> rule tag
                // v -> rule object
                for (const auto &k : rules.keys())
                {
                    auto v = rules.value(k);

                    if (v.inboundTag.contains(originalTag))
                    {
                        v.inboundTag.append(*newTag);
                        v.inboundTag.removeAll(originalTag);
                        // Put this inside the if block since no need an extra
                        // operation if the condition is false.
                        rules.insert(k, v);
                    }
                }
            }
            else
            {
                LOG(MODULE_UI, "Failed to rename an outbound --> Item not found.")
            }

            break;
    }*/
}

// Do not use const reference here.
void RouteEditor::ResolveDefaultOutboundTag(const QString original, const QString newTag)
{
    LOG(MODULE_UI, "Resolving default outbound settings: default=" + defaultOutbound + " original=" + original + " new=" + newTag)
    auto isDefaultChanged = original == defaultOutbound;
    //
    isLoading = true;
    defaultOutboundCombo->clear();
    //
    for (const auto &out : outbounds) defaultOutboundCombo->addItem(getTag(out.realOutbound));
    //
    isLoading = false;
    //
    if (!isDefaultChanged)
    {
        LOG(MODULE_UI, "Default outbound is not changed: retaining: " + defaultOutbound)
        // Just simply restore the default one.
        defaultOutboundCombo->setCurrentText(defaultOutbound);
    }
    else if (newTag.isEmpty())
    {
        LOG(MODULE_UI, "Default outbound is removed, using first key from the outbounds as the default one.")

        // Removed the default one, so set the first one as the default.
        if (outbounds.isEmpty())
        {
            defaultOutbound.clear();
        }
        else
        {
            defaultOutbound = getTag(outbounds.first().realOutbound);
            defaultOutboundCombo->addItem(getTag(outbounds.first().realOutbound));
        }
    }
    else
    {
        LOG(MODULE_UI, "Default outbound is renamed, new tag is " + newTag)
        defaultOutboundCombo->setCurrentText(newTag);
        defaultOutbound = newTag;
    }
}
