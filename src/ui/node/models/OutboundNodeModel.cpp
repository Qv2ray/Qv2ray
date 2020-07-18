#include "ui/node/models/OutboundNodeModel.hpp"

#include "ui/node/widgets/OutboundBalancerWidget.hpp"
#include "ui/node/widgets/OutboundChainWidget.hpp"
#include "ui/node/widgets/OutboundWidget.hpp"

OutboundNodeModel::OutboundNodeModel(std::shared_ptr<OutboundObjectMeta> data) : NodeDataModel()
{
    switch (data->metaType)
    {
        case complex::METAOUTBOUND_ORIGINAL:
        {
            widget = new OutboundWidget();
            ((OutboundWidget *) widget)->setValue(data);
            break;
        }
        case complex::METAOUTBOUND_BALANCER:
        {
            widget = new OutboundBalancerWidget();
            ((OutboundBalancerWidget *) widget)->setValue(data);
            break;
        }
        case complex::METAOUTBOUND_CHAINED:
        {
            widget = new OutboundChainWidget();
            ((OutboundChainWidget *) widget)->setValue(data);
            break;
        }
    }
    connect(widget, &QvNodeWidget::OnSizeUpdated, this, &OutboundNodeModel::embeddedWidgetSizeUpdated);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_TranslucentBackground);
}

void OutboundNodeModel::inputConnectionCreated(const QtNodes::Connection &c)
{
}

void OutboundNodeModel::inputConnectionDeleted(const QtNodes::Connection &c)
{
}

void OutboundNodeModel::outputConnectionCreated(const QtNodes::Connection &c)
{
}

void OutboundNodeModel::outputConnectionDeleted(const QtNodes::Connection &c)
{
}
void OutboundNodeModel::setInData(std::vector<std::shared_ptr<NodeData>> nodeData, PortIndex port)
{
}
