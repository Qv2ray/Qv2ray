#include "ui/node/models/RuleNodeModel.hpp"

#include "ui/node/widgets/RuleWidget.hpp"

RuleNodeDataModel::RuleNodeDataModel(std::shared_ptr<RuleObject> data) : NodeDataModel()
{
    widget = new QvNodeRuleWidget();
    connect(widget, &QvNodeWidget::OnSizeUpdated, this, &RuleNodeDataModel::embeddedWidgetSizeUpdated);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_TranslucentBackground);
}
unsigned int RuleNodeDataModel::nPorts(PortType) const
{
    return 1;
}

std::shared_ptr<NodeData> RuleNodeDataModel::outData(PortIndex)
{
    return std::make_shared<RuleNodeData>(dataptr);
}

std::shared_ptr<NodeDataType> RuleNodeDataModel::dataType(PortType portType, PortIndex portIndex) const
{
    switch (portType)
    {
        case PortType::In: return NODE_TYPE_INBOUND;
        case PortType::Out: return NODE_TYPE_OUTBOUND;
        default: return {};
    }
}

void RuleNodeDataModel::setInData(std::shared_ptr<NodeData>, int)
{
    //    dataptr = data;
    //    widget->adjustSize();
}

void RuleNodeDataModel::setData(std::shared_ptr<RuleObject> data)
{
    dataptr = data;
    widget->adjustSize();
}

void RuleNodeDataModel::inputConnectionCreated(const QtNodes::Connection &c)
{
}

void RuleNodeDataModel::inputConnectionDeleted(const QtNodes::Connection &c)
{
}

void RuleNodeDataModel::outputConnectionCreated(const QtNodes::Connection &c)
{
}

void RuleNodeDataModel::outputConnectionDeleted(const QtNodes::Connection &c)
{
}
