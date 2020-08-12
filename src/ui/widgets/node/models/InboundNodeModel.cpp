#include "InboundNodeModel.hpp"

#include "core/CoreUtils.hpp"
#include "ui/widgets/node/widgets/InboundOutboundWidget.hpp"

InboundNodeModel::InboundNodeModel(std::shared_ptr<NodeDispatcher> _dispatcher, std::shared_ptr<node_data_t> data) : NodeDataModel()
{
    dataptr = data;
    dispatcher = _dispatcher;
    widget = new InboundOutboundWidget(NODE_INBOUND, dispatcher);
    connect(widget, &QvNodeWidget::OnSizeUpdated, this, &InboundNodeModel::embeddedWidgetSizeUpdated);
    ((InboundOutboundWidget *) widget)->setValue(data);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_TranslucentBackground);
}

void InboundNodeModel::inputConnectionCreated(const QtNodes::Connection &){};
void InboundNodeModel::inputConnectionDeleted(const QtNodes::Connection &){};
void InboundNodeModel::outputConnectionCreated(const QtNodes::Connection &){};
void InboundNodeModel::outputConnectionDeleted(const QtNodes::Connection &){};
void InboundNodeModel::setInData(std::vector<std::shared_ptr<NodeData>>, PortIndex){};
void InboundNodeModel::onNodeHoverLeave(){};

void InboundNodeModel::onNodeHoverEnter()
{
    emit dispatcher->OnInboundOutboundNodeHovered(getTag(*dataptr.get()), GetInboundInfo(*dataptr.get()));
}
