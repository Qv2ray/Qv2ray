#include "ChainOutboundNodeModel.hpp"

#include "ui/widgets/node/widgets/ChainOutboundWidget.hpp"

ChainOutboundNodeModel::ChainOutboundNodeModel(std::shared_ptr<NodeDispatcher> dispatcher, std::shared_ptr<node_data_t> data)
{
    this->dispatcher = dispatcher;
    widget = new ChainOutboundWidget(dispatcher);
    ((ChainOutboundWidget *) widget)->setValue(data);
}

void ChainOutboundNodeModel::setInData(std::vector<std::shared_ptr<NodeData>>, PortIndex){};
void ChainOutboundNodeModel::inputConnectionCreated(const QtNodes::Connection &){};
void ChainOutboundNodeModel::inputConnectionDeleted(const QtNodes::Connection &){};
void ChainOutboundNodeModel::outputConnectionCreated(const QtNodes::Connection &){};
void ChainOutboundNodeModel::outputConnectionDeleted(const QtNodes::Connection &){};
void ChainOutboundNodeModel::onNodeHoverLeave(){};
void ChainOutboundNodeModel::onNodeHoverEnter(){};
