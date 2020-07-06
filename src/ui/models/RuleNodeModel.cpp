#include "ui/models/RuleNodeModel.hpp"

QvRuleNodeModel::QvRuleNodeModel(std::shared_ptr<RuleNodeData> data) : NodeDataModel()
{
    _ruleTag = data;
    _label = new QLabel();
    //
    QFont font = _label->font();
    font.setPointSize(font.pointSize() + GRAPH_NODE_LABEL_FONTSIZE_INCREMENT);
    font.setBold(true);
    _label->setFont(font);
    //
    _label->setText(data->GetRuleTag());
    _label->setWindowFlags(Qt::FramelessWindowHint);
    _label->setAttribute(Qt::WA_TranslucentBackground);
}
