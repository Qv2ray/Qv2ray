#include "NodeBase.hpp"
std::shared_ptr<NodeDataType> InboundNodeModel::dataType(PortType, PortIndex) const
{
    return NODE_TYPE_INBOUND;
}

std::shared_ptr<NodeDataType> OutboundNodeModel::dataType(PortType, PortIndex) const
{
    return NODE_TYPE_OUTBOUND;
}

std::shared_ptr<NodeDataType> RuleNodeModel::dataType(PortType portType, PortIndex) const
{
    switch (portType)
    {
        case PortType::In: return NODE_TYPE_INBOUND;
        case PortType::Out: return NODE_TYPE_OUTBOUND;
        default: return {};
    }
}

std::shared_ptr<NodeDataType> ChainOutboundNodeModel::dataType(PortType, PortIndex) const
{
    return NODE_TYPE_CHAINED_OUTBOUND;
}
//
// *******************************************************************************************
//

unsigned int InboundNodeModel::nPorts(PortType portType) const
{
    return portType == PortType::Out ? 1 : 0;
}

unsigned int OutboundNodeModel::nPorts(PortType portType) const
{
    return portType == PortType::Out ? 0 : 1;
}
unsigned int RuleNodeModel::nPorts(PortType) const
{
    return 1;
}
unsigned int ChainOutboundNodeModel::nPorts(PortType) const
{
    return 1;
}

//
// *******************************************************************************************
//

std::shared_ptr<NodeData> InboundNodeModel::outData(PortIndex)
{
    return std::make_shared<InboundNodeData>(dataptr);
}

std::shared_ptr<NodeData> OutboundNodeModel::outData(PortIndex)
{
    return std::make_shared<OutboundNodeData>(dataptr);
}

std::shared_ptr<NodeData> RuleNodeModel::outData(PortIndex)
{
    return std::make_shared<RuleNodeData>(dataptr);
}
std::shared_ptr<NodeData> ChainOutboundNodeModel::outData(PortIndex)
{
    return std::make_shared<ChainOutboundData>(dataptr);
}

//
// *******************************************************************************************
//
// Forwards all data to std::vector-based override function.
void InboundNodeModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex port)
{
    setInData(std::vector{ nodeData }, port);
}
void OutboundNodeModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex port)
{
    setInData(std::vector{ nodeData }, port);
}
void RuleNodeModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex port)
{
    setInData(std::vector{ nodeData }, port);
}
void ChainOutboundNodeModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex port)
{
    setInData(std::vector{ nodeData }, port);
}

//
// *******************************************************************************************
//

QtNodes::NodeDataModel::ConnectionPolicy InboundNodeModel::portInConnectionPolicy(PortIndex) const
{
    // No port
    return NodeDataModel::ConnectionPolicy::One;
}
QtNodes::NodeDataModel::ConnectionPolicy OutboundNodeModel::portInConnectionPolicy(PortIndex) const
{
    return NodeDataModel::ConnectionPolicy::Many;
}
QtNodes::NodeDataModel::ConnectionPolicy RuleNodeModel::portInConnectionPolicy(PortIndex) const
{
    return NodeDataModel::ConnectionPolicy::Many;
}
QtNodes::NodeDataModel::ConnectionPolicy ChainOutboundNodeModel::portInConnectionPolicy(PortIndex) const
{
    return NodeDataModel::ConnectionPolicy::One;
}

//
// *******************************************************************************************
//

QtNodes::NodeDataModel::ConnectionPolicy InboundNodeModel::portOutConnectionPolicy(PortIndex) const
{
    return NodeDataModel::ConnectionPolicy::Many;
}
QtNodes::NodeDataModel::ConnectionPolicy OutboundNodeModel::portOutConnectionPolicy(PortIndex) const
{
    // No port
    return NodeDataModel::ConnectionPolicy::One;
}
QtNodes::NodeDataModel::ConnectionPolicy RuleNodeModel::portOutConnectionPolicy(PortIndex) const
{
    return NodeDataModel::ConnectionPolicy::One;
}
QtNodes::NodeDataModel::ConnectionPolicy ChainOutboundNodeModel::portOutConnectionPolicy(PortIndex) const
{
    return NodeDataModel::ConnectionPolicy::One;
}
