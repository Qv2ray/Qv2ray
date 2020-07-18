#include "InboundNodeModel.hpp"

#include "ui/node/widgets/InboundWidget.hpp"
InboundNodeModel::InboundNodeModel(std::shared_ptr<INBOUND> data) : NodeDataModel()
{
    widget = new InboundWidget();
    connect(widget, &QvNodeWidget::OnSizeUpdated, this, &InboundNodeModel::embeddedWidgetSizeUpdated);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_TranslucentBackground);
}

void InboundNodeModel::inputConnectionCreated(const QtNodes::Connection &c)
{
}

void InboundNodeModel::inputConnectionDeleted(const QtNodes::Connection &c)
{
}

void InboundNodeModel::outputConnectionCreated(const QtNodes::Connection &c)
{
}

void InboundNodeModel::outputConnectionDeleted(const QtNodes::Connection &c)
{
}

void InboundNodeModel::setInData(std::vector<std::shared_ptr<NodeData>> nodeData, PortIndex port)
{
}
