#include "ui/node/models/RuleNodeModel.hpp"

#include "ui/node/widgets/RuleWidget.hpp"

RuleNodeModel::RuleNodeModel(std::shared_ptr<RuleObject> data) : NodeDataModel()
{
    widget = new QvNodeRuleWidget();
    connect(widget, &QvNodeWidget::OnSizeUpdated, this, &NodeDataModel::embeddedWidgetSizeUpdated);
    ((QvNodeRuleWidget *) widget)->setValue(data);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_TranslucentBackground);
}

void RuleNodeModel::inputConnectionCreated(const QtNodes::Connection &c)
{
}

void RuleNodeModel::inputConnectionDeleted(const QtNodes::Connection &c)
{
}

void RuleNodeModel::outputConnectionCreated(const QtNodes::Connection &c)
{
}

void RuleNodeModel::outputConnectionDeleted(const QtNodes::Connection &c)
{
}

void RuleNodeModel::setInData(std::vector<std::shared_ptr<NodeData>> nodeData, PortIndex port)
{
}
