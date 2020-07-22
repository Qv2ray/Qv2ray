#include "InboundNodeModel.hpp"

#include "core/CoreUtils.hpp"
#include "ui/node/widgets/InboundOutboundWidget.hpp"
InboundNodeModel::InboundNodeModel(std::shared_ptr<NodeDispatcher> _dispatcher, std::shared_ptr<INBOUND> data) : NodeDataModel()
{
    dataptr = data;
    dispatcher = _dispatcher;
    widget = new InboundOutboundWidget(InboundOutboundWidget::MODE_INBOUND, dispatcher);
    connect(widget, &QvNodeWidget::OnSizeUpdated, this, &InboundNodeModel::embeddedWidgetSizeUpdated);
    ((InboundOutboundWidget *) widget)->setValue(data);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_TranslucentBackground);
}

void InboundNodeModel::inputConnectionCreated(const QtNodes::Connection &c)
{
    if (!dispatcher->IsNodeConstructing())
        return;
}

void InboundNodeModel::inputConnectionDeleted(const QtNodes::Connection &c)
{
    if (!dispatcher->IsNodeConstructing())
        return;
}

void InboundNodeModel::outputConnectionCreated(const QtNodes::Connection &c)
{
    if (!dispatcher->IsNodeConstructing())
        return;
}

void InboundNodeModel::outputConnectionDeleted(const QtNodes::Connection &c)
{
    if (!dispatcher->IsNodeConstructing())
        return;
}

void InboundNodeModel::setInData(std::vector<std::shared_ptr<NodeData>> nodeData, PortIndex port)
{
}

void InboundNodeModel::onNodeHoverEnter()
{
    emit dispatcher->OnInboundOutboundNodeHovered(getTag(*dataptr.get()), GetInboundInfo(*dataptr.get()));
}

void InboundNodeModel::onNodeHoverLeave(){};
