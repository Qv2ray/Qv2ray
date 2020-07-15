#include "ui/models/OutboundNodeModel.hpp"

QvOutboundNodeModel::QvOutboundNodeModel(std::shared_ptr<OutboundNodeData> data) : NodeDataModel()
{
    _out = data;
    widget = new InboundOutboundWidget();
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_TranslucentBackground);
}
