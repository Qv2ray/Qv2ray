#include "ui/node/models/OutboundNodeModel.hpp"

#include "core/CoreUtils.hpp"
#include "ui/node/widgets/BalancerWidget.hpp"
#include "ui/node/widgets/ChainWidget.hpp"
#include "ui/node/widgets/InboundOutboundWidget.hpp"

OutboundNodeModel::OutboundNodeModel(std::shared_ptr<NodeDispatcher> _dispatcher, std::shared_ptr<OutboundObjectMeta> data) : NodeDataModel()
{
    dataptr = data;
    dispatcher = _dispatcher;
    switch (data->metaType)
    {
        case complex::METAOUTBOUND_ORIGINAL:
        {
            widget = new InboundOutboundWidget(InboundOutboundWidget::MODE_OUTBOUND, dispatcher);
            ((InboundOutboundWidget *) widget)->setValue(data);
            break;
        }
        case complex::METAOUTBOUND_BALANCER:
        {
            widget = new BalancerWidget(dispatcher);
            ((BalancerWidget *) widget)->setValue(data);
            break;
        }
        case complex::METAOUTBOUND_CHAINED:
        {
            widget = new ChainWidget(dispatcher);
            ((ChainWidget *) widget)->setValue(data);
            break;
        }
    }
    connect(widget, &QvNodeWidget::OnSizeUpdated, this, &OutboundNodeModel::embeddedWidgetSizeUpdated);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_TranslucentBackground);
}

void OutboundNodeModel::inputConnectionCreated(const QtNodes::Connection &){};
void OutboundNodeModel::inputConnectionDeleted(const QtNodes::Connection &){};
void OutboundNodeModel::outputConnectionCreated(const QtNodes::Connection &){};
void OutboundNodeModel::outputConnectionDeleted(const QtNodes::Connection &){};
void OutboundNodeModel::setInData(std::vector<std::shared_ptr<NodeData>>, PortIndex){};

void OutboundNodeModel::onNodeHoverEnter()
{
    if (dataptr->metaType != METAOUTBOUND_ORIGINAL)
        return;
    ProtocolSettingsInfoObject o;
    if (dataptr->object.mode == MODE_JSON)
    {
        emit dispatcher->OnInboundOutboundNodeHovered(dataptr->getTag(), GetOutboundInfo(dataptr->realOutbound));
    }
    else
    {
        emit dispatcher->OnInboundOutboundNodeHovered(dataptr->getTag(), GetConnectionInfo(dataptr->object.connectionId));
    }
}

void OutboundNodeModel::onNodeHoverLeave(){};
