#include "InboundNodeModel.hpp"

QvInboundNodeModel::QvInboundNodeModel(std::shared_ptr<InboundNodeData> data) : NodeDataModel()
{
    _in = data;
    widget = new InboundWidget();
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_TranslucentBackground);
    connect(widget, &InboundWidget::OnSizeUpdated, this, &QvInboundNodeModel::embeddedWidgetSizeUpdated);
}

std::unique_ptr<NodeDataModel> QvInboundNodeModel::clone() const
{
    return std::make_unique<QvInboundNodeModel>(_in);
}

std::shared_ptr<NodeData> QvInboundNodeModel::outData(PortIndex)
{
    return _in;
}

std::shared_ptr<NodeDataType> QvInboundNodeModel::dataType(PortType portType, PortIndex portIndex) const
{
    Q_UNUSED(portType)
    Q_UNUSED(portIndex)
    return NODE_TYPE_INBOUND;
}

QString QvInboundNodeModel::name() const
{
    return "InboundNode";
}

unsigned int QvInboundNodeModel::nPorts(PortType portType) const
{
    return portType == PortType::Out ? 1 : 0;
}

void QvInboundNodeModel::setData(std::shared_ptr<INBOUND> data)
{
    _in = std::make_shared<InboundNodeData>(data);
    widget->adjustSize();
}
