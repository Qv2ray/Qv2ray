// WARNING
// Since it's required for this file to know the content of the macros defined in another CPP file.
// We included an CPP file instead of the proper HPP file.
#include "w_RoutesEditor.cpp"

// Supplementary source file for Routes editor, basically providing routes-related operations.


void RouteEditor::AddNewInbound(INBOUND in)
{
    QString tag = in["tag"].toString();
    auto _nodeData = make_unique<QvInboundNodeModel>(make_shared<InboundNodeData>(tag));
    auto &node = nodeScene->createNode(std::move(_nodeData));
    auto pos = QPointF();
    pos.setX(0 + GRAPH_GLOBAL_OFFSET_X);
    pos.setY(inboundNodes.count() * 130 + GRAPH_GLOBAL_OFFSET_Y);
    nodeScene->setNodePosition(node, pos);
    inboundNodes[tag] = &node;
    inbounds[getTag(in)] = in;
}

void RouteEditor::AddNewOutbound(OUTBOUND out)
{
    QString tag = getTag(out);
    auto _nodeData = make_unique<QvOutboundNodeModel>(make_shared<OutboundNodeData>(tag));
    auto pos = nodeGraphWidget->pos();
    pos.setX(pos.x() + 850 + GRAPH_GLOBAL_OFFSET_X);
    pos.setY(pos.y() + outboundNodes.count() * 120 + GRAPH_GLOBAL_OFFSET_Y);
    auto &node = nodeScene->createNode(std::move(_nodeData));
    nodeScene->setNodePosition(node, pos);
    outboundNodes[tag] = &node;
    outbounds[tag] = out;
    defaultOutboundCombo->addItem(tag);
}

void RouteEditor::AddNewRule(RuleObject rule)
{
    // Prevent duplicate
    if (ruleNodes.contains(rule.QV2RAY_RULE_TAG)) {
        rule.QV2RAY_RULE_TAG += "-" + GenerateRandomString(5);
    }

    rules[rule.QV2RAY_RULE_TAG] = rule;
    auto pos = nodeGraphWidget->pos();
    pos.setX(pos.x() + 350 + GRAPH_GLOBAL_OFFSET_X);
    pos.setY(pos.y() + ruleNodes.count() * 120 + GRAPH_GLOBAL_OFFSET_Y);
    auto _nodeData = make_unique<QvRuleNodeDataModel>(make_shared<RuleNodeData>(rule.QV2RAY_RULE_TAG));
    auto &node = nodeScene->createNode(std::move(_nodeData));
    nodeScene->setNodePosition(node, pos);

    for (auto inTag : rule.inboundTag) {
        auto inboundNode = inboundNodes[inTag];
        nodeScene->createConnection(node, 0, *inboundNode, 0);
    }

    // If not using balancers (use outbound tag)
    if (!rule.QV2RAY_RULE_USE_BALANCER && outboundNodes.contains(rule.outboundTag)) {
        nodeScene->createConnection(*outboundNodes[rule.outboundTag], 0, node, 0);
    }

    this->ruleNodes[rule.QV2RAY_RULE_TAG] = &node;
    ruleListWidget->addItem(rule.QV2RAY_RULE_TAG);
}

void RouteEditor::RenameItemTag(ROUTE_EDIT_MODE mode, const QString &originalTag, const QString &newTag)
{
    switch (mode) {
        case RENAME_RULE:
            if (rules.contains(originalTag) && ruleNodes.contains(originalTag)) {
                if (rules.contains(newTag) && rules.contains(newTag)) {
                    QvMessageBox(this, tr("Rename tags"), tr("The new tag has been used, please suggest another."));
                    return;
                }

                rules[newTag] = rules.take(originalTag);
                ruleNodes[newTag] = ruleNodes.take(originalTag);
                //
                auto node = static_cast<QvRuleNodeDataModel *>(ruleNodes[newTag]->nodeDataModel());
                node->setData(newTag);
                // No other operation needed, but need to rename the one in the ruleOrder list widget.
                auto items = ruleListWidget->findItems(originalTag, Qt::MatchExactly);

                if (!items.isEmpty()) {
                    auto item = items.first();
                    item->setText(newTag);
                } else {
                    LOG(MODULE_UI, "Cannot find a node: " + originalTag)
                }

                if (currentRuleTag == originalTag) {
                    currentRuleTag = newTag;
                }

                // Do not move this line, since originalTag is a reference (implicitly a pointer) to this value.
                // We must change this at last.
                rules[newTag].QV2RAY_RULE_TAG = newTag;
            } else {
                LOG(MODULE_UI, "There's thing match " + originalTag + " in the containers.")
            }

            break;

        case RENAME_OUTBOUND:
            if (outbounds.contains(originalTag) && outboundNodes.contains(originalTag)) {
                if (outbounds.contains(newTag) && outboundNodes.contains(newTag)) {
                    QvMessageBox(this, tr("Rename tags"), tr("The new tag has been used, please suggest another."));
                    return;
                }

                outbounds[newTag] = outbounds.take(originalTag);
                outboundNodes[newTag] = outboundNodes.take(originalTag);
                auto node = static_cast<QvOutboundNodeModel *>(outboundNodes[newTag]->nodeDataModel());
                node->setData(newTag);

                // Change outbound tag in rules accordingly.
                for (auto k : rules.keys()) {
                    auto v = rules[k];

                    if (v.outboundTag == originalTag) {
                        v.outboundTag = newTag;
                        // Put this inside the if block since no need an extra operation if the condition is false.
                        rules[k] = v;
                    }
                }
            } else {
                LOG(MODULE_UI, "Failed to rename an outbound --> Item not found.")
            }

            break;

        case RENAME_INBOUND:
            if (inbounds.contains(originalTag) && inboundNodes.contains(originalTag)) {
                if (inbounds.contains(newTag) && inboundNodes.contains(newTag)) {
                    QvMessageBox(this, tr("Rename tags"), tr("The new tag has been used, please suggest another."));
                    return;
                }

                inbounds[newTag] = inbounds.take(originalTag);
                inboundNodes[newTag] = inboundNodes.take(originalTag);
                static_cast<QvInboundNodeModel *>(inboundNodes[newTag]->nodeDataModel())->setData(newTag);

                // Change inbound tag in rules accordingly.
                for (auto k : rules.keys()) {
                    auto v = rules[k];

                    if (v.inboundTag.contains(originalTag)) {
                        v.inboundTag.removeAll(originalTag);
                        v.inboundTag.append(newTag);
                        // Put this inside the if block since no need an extra operation if the condition is false.
                        rules[k] = v;
                    }
                }
            } else {
                LOG(MODULE_UI, "Failed to rename an outbound --> Item not found.")
            }

            break;
    }
}
