#include "InboundNodeModel.hpp"

#include "ui/node/widgets/InboundWidget.hpp"

InboundNodeDataModel::InboundNodeDataModel(std::shared_ptr<INBOUND> data) : NodeDataModel()
{
    widget = new InboundWidget();
    connect(widget, &QvNodeWidget::OnSizeUpdated, this, &InboundNodeDataModel::embeddedWidgetSizeUpdated);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_TranslucentBackground);
}

std::shared_ptr<NodeDataType> InboundNodeDataModel::dataType(PortType portType, PortIndex portIndex) const
{
    return NODE_TYPE_INBOUND;
}

void InboundNodeDataModel::setInData(std::shared_ptr<NodeData>, int)
{
    //    dataptr = data;
    //    widget->adjustSize();
}
unsigned int InboundNodeDataModel::nPorts(PortType portType) const
{
    return portType == PortType::Out ? 1 : 0;
}

std::shared_ptr<NodeData> InboundNodeDataModel::outData(PortIndex)
{
    return std::make_shared<InboundNodeData>(dataptr);
}

void InboundNodeDataModel::setData(std::shared_ptr<INBOUND> data)
{
    dataptr = data;
    widget->adjustSize();
}

void InboundNodeDataModel::inputConnectionCreated(const QtNodes::Connection &c)
{
}

void InboundNodeDataModel::inputConnectionDeleted(const QtNodes::Connection &c)
{
}

void InboundNodeDataModel::outputConnectionCreated(const QtNodes::Connection &c)
{
}

void InboundNodeDataModel::outputConnectionDeleted(const QtNodes::Connection &c)
{
}
