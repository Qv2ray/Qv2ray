#include "ChainOutboundNodeModel.hpp"

ChainOutboundNodeModel::ChainOutboundNodeModel(std::shared_ptr<ChainObject>)
{
}

void ChainOutboundNodeModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex port)
{
}

void ChainOutboundNodeModel::setInData(std::vector<std::shared_ptr<NodeData>> nodeData, PortIndex port)
{
}

void ChainOutboundNodeModel::inputConnectionCreated(const QtNodes::Connection &)
{
}

void ChainOutboundNodeModel::inputConnectionDeleted(const QtNodes::Connection &)
{
}

void ChainOutboundNodeModel::outputConnectionCreated(const QtNodes::Connection &)
{
}

void ChainOutboundNodeModel::outputConnectionDeleted(const QtNodes::Connection &)
{
}
