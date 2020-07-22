#include "ui/node/models/RuleNodeModel.hpp"

#include "ui/node/widgets/RuleWidget.hpp"

RuleNodeModel::RuleNodeModel(std::shared_ptr<NodeDispatcher> _dispatcher, std::shared_ptr<RuleObject> data) : NodeDataModel()
{
    dataptr = data;
    dispatcher = _dispatcher;
    widget = new QvNodeRuleWidget(dispatcher);
    connect(widget, &QvNodeWidget::OnSizeUpdated, this, &NodeDataModel::embeddedWidgetSizeUpdated);
    ((QvNodeRuleWidget *) widget)->setValue(data);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_TranslucentBackground);
}

void RuleNodeModel::inputConnectionCreated(const QtNodes::Connection &c)
{
    if (!dispatcher->IsNodeConstructing())
        return;
}

void RuleNodeModel::inputConnectionDeleted(const QtNodes::Connection &c)
{
    if (!dispatcher->IsNodeConstructing())
        return;
}

void RuleNodeModel::outputConnectionCreated(const QtNodes::Connection &c)
{
    if (!dispatcher->IsNodeConstructing())
        return;
}

void RuleNodeModel::outputConnectionDeleted(const QtNodes::Connection &c)
{
    if (!dispatcher->IsNodeConstructing())
        return;
}

void RuleNodeModel::setInData(std::vector<std::shared_ptr<NodeData>> nodeData, PortIndex port)
{
}

void RuleNodeModel::onNodeHoverEnter(){};
void RuleNodeModel::onNodeHoverLeave(){};
