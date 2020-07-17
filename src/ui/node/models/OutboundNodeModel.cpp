#include "ui/node/models/OutboundNodeModel.hpp"

#include "ui/node/widgets/OutboundBalancerWidget.hpp"
#include "ui/node/widgets/OutboundChainWidget.hpp"
#include "ui/node/widgets/OutboundWidget.hpp"

OutboundNodeDataModel::OutboundNodeDataModel(std::shared_ptr<complex::OutboundObjectMeta> data) : NodeDataModel()
{
    switch (data->metaType)
    {
        case complex::METAOUTBOUND_CHAINED:
        {
        }
        case complex::METAOUTBOUND_BALANCER:
        case complex::METAOUTBOUND_ORIGINAL: break;
    }

    // widget = new InboundWidget();
    // widget->setWindowFlags(Qt::FramelessWindowHint);
    // widget->setAttribute(Qt::WA_TranslucentBackground);
}

std::shared_ptr<NodeDataType> OutboundNodeDataModel::dataType(PortType portType, PortIndex portIndex) const
{
    return NODE_TYPE_OUTBOUND;
}

void OutboundNodeDataModel::setInData(std::shared_ptr<NodeData>, int)
{
    //    dataptr = data;
    //    widget->adjustSize();
}
unsigned int OutboundNodeDataModel::nPorts(PortType portType) const
{
    return portType == PortType::Out ? 0 : 1;
}

std::shared_ptr<NodeData> OutboundNodeDataModel::outData(PortIndex)
{
    return std::make_shared<OutboundNodeData>(dataptr);
}

void OutboundNodeDataModel::setData(std::shared_ptr<complex::OutboundObjectMeta> data)
{
    dataptr = data;
    widget->adjustSize();
}

void OutboundNodeDataModel::inputConnectionCreated(const QtNodes::Connection &c)
{
}

void OutboundNodeDataModel::inputConnectionDeleted(const QtNodes::Connection &c)
{
}

void OutboundNodeDataModel::outputConnectionCreated(const QtNodes::Connection &c)
{
}

void OutboundNodeDataModel::outputConnectionDeleted(const QtNodes::Connection &c)
{
}
