#include "ui/models/RuleNodeModel.hpp"

QvRuleNodeModel::QvRuleNodeModel(std::shared_ptr<RuleNodeData> data) : NodeDataModel()
{
    _ruleTag = data;
    ruleWidget = new QvNodeRuleWidget();
    ruleWidget->setWindowFlags(Qt::FramelessWindowHint);
    ruleWidget->setAttribute(Qt::WA_TranslucentBackground);
}
