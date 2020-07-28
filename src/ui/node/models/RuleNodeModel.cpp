#include "ui/node/models/RuleNodeModel.hpp"

#include "core/CoreUtils.hpp"
#include "ui/node/widgets/RuleWidget.hpp"

RuleNodeModel::RuleNodeModel(std::shared_ptr<NodeDispatcher> _dispatcher, std::shared_ptr<node_data_t> data) : NodeDataModel()
{
    dataptr = data;
    dispatcher = _dispatcher;
    widget = new QvNodeRuleWidget(dispatcher);
    connect(widget, &QvNodeWidget::OnSizeUpdated, this, &NodeDataModel::embeddedWidgetSizeUpdated);
    ((QvNodeRuleWidget *) widget)->setValue(data);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_TranslucentBackground);
}

void RuleNodeModel::inputConnectionCreated(const QtNodes::Connection &c)
{
    if (dispatcher->IsNodeConstructing())
        return;
    const auto &inNodeData = convert_nodedata<InboundNodeData>(c.getNode(PortType::Out));
    const auto inboundTag = getTag(*inNodeData->GetData().get());
    if (!dataptr->inboundTag.contains(inboundTag))
        dataptr->inboundTag.push_back(inboundTag);
}

void RuleNodeModel::inputConnectionDeleted(const QtNodes::Connection &c)
{
    if (dispatcher->IsNodeConstructing())
        return;
    const auto &inNodeData = convert_nodedata<InboundNodeData>(c.getNode(PortType::Out));
    const auto inboundTag = getTag(*inNodeData->GetData().get());
    dataptr->inboundTag.removeAll(inboundTag);
}

void RuleNodeModel::outputConnectionCreated(const QtNodes::Connection &c)
{
    if (dispatcher->IsNodeConstructing())
        return;
    const auto &outbound = convert_nodedata<OutboundNodeData>(c.getNode(PortType::In));
    switch (outbound->GetData().get()->metaType)
    {
        case METAOUTBOUND_CHAIN:
        {
            QvMessageBoxWarn(nullptr, "Not Impl", "Connection to a chain has not been implemented.");
            break;
        }
        case METAOUTBOUND_EXTERNAL:
        case METAOUTBOUND_ORIGINAL:
        case METAOUTBOUND_BALANCER:
        {
            dataptr->outboundTag = outbound->GetData()->getTag();
            break;
        }
            {
                dataptr->balancerTag = outbound->GetData()->getTag();
                break;
            }
    }
}

void RuleNodeModel::outputConnectionDeleted(const QtNodes::Connection &c)
{
    if (dispatcher->IsNodeConstructing())
        return;
    dataptr->QV2RAY_RULE_USE_BALANCER = false;
    dataptr->balancerTag.clear();
    dataptr->outboundTag.clear();
}

void RuleNodeModel::setInData(std::vector<std::shared_ptr<NodeData>>, PortIndex){};
void RuleNodeModel::onNodeHoverEnter(){};
void RuleNodeModel::onNodeHoverLeave(){};
