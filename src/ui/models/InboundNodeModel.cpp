#include "InboundNodeModel.hpp"

QvInboundNodeModel::QvInboundNodeModel(std::shared_ptr<InboundNodeData> data) : NodeDataModel()
{
    _in = data;
    widget = new InboundOutboundWidget();
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_TranslucentBackground);
}
